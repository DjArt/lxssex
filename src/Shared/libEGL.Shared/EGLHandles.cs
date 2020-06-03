using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Text;

namespace EGL
{
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode = true)]
    [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
    public abstract class SafeEGLHandle : SafeHandleMinusOneIsInvalid
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal SafeEGLHandle() : base(true) { }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle() => true;
    }

    public sealed class EGLDisplay : SafeEGLHandle { }
    public sealed class EGLSurface : SafeEGLHandle { }
    public sealed class EGLContext : SafeEGLHandle { }
    public sealed class EGLNativePixmapType : SafeEGLHandle { }
    public sealed class EGLNativeWindowType : SafeEGLHandle { }
    public sealed class EGLNativeDisplayType : SafeEGLHandle { }
    public sealed class EGLConfig : SafeEGLHandle { }
    public sealed class EGLSync : SafeEGLHandle { }
    public sealed class EGLClientBuffer : SafeEGLHandle { }
    public sealed class EGLImage : SafeEGLHandle { }
}
