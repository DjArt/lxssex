namespace lxssex.WindowManager.Shared
{
    public enum WMError : uint
    {
        Success = 0x0,
        InternalError = 0xFFFFFFF,
        WindowNotFound = 0xFFFFFFFE,
        ArgumentException = 0xFFFFFFD,
    }
}
