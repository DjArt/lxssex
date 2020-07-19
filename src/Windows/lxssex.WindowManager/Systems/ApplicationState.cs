using System;

namespace lxssex.WindowManager.Systems
{
    [Flags]
    public enum ApplicationState : uint
    {
        Suspended = 0x0,
        Background = 0x1,
        Foreground = 0x2,
        Stopped = 0x4,
    }
}
