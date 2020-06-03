using System;

namespace lxssex.RPC
{
    [Flags]
    public enum ChannelEventType : uint
    {
        ChannelSync = 0b00000000,
        WindowsSide = 0b00000001,
        LinuxSide = 0b00000010,
        ChannelOpened = 0b00000100,
        ChannelClosed = 0b00001000,
    }
}
