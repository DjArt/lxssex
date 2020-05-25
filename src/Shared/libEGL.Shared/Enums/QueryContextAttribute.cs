using static EGL.Constants;

namespace EGL.Enums
{
    public enum QueryContextAttribute : uint
    {
        ConfigID = EGL_CONFIG_ID,
        ClientType = EGL_CONTEXT_CLIENT_TYPE,
        ClientVersion = EGL_CONTEXT_CLIENT_VERSION,
        RenderBuffer = EGL_RENDER_BUFFER,
    }
}
