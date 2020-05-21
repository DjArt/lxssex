using static EGL.Constants;

namespace EGL
{
    public enum ConfigCaveat : uint
    {
        None = EGL_NONE,
        DontCare = EGL_DONT_CARE,
        SlowConfig = EGL_SLOW_CONFIG,
        NonConformantConfig = EGL_NON_CONFORMANT_CONFIG,
    }
}
