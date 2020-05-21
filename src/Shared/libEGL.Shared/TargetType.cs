using System;
using System.Collections.Generic;
using System.Text;

namespace EGL
{
    public enum TargetType : uint
    {
        Texture2D = 0x30B1,
        Texture3D = 0x30B2,
        TextureCubeMapPositiveX = 0x30B3,
        TextureCubeMapNegativeX = 0x30B4,
        TextureCubeMapPositiveY = 0x30B5,
        TextureCubeMapNegativeY = 0x30B6,
        TextureCubeMapPositiveZ = 0x30B7,
        TextureCubeMapNegativeZ = 0x30B8,
        RenderBuffer = 0x30B9
    }
}
