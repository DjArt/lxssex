using System;

using static EGL.Constants;

namespace EGL
{
    [Flags]
    public enum SurfaceType : uint
    {
        MultisampleResolveBox = EGL_MULTISAMPLE_RESOLVE_BOX_BIT,
        PBuffer = EGL_PBUFFER_BIT,
        Pixmap = EGL_PIXMAP_BIT,
        SwapBehaviorPreserved = EGL_SWAP_BEHAVIOR_PRESERVED_BIT,
        VGAplhaFormatPre = EGL_VG_ALPHA_FORMAT_PRE_BIT,
        VGColorspaceLinear = EGL_VG_COLORSPACE_LINEAR_BIT,
        Window = EGL_WINDOW_BIT,
    }
}
