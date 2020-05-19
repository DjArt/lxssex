using System.Runtime.Serialization;
using System.Security.Permissions;
using System.Text;

namespace System.Runtime.InteropServices.Unix
{
    [Serializable]
    public class UnixIOException : ExternalException
    {
        private static string GetErrorMessage(int error)
        {
            StringBuilder msg = new StringBuilder(256);
            int status = Imports.StrError(error, msg, 0);
            return status >= 0 ? msg.ToString() : string.Format("Unknown error (0x{0:x})", error);
        }

        public int NativeErrorCode { get; }

        [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
        public UnixIOException() : this(Marshal.GetLastWin32Error()) { }

        [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
        public UnixIOException(int error) : this(error, GetErrorMessage(error)) { }

        [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
        public UnixIOException(string message) : this(Marshal.GetLastWin32Error(), message) { }

        public UnixIOException(int error, string message) : base(message)
        {
            NativeErrorCode = error;
        }

        public UnixIOException(string message, Exception innerException) : base(message, innerException) { }

        protected UnixIOException(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            NativeErrorCode = info.GetInt32("NativeErrorCode");
        }

        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info == null)
            {
                throw new ArgumentNullException("info");
            }

            info.AddValue("NativeErrorCode", NativeErrorCode);
            base.GetObjectData(info, context);
        }
    }
}
