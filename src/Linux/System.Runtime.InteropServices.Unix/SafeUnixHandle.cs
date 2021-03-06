﻿using Microsoft.Win32.SafeHandles;
using System.Runtime.ConstrainedExecution;
using System.Security.Permissions;

namespace System.Runtime.InteropServices.Unix
{
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode = true)]
    [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
    public class SafeUnixHandle : SafeHandleMinusOneIsInvalid
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        private SafeUnixHandle() : base(true) { }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle() => Imports.Close(handle) != -1;
    }
}
