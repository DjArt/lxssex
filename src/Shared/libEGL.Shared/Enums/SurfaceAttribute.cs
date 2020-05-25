using static EGL.Constants;

namespace EGL.Enums
{
    public enum SurfaceAttribute : uint
    {
        MipmapLevel = EGL_MIPMAP_LEVEL,
        MultisampleResolve = EGL_MULTISAMPLE_RESOLVE,
        SwapBehavior = EGL_SWAP_BEHAVIOR,
    }
}
