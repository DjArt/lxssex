﻿using System;
using System.Collections.Generic;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace lxssex.RPC
{
    public class Channel : IDisposable
    {
        private static readonly MethodInfo[] PRIMITIVE_WRITE_METHODS;
        private static readonly MethodInfo[] PRIMITIVE_READ_METHODS;
        private static readonly MethodInfo GENERIC_WRITE_METHOD;
        private static readonly MethodInfo GENERIC_WRITE_ARRAY_METHOD;
        private static readonly MethodInfo GENERIC_READ_METHOD;
        private static readonly MethodInfo GENERIC_READ_ARRAY_METHOD;

        static Channel()
        {
            PRIMITIVE_WRITE_METHODS = typeof(UnmanagedMemoryAccessor).GetTypeInfo().DeclaredMethods.Where(x => x.IsPublic && !x.IsStatic && !x.ContainsGenericParameters && x.Name == "Write").ToArray();
            PRIMITIVE_READ_METHODS = typeof(UnmanagedMemoryAccessor).GetTypeInfo().DeclaredMethods.Where(x => x.IsPublic && !x.IsStatic && !x.ContainsGenericParameters && x.Name.StartsWith("Read") && x.ReturnType.IsPrimitive).ToArray();
            GENERIC_WRITE_METHOD = typeof(UnmanagedMemoryAccessor).GetTypeInfo().DeclaredMethods.First(x => x.IsPublic && !x.IsStatic && x.ContainsGenericParameters && x.Name == "Write");
            GENERIC_WRITE_ARRAY_METHOD = typeof(UnmanagedMemoryAccessor).GetTypeInfo().DeclaredMethods.First(x => x.IsPublic && !x.IsStatic && x.ContainsGenericParameters && x.Name == "WriteArray");
            GENERIC_READ_METHOD = typeof(UnmanagedMemoryAccessor).GetTypeInfo().DeclaredMethods.First(x => x.IsPublic && !x.IsStatic && x.ContainsGenericParameters && x.Name == "Read");
            GENERIC_READ_ARRAY_METHOD = typeof(UnmanagedMemoryAccessor).GetTypeInfo().DeclaredMethods.First(x => x.IsPublic && !x.IsStatic && x.ContainsGenericParameters && x.Name == "ReadArray");
        }

        private static MethodInfo GetWriter(Type dataType)
        {
            if (dataType.IsArray)
            {
                return GENERIC_WRITE_ARRAY_METHOD.MakeGenericMethod(dataType.GetElementType());
            }
            else if (dataType.IsPrimitive)
            {
                return PRIMITIVE_WRITE_METHODS.First(x => x.GetParameters()[1].ParameterType == dataType);
            }
            else
            {
                return GENERIC_WRITE_METHOD.MakeGenericMethod(dataType);
            }
        }

        private static MethodInfo GetReader(Type dataType)
        {
            if (dataType.IsArray)
            {
                return GENERIC_READ_ARRAY_METHOD.MakeGenericMethod(dataType.GetElementType());
            }
            else if (dataType.IsPrimitive)
            {
                return PRIMITIVE_READ_METHODS.First(x => x.ReturnType == dataType);
            }
            else
            {
                return GENERIC_READ_METHOD.MakeGenericMethod(dataType);
            }
        }

        private bool disposedValue;

        public TypeInfo ServicingType { get; }
        public object ServicingObject { get; }
        public SafeBuffer SharedMemory { get; }
        public WaitHandle SyncEvent { get; }
        public ChannelGetEventDelegate GetEvent { get; }
        public ChannelSetEventDelegate SetEvent { get; }

        public Channel(Type servicingType, SafeBuffer sharedMemory, WaitHandle syncEvent, ChannelGetEventDelegate getEvent, ChannelSetEventDelegate setEvent)
        {
            ServicingType = servicingType.GetTypeInfo();
            SharedMemory = sharedMemory;
            SyncEvent = syncEvent;
            GetEvent = getEvent;
            SetEvent = setEvent;
        }

        public Channel(object servicingObject, SafeBuffer sharedMemory, WaitHandle syncEvent, ChannelGetEventDelegate getEvent, ChannelSetEventDelegate setEvent) : this(servicingObject.GetType(), sharedMemory, syncEvent, getEvent, setEvent)
        {
            ServicingObject = servicingObject;
        }

        private string ReadData(out object[] values, out object @return)
        {
            using UnmanagedMemoryAccessor accessor = new UnmanagedMemoryAccessor(SharedMemory, 0, (long)SharedMemory.ByteLength, FileAccess.ReadWrite);

            long pos = sizeof(long);
            int encodedCallerNameLength = accessor.ReadInt32(pos);
            pos += sizeof(int);
            byte[] encodedCallerName = new byte[encodedCallerNameLength];
            accessor.ReadArray(pos, encodedCallerName, 0, encodedCallerNameLength);
            string callerName = Encoding.UTF8.GetString(encodedCallerName);

            MethodInfo caller = ServicingType.GetDeclaredMethod(callerName);
            if (caller is null) throw new MissingMemberException(ServicingType.FullName, callerName);

            ParameterInfo[] @params = caller.GetParameters();
            values = new object[@params.Length];

            return ReadData(@params, values, caller.ReturnParameter, out @return);
        }

        private string ReadData(ParameterInfo[] @params, object[] values, ParameterInfo returnParam, out object @return)
        {
            using UnmanagedMemoryAccessor accessor = new UnmanagedMemoryAccessor(SharedMemory, 0, (long)SharedMemory.ByteLength, FileAccess.ReadWrite);
            long resultBytes = accessor.ReadInt64(0);
            long pos = sizeof(long);
            int encodedCallerNameLength = accessor.ReadInt32(pos);
            pos += sizeof(int);
            byte[] encodedCallerName = new byte[encodedCallerNameLength];
            accessor.ReadArray(pos, encodedCallerName, 0, encodedCallerNameLength);
            pos += encodedCallerNameLength;

            object Deserialize(ParameterInfo param)
            {
                if (param == null)
                {
                    return null;
                }
                else if (param.ParameterType.IsArray)
                {
                    Type elementType = param.ParameterType.GetElementType();
                    MethodInfo readMethod = GetReader(param.ParameterType);
                    int arrayLength = accessor.ReadInt32(pos);
                    Array array = Array.CreateInstance(elementType, arrayLength);
                    pos += sizeof(int);
                    readMethod.Invoke(accessor, new object[] { pos, array, 0, arrayLength });
                    pos += arrayLength * Marshal.SizeOf(elementType);
                    return array;
                }
                else if (param.ParameterType == typeof(string))
                {
                    int encodedStringLength = accessor.ReadInt32(pos);
                    pos += sizeof(int);
                    byte[] encodedString = new byte[encodedStringLength];
                    accessor.ReadArray(pos, encodedString, 0, encodedStringLength);
                    pos += encodedStringLength;
                    return Encoding.UTF8.GetString(encodedString);
                }
                else if (param.ParameterType == typeof(IntPtr))
                {
                    long value = accessor.ReadInt64(pos);
                    pos += sizeof(long);
                    return new IntPtr(value);
                }
                else if (param.ParameterType == typeof(UIntPtr))
                {
                    ulong value = accessor.ReadUInt64(pos);
                    pos += sizeof(ulong);
                    return new UIntPtr(value);
                }
                else
                {
                    Type valueType = param.ParameterType.IsByRef ? param.ParameterType.GetElementType() : param.ParameterType;
                    MethodInfo readMethod = GetReader(valueType);
                    object value = null;
                    if (valueType.IsPrimitive)
                    {
                        value = readMethod.Invoke(accessor, new object[] { pos });
                    }
                    else
                    {
                        object[] readParams = new object[] { pos, null };
                        readMethod.Invoke(accessor, readParams);
                        value = readParams[1];
                    }
                    Console.WriteLine("{0}: {1} readed", valueType.FullName, value);
                    pos += Marshal.SizeOf(valueType);
                    return value;
                }
            }

            for (int i0 = 0; i0 < @params.Length; i0++)
            {
                values[i0] = Deserialize(@params[i0]);
            }
            @return = Deserialize(returnParam);
            if (resultBytes != pos) throw new DataMisalignedException();
            return Encoding.UTF8.GetString(encodedCallerName);
        }

        private void WriteData(string callerName, ParameterInfo[] @params, object[] values, ParameterInfo returnParam, object @return = null)
        {
            using (UnmanagedMemoryAccessor accessor = new UnmanagedMemoryAccessor(SharedMemory, 0, (long)SharedMemory.ByteLength, FileAccess.ReadWrite))
            {
                long pos = sizeof(long);
                byte[] encodedName = Encoding.UTF8.GetBytes(callerName);
                accessor.Write(pos, encodedName.Length);
                pos += sizeof(int);
                accessor.WriteArray(pos, encodedName, 0, encodedName.Length);
                pos += encodedName.Length;

                void Serialize(ParameterInfo param, object value)
                {
                    if (param == null)
                    {
                        //skip
                    }
                    else if (param.ParameterType.IsArray)
                    {
                        MethodInfo writeMethod = GetWriter(param.ParameterType);
                        Array array = (Array)value;
                        accessor.Write(pos, array.Length);
                        pos += sizeof(int);
                        writeMethod.Invoke(accessor, new object[] { pos, array, 0, array.Length });
                        pos += array.Length * Marshal.SizeOf(param.ParameterType.GetElementType());
                    }
                    else if (param.ParameterType == typeof(string))
                    {
                        byte[] encodedString = Encoding.UTF8.GetBytes((string)value);
                        accessor.Write(pos, encodedString.Length);
                        pos += sizeof(int);
                        accessor.WriteArray(pos, encodedString, 0, encodedString.Length);
                        pos += encodedString.Length;
                    }
                    else if (param.ParameterType == typeof(IntPtr))
                    {
                        accessor.Write(pos, ((IntPtr)value).ToInt64());
                        pos += sizeof(long);
                    }
                    else if (param.ParameterType == typeof(UIntPtr))
                    {
                        accessor.Write(pos, ((UIntPtr)value).ToUInt64());
                        pos += sizeof(ulong);
                    }
                    else
                    {
                        Type valueType = param.ParameterType.IsByRef ? param.ParameterType.GetElementType() : param.ParameterType;
                        MethodInfo writeMethod = GetWriter(valueType);
                        Console.WriteLine("{0}: {1} written", valueType.FullName, value);
                        writeMethod.Invoke(accessor, new[] { pos, value });
                        pos += Marshal.SizeOf(valueType);
                    }
                }
                
                for (int i0 = 0; i0 < @params.Length; i0++)
                {
                    Serialize(@params[i0], values[i0]);
                }
                Serialize(returnParam, @return);
                accessor.Write(0, pos);
            }
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    SyncEvent.Dispose();
                    SharedMemory.Dispose();
                }
                disposedValue = true;
            }
        }

        public object SendMessage(object[] args, [CallerMemberName] string callerName = null)
        {
            if (disposedValue)
            {
                throw new ObjectDisposedException(nameof(Channel));
            }
            else
            {
                MethodInfo method = ServicingType.GetDeclaredMethod(callerName);
                ParameterInfo[] @params = method.GetParameters();
                WriteData(callerName, @params, args, method.ReturnParameter);

                //Notify other side
                SetEvent(ChannelEventType.ChannelSync);
                //Wait while other side complete request
                SyncEvent.WaitOne();
                //Check incoming event
                switch (GetEvent())
                {
                    case ChannelEventType.ChannelSync:
                    case ChannelEventType.ChannelSync | ChannelEventType.LinuxSide:
                    case ChannelEventType.ChannelSync | ChannelEventType.WindowsSide:
                    case ChannelEventType.ChannelOpened:
                    case ChannelEventType.ChannelOpened | ChannelEventType.LinuxSide:
                    case ChannelEventType.ChannelOpened | ChannelEventType.WindowsSide:
                    case ChannelEventType.ChannelOpened | ChannelEventType.LinuxSide | ChannelEventType.WindowsSide:
                        break;
                    case ChannelEventType.ChannelClosed:
                    case ChannelEventType.ChannelClosed | ChannelEventType.LinuxSide:
                    case ChannelEventType.ChannelClosed | ChannelEventType.WindowsSide:
                    case ChannelEventType.ChannelClosed | ChannelEventType.LinuxSide | ChannelEventType.WindowsSide:
                        Dispose();
                        throw new OperationCanceledException();
                    default:
                        throw new NotSupportedException();
                }

                List<ParameterInfo> byRef = @params.Where(x => x.IsOut && x.ParameterType.IsByRef).ToList();
                object @return = null;
                if (byRef.Count > 0 || method.ReturnParameter != null)
                {
                    if (ReadData(@params, args, method.ReturnParameter, out @return) != callerName)
                    {
                        throw new InvalidDataException();
                    }
                }
                return @return;
            }
        }

        public void ReceiveMessage()
        {
            if (disposedValue)
            {
                throw new ObjectDisposedException(nameof(Channel));
            }
            else
            {
                //Wait while other side send request
                SyncEvent.WaitOne();
                //Check incoming event
                switch (GetEvent())
                {
                    case ChannelEventType.ChannelSync:
                    case ChannelEventType.ChannelSync | ChannelEventType.LinuxSide:
                    case ChannelEventType.ChannelSync | ChannelEventType.WindowsSide:
                    case ChannelEventType.ChannelOpened:
                    case ChannelEventType.ChannelOpened | ChannelEventType.LinuxSide:
                    case ChannelEventType.ChannelOpened | ChannelEventType.WindowsSide:
                    case ChannelEventType.ChannelOpened | ChannelEventType.LinuxSide | ChannelEventType.WindowsSide:
                        break;
                    case ChannelEventType.ChannelClosed:
                    case ChannelEventType.ChannelClosed | ChannelEventType.LinuxSide:
                    case ChannelEventType.ChannelClosed | ChannelEventType.WindowsSide:
                    case ChannelEventType.ChannelClosed | ChannelEventType.LinuxSide | ChannelEventType.WindowsSide:
                        Dispose();
                        throw new OperationCanceledException();
                    default:
                        throw new NotSupportedException();
                }

                string callerName = ReadData(out object[] args, out _);

                MethodInfo method = ServicingType.GetDeclaredMethod(callerName);
                ParameterInfo[] @params = method.GetParameters();

                object @return = method.Invoke(ServicingObject, args);

                List<ParameterInfo> byRef = @params.Where(x => x.IsOut && x.ParameterType.IsByRef).ToList();
                if (byRef.Count > 0 || method.ReturnParameter != null)
                {
                    WriteData(callerName, @params, args, method.ReturnParameter, @return);
                }

                //Notify other side
                SetEvent(ChannelEventType.ChannelSync);
            }
        }

        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }
}
