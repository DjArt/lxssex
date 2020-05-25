using static EGL.Constants;

namespace EGL.Enums
{
    public enum TextureType : uint
    {
        NoTexture = EGL_NO_TEXTURE,
        TextureRGB = EGL_TEXTURE_RGB,
        TextureRGBA = EGL_TEXTURE_RGBA,
        Texture2D = EGL_GL_TEXTURE_2D,
        Texture3D = EGL_GL_TEXTURE_3D,
        TextureCubeMapPositiveX = EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        TextureCubeMapNegativeX = EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        TextureCubeMapPositiveY = EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        TextureCubeMapNegativeY = EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        TextureCubeMapPositiveZ = EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        TextureCubeMapNegativeZ = EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        RenderBuffer = EGL_GL_RENDERBUFFER,
    }
}
