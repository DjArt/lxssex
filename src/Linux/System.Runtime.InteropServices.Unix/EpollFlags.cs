namespace System.Runtime.InteropServices.Unix
{
    [Flags]
    public enum EpollFlags : uint
    {
        None = 0,
        CloseOnExecution = 0x2000000
    }
}
