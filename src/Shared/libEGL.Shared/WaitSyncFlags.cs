using System;

using static EGL.Constants;

namespace EGL
{
    [Flags]
    public enum WaitSyncFlags : uint
    {
        FlushCommands = EGL_SYNC_FLUSH_COMMANDS_BIT,
    }
}
