namespace System.Runtime.InteropServices.Unix
{
    [Flags]
    public enum EpollEventType : uint
    {
        None = 0x00000000,
        In = 0x00000001,
        PRI = 0x00000002,
        Out = 0x00000004,
        Error = 0x00000008,
        HangUp = 0x00000010,
        RDNORM = 0x00000040,
        RDBAND = 0x00000080,
        WRNORM = 0x00000100,
        WRBAND = 0x00000200,
        Message = 0x00000400,
        RDHUP = 0x00002000,
        WakeUp = 0x10000000,
        Exclusive = 0x20000000,
        OneShot = 0x40000000,
        EdgeTriggered = 0x80000000,
    }
}
