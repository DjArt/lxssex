using System;
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
    public class MessageHandler
    {
        private static readonly MethodInfo[] PRIMITIVE_WRITE_METHODS = typeof(MemoryMappedViewAccessor).GetTypeInfo().DeclaredMethods.Where(x => x.IsPublic && !x.IsStatic && !x.ContainsGenericParameters && x.Name == "Write").ToArray();
        private static readonly MethodInfo GENERIC_WRITE_METHOD = typeof(MemoryMappedViewAccessor).GetTypeInfo().DeclaredMethods.First(x => x.IsPublic && !x.IsStatic && x.ContainsGenericParameters && x.Name == "Write");

        private static MethodInfo GetWriter(Type dataType)
        {
            return dataType.IsPrimitive ? PRIMITIVE_WRITE_METHODS.First(x => x.GetParameters()[1].ParameterType == dataType) : GENERIC_WRITE_METHOD.MakeGenericMethod(dataType);
        }

        public TypeInfo ServicingType { get; }
        public MemoryMappedFile SharedMemory { get; }
        public WaitHandle SyncEvent { get; }

        public MessageHandler(Type servicingType, MemoryMappedFile sharedMemory, WaitHandle syncEvent)
        {
            ServicingType = servicingType.GetTypeInfo();
            SharedMemory = sharedMemory;
            SyncEvent = syncEvent;
        }

        private string ReadData(ParameterInfo[] @params, object[] values, ParameterInfo returnParam, out object @return)
        {
            @return = null;
            return null;
        }

        private void WriteData(string callerName, ParameterInfo[] @params, object[] values, ParameterInfo returnParam, object @return = null)
        {
            byte[] encodedName = Encoding.UTF8.GetBytes(callerName);
            using (MemoryMappedViewAccessor accessor = SharedMemory.CreateViewAccessor())
            {
                long pos = sizeof(long);
                accessor.Write(pos, encodedName.Length);
                pos += sizeof(int);
                accessor.WriteArray(pos, encodedName, 0, encodedName.Length);
                pos += encodedName.Length;
                ParameterInfo param = null;
                object value = null;
                for (int i0 = 0; i0 < @params.Length; i0++, param = @params[i0], value = values[i0])
                {
                    if (param.ParameterType.IsArray)
                    {
                        MethodInfo writeMethod = GetWriter(param.ParameterType.GetElementType());
                        Array array = (Array)value;
                        accessor.Write(pos, array.Length);
                        pos += sizeof(int);
                        for (int i1 = 0; i1 < array.Length; i1++)
                        {
                            object item = array.GetValue(i1);
                            writeMethod.Invoke(accessor, new[] { pos, item });
                            pos += Marshal.SizeOf(item);
                        }
                    }
                    else if (param.ParameterType == typeof(string))
                    {

                    }
                    else
                    {
                        MethodInfo writeMethod = GetWriter(param.ParameterType);
                        writeMethod.Invoke(accessor, new[] { pos, value });
                        pos += Marshal.SizeOf(value);
                    }
                }
            }
        }

        public object SendMessage(object[] args, [CallerMemberName]string callerName = null)
        {
            MethodInfo method = ServicingType.GetDeclaredMethod(callerName);
            ParameterInfo[] @params = method.GetParameters();
            WriteData(callerName, @params, args, method.ReturnParameter);
            //Wait while other side complete request
            SyncEvent.WaitOne();
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
}
