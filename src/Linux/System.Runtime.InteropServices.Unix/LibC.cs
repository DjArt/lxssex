using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Text;

namespace System.Runtime.InteropServices.Unix
{
    public static class LibC
    {
        public static uint Ioctl<T>(SafeUnixHandle handle, int request, ref T data)
            where T : unmanaged
        {
            unsafe
            {
                fixed (T* p = &data)
                {
                    int status = Imports.Ioctl(handle, request, p);
                    if (status < 0) throw new UnixIOException(status);
                    return (uint)status;
                }
            }
        }

        public static SafeMemoryMappedViewUnixHandle MMap(IntPtr addr, ulong length, MappingProtection protection, MappingFlags flags, SafeUnixHandle fileDescriptor, long offset)
        {
            SafeMemoryMappedViewUnixHandle result = Imports.MMap(addr, length, protection, flags, fileDescriptor?.DangerousGetHandle() ?? new IntPtr(-1), 0);
            result.Initialize(length);
            return result;
        }

        public static SafeUnixHandle Open(string path, uint flags, int mode)
        {
            return Imports.Open(path, flags, mode);
        }
    }
}
