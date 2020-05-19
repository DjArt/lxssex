using System;
using System.Collections.Generic;
using System.Text;

namespace EGL
{
    public enum Target : uint
    {
        Texture2D,
        Texture3D,
        TextureCubeMapPositiveX,
        TextureCubeMapNegativeX,
        TextureCubeMapPositiveY,
        TextureCubeMapNegativeY,
        TextureCubeMapPositiveZ,
        TextureCubeMapNegativeZ,
        RenderBuffer
    }
}
