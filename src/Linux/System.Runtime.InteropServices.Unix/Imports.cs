using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace System.Runtime.InteropServices.Unix
{
    internal static class Imports
    {
        [DllImport("libc", EntryPoint = "close", SetLastError = true)]
        internal static extern int Close(IntPtr handle);
        [DllImport("libc", EntryPoint = "epoll_create1", SetLastError = true)]
        internal static extern SafeWaitHandle CreateEpoll(EpollFlags flags);
        [DllImport("libc", EntryPoint = "epoll_ctl", SetLastError = true)]
        internal static extern int EpollControl(SafeWaitHandle epollHandle, EpollControlOperation operation, SafeUnixHandle handle, in EpollEvent @event);
        [DllImport("libc", EntryPoint = "epoll_wait", SetLastError = true)]
        internal static extern int EpollWait(SafeWaitHandle epollHandle, [Out] EpollEvent[] events, int maxEvents, int millisecondsTimeout);
        [DllImport("libc", EntryPoint = "ioctl", SetLastError = true)]
        internal static extern unsafe int Ioctl(SafeUnixHandle handle, int request, void* data);
        [DllImport("libc", EntryPoint = "mmap", SetLastError = false)]
        internal static extern SafeMemoryMappedViewUnixHandle MMap(IntPtr addr, ulong length, MappingProtection protection, MappingFlags flags, IntPtr fileDescriptor, long offset);
        [DllImport("libc", EntryPoint = "munmap", SetLastError = true)]
        internal static extern int MUnmap(IntPtr addr, ulong length);
        [DllImport("libc", EntryPoint = "open", SetLastError = true)]
        internal static extern SafeUnixHandle Open([MarshalAs(UnmanagedType.LPStr)]string path, uint flags, int mode);
        [DllImport("libc", EntryPoint = "strerror_r", SetLastError = true)]
        internal static extern int StrError(int error, [MarshalAs(UnmanagedType.LPStr)]StringBuilder buffer, ulong length);
    }
}
