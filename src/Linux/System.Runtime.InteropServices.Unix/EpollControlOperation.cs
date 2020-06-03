namespace System.Runtime.InteropServices.Unix
{
    internal enum EpollControlOperation : uint
    {
        Add = 0x1,
        Delete = 0x2,
        Modify = 0x3,
    }
}
