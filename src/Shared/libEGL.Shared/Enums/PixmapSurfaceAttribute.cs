using static EGL.Constants;

namespace EGL.Enums
{
    public enum PixmapSurfaceAttribute : uint
    {
        None = EGL_NONE,
        GLColorspace = EGL_GL_COLORSPACE,
        VGAlphaFormat = EGL_VG_ALPHA_FORMAT,
        VGColorspace = EGL_VG_COLORSPACE,
    }
}
