using System;

using static EGL.Constants;

namespace EGL
{
    [Flags]
    public enum ConformantType : uint
    {
        OpenGL = EGL_OPENGL_BIT,
        OpenGLES = EGL_OPENGL_ES_BIT,
        OpenGLES2 = EGL_OPENGL_ES2_BIT,
        OpenGLES3 = EGL_OPENGL_ES3_BIT,
        OpenVG = EGL_OPENVG_BIT,
    }
}
