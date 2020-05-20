using System;
using System.Collections.Generic;
using System.Text;

namespace System.Runtime.InteropServices.Unix
{
    [Flags]
    public enum MappingProtection : uint
    {
        /// <summary>
        /// Page cannot be accessed
        /// </summary>
        None = 0x0,
        /// <summary>
        /// Page can be read
        /// </summary>
        Read = 0x1,
        /// <summary>
        /// Page can be written
        /// </summary>
        Write = 0x2,
        /// <summary>
        /// Page can be executed
        /// </summary>
        Execution = 0x4,
        /// <summary>
        /// Page may be used for atomic operations
        /// </summary>
        Sem = 0x8,
    }
}
