using System;
using System.Collections.Generic;
using System.Text;

namespace System.Runtime.InteropServices.Unix
{
    [Flags]
    public enum MappingFlags : uint
    {
        None = 0x0000,
        /// <summary>
        /// Share changes
        /// </summary>
        Shared = 0x0001,
        /// <summary>
        /// Changes are private
        /// </summary>
        Private = 0x0002,
        /// <summary>
        /// Map address must be exactly as requested
        /// </summary>
        Fixed = 0x0010,
        /// <summary>
        /// Allocated from memory, swap space
        /// </summary>
        Anonymous = 0x0020,
    }
}
