using static EGL.Constants;

namespace EGL.Enums
{
    public enum WindowSurfaceAttribute : uint
    {
        None = EGL_NONE,
        GLColorspace = EGL_GL_COLORSPACE,
        RederBuffer = EGL_RENDER_BUFFER,
        VGAlphaFormat = EGL_VG_ALPHA_FORMAT,
        VGColorspace = EGL_VG_COLORSPACE,
    }
}
