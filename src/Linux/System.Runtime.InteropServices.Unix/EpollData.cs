namespace System.Runtime.InteropServices.Unix
{
    internal struct EpollData
    {
        public IntPtr Ptr;
        public SafeUnixHandle Handle;
        public int U32;
        public long U64;
    }
}
