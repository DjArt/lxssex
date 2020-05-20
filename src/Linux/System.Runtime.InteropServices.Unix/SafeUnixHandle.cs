using System.Runtime.ConstrainedExecution;
using System.Security.Permissions;

namespace System.Runtime.InteropServices.Unix
{
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode = true)]
    [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
    public class SafeUnixHandle : SafeHandle
    {
        private static readonly IntPtr INVALID = new IntPtr(-1);

        public override bool IsInvalid => handle == INVALID;

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        private SafeUnixHandle() : base(INVALID, true) { }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle() => Imports.Close(handle) != -1;
    }
}
