using static EGL.Constants;

namespace EGL.Enums
{
    public enum API : uint
    {
        [AttributeParameterCount(0)]
        OpenGLES = EGL_OPENGL_ES_API,
        [AttributeParameterCount(0)]
        OpenVG = EGL_OPENVG_API,
        [AttributeParameterCount(0)]
        OpenGL = EGL_OPENGL_API,
    }
}
