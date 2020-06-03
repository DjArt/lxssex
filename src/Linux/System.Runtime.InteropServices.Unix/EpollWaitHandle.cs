using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;

namespace System.Runtime.InteropServices.Unix
{
    public sealed class EpollWaitHandle : WaitHandle
    {
        private const int MAX_EVENTS = 64;

        internal static int ToTimeoutMilliseconds(TimeSpan timeout)
        {
            long timeoutMilliseconds = (long)timeout.TotalMilliseconds;
            if (timeoutMilliseconds < -1)
            {
                throw new ArgumentOutOfRangeException(nameof(timeout));
            }
            if (timeoutMilliseconds > int.MaxValue)
            {
                throw new ArgumentOutOfRangeException(nameof(timeout));
            }
            return (int)timeoutMilliseconds;
        }

        public EpollWaitHandle() : this(EpollFlags.None) { }

        public EpollWaitHandle(EpollFlags flags)
        {
            SafeWaitHandle = Imports.CreateEpoll(flags);
        }

        public EpollWaitHandle(SafeWaitHandle epollHandle)
        {
            SafeWaitHandle = epollHandle;
        }

        public void AddTracking(SafeUnixHandle handle, EpollEventType events)
        {
            Imports.EpollControl(SafeWaitHandle, EpollControlOperation.Add, handle, new EpollEvent()
            {
                Data = new EpollData()
                {
                    Handle = handle,
                },
                Events = events,
            });
        }

        public void ConfigureTracking(SafeUnixHandle handle, EpollEventType events)
        {
            Imports.EpollControl(SafeWaitHandle, EpollControlOperation.Modify, handle, new EpollEvent()
            {
                Data = new EpollData()
                {
                    Handle = handle,
                },
                Events = events,
            });
        }

        public void RemoveTracking(SafeUnixHandle handle)
        {
            Imports.EpollControl(SafeWaitHandle, EpollControlOperation.Delete, handle, default);
        }

        public bool Wait(out SafeUnixHandle[] handles) => Wait(-1, out handles);
        public bool Wait(TimeSpan timeout, out SafeUnixHandle[] handles) => Wait(ToTimeoutMilliseconds(timeout), out handles);
        public bool Wait(int millisecondsTimeout, out SafeUnixHandle[] handles)
        {
            EpollEvent[] events = new EpollEvent[MAX_EVENTS];
            try
            {
                Imports.EpollWait(SafeWaitHandle, events, MAX_EVENTS, millisecondsTimeout);
                handles = events.Select(x => x.Data.Handle).TakeWhile(x => !x.IsInvalid).ToArray();
                return true;
            }
            catch
            {
                handles = null;
                return false;
            }
        }

        public override bool WaitOne() => Wait(-1, out _);
        public override bool WaitOne(int millisecondsTimeout) => Wait(millisecondsTimeout, out _);
        public override bool WaitOne(int millisecondsTimeout, bool exitContext) => throw new NotImplementedException();
        public override bool WaitOne(TimeSpan timeout) => Wait(ToTimeoutMilliseconds(timeout), out _);
        public override bool WaitOne(TimeSpan timeout, bool exitContext) => WaitOne(ToTimeoutMilliseconds(timeout), exitContext);
    }
}
