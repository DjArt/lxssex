using static EGL.Constants;

namespace EGL.Enums
{
    public enum PBufferSurfaceAttribute : uint
    {
        None = EGL_NONE,
        GLColorspace = EGL_GL_COLORSPACE,
        Height = EGL_HEIGHT,
        LargestPBuffer = EGL_LARGEST_PBUFFER,
        MipmapTexture = EGL_MIPMAP_TEXTURE,
        TextureFormat = EGL_TEXTURE_FORMAT,
        TextureTarget = EGL_TEXTURE_TARGET,
        VGAlphaFormat = EGL_VG_ALPHA_FORMAT,
        VGColorspace = EGL_VG_COLORSPACE,
        Width = EGL_WIDTH
    }
}
