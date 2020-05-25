using static EGL.Constants;

namespace EGL.Enums
{
    public enum SyncAttribute : uint
    {
        None = EGL_NONE,
        Status = EGL_SYNC_STATUS,
        Type = EGL_SYNC_TYPE,
        Condition = EGL_SYNC_CONDITION,
    }
}
