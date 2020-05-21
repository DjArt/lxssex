using static EGL.Constants;

namespace EGL
{
    public enum API : uint
    {
        OpenGLES = EGL_OPENGL_ES_API,
        OpenVG = EGL_OPENVG_API,
        OpenGL = EGL_OPENGL_API,
    }
}
