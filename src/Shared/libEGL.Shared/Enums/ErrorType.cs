using static EGL.Constants;

namespace EGL.Enums
{
    public enum ErrorType : uint
    {
        Success = EGL_SUCCESS,
        NotInitialized = EGL_NOT_INITIALIZED,
        BadAccess = EGL_BAD_ACCESS,
        BadAlloc = EGL_BAD_ALLOC,
        BadAttribute = EGL_BAD_ATTRIBUTE,
        BadContext = EGL_BAD_CONTEXT,
        BadConfig = EGL_BAD_CONFIG,
        BadCurrentSurface = EGL_BAD_CURRENT_SURFACE,
        BadDisplay = EGL_BAD_DISPLAY,
        BadSurface = EGL_BAD_SURFACE,
        BadMatch = EGL_BAD_MATCH,
        BadParameter = EGL_BAD_PARAMETER,
        BadNativePixmap = EGL_BAD_NATIVE_PIXMAP,
        BadNativeWindow = EGL_BAD_NATIVE_WINDOW,
        ContextLost = EGL_CONTEXT_LOST,
    }
}
