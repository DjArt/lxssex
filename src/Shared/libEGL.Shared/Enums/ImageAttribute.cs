using static EGL.Constants;

namespace EGL.Enums
{
    public enum ImageAttribute : uint
    {
        None = EGL_NONE,
        GLTextureLevel = EGL_GL_TEXTURE_LEVEL,
        GLTextureZOffset = EGL_GL_TEXTURE_ZOFFSET,
        ImagePreserved = EGL_IMAGE_PRESERVED,
    }
}
