using static EGL.Constants;

namespace EGL.Enums
{
    public enum ContextAttribute : uint
    {
        MajorVersion = EGL_CONTEXT_MAJOR_VERSION,
        MinrVersion = EGL_CONTEXT_MINOR_VERSION,
        OpenGLProfileMask = EGL_CONTEXT_OPENGL_PROFILE_MASK,
        OpenGLDebug = EGL_CONTEXT_OPENGL_DEBUG,
        OpenGLForwardCompatible = EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE,
        OpenGLRobustAccess = EGL_CONTEXT_OPENGL_ROBUST_ACCESS,
        OpenGLResetNotificationStrategy = EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY,
    }
}
