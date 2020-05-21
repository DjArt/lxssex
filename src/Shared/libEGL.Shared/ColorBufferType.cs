using static EGL.Constants;

namespace EGL
{
    public enum ColorBufferType : uint
    {
        /// <summary>
        /// EGL_RGB_BUFFER indicates an RGB color buffer; in this case, attributes EGL_RED_SIZE, EGL_GREEN_SIZE and EGL_BLUE_SIZE must be non-zero, and EGL_LUMINANCE_SIZE must be zero.
        /// </summary>
        RGBBuffer = EGL_RGB_BUFFER,
        /// <summary>
        /// EGL_LUMINANCE_BUFFER indicates a luminance color buffer. In this case EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE must be zero, and EGL_LUMINANCE_SIZE must be non-zero.
        /// </summary>
        LuminanceBuffer = EGL_LUMINANCE_BUFFER,
    }
}
