using static EGL.Constants;

namespace EGL.Enums
{
    public enum WaitSyncResult : uint
    {
        Error = EGL_FALSE,
        TimeoutExpired = EGL_TIMEOUT_EXPIRED,
        ConditionSatisfied = EGL_CONDITION_SATISFIED,
    }
}
