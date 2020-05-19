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
                    int status = Imports.Ioctl(handle, request, (IntPtr)p);
                    if (status < 0) throw new UnixIOException(status);
                    return (uint)status;
                }
            }
        }

        public static SafeUnixHandle Open(string path, uint flags, int mode)
        {
            SafeUnixHandle result = Imports.Open(path, flags, mode);
            if (result.IsInvalid) throw new UnixIOException();
            return result;
        }
    }
}
