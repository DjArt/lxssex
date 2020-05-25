using static EGL.Constants;

namespace EGL.Enums
{
    public enum QuerySurfaceAttribute : uint
    {
        ConfigID = EGL_CONFIG_ID,
        GLColorspace = EGL_GL_COLORSPACE,
        Height = EGL_HEIGHT,
        HorizontalResolution = EGL_HORIZONTAL_RESOLUTION,
        LargestPBuffer = EGL_LARGEST_PBUFFER,
        MipmapLevel = EGL_MIPMAP_LEVEL,
        MipmapTexture = EGL_MIPMAP_TEXTURE,
        MultisampleResolve = EGL_MULTISAMPLE_RESOLVE,
        PixelAspectRatio = EGL_PIXEL_ASPECT_RATIO,
        RenderBuffer = EGL_RENDER_BUFFER,
        SwapBehavior = EGL_SWAP_BEHAVIOR,
        TextureFormat = EGL_TEXTURE_FORMAT,
        TextureTarget = EGL_TEXTURE_TARGET,
        VerticalResolution = EGL_VERTICAL_RESOLUTION,
        VGAlphaFormat = EGL_VG_ALPHA_FORMAT,
        VGColorspace = EGL_VG_COLORSPACE,
        Width = EGL_WIDTH
    }
}
