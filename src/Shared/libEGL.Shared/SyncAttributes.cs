using System;
using System.Collections.Generic;
using System.Text;

namespace EGL
{
    public enum SyncAttributes : uint
    {
        None = 0x3038,
        Status = 0x30F1,
        Type = 0x30F7,
        Condition = 0x30F8,
    }
}
