using static EGL.Constants;

namespace EGL.Enums
{
    public enum PBufferAttribute : uint
    {
        None = EGL_NONE,
        [AttributeParameterCount(1)]
        MipmapTexture = EGL_MIPMAP_TEXTURE,
        [AttributeParameterCount(1)]
        TextureFormat = EGL_TEXTURE_FORMAT,
        [AttributeParameterCount(1)]
        TextureTarget = EGL_TEXTURE_TARGET,
    }
}
