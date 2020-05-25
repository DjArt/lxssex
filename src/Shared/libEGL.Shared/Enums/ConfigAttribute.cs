using static EGL.Constants;

namespace EGL.Enums
{
    public enum ConfigAttribute : uint
    {
        None = EGL_NONE,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired alpha mask buffer size, in bits. The smallest alpha mask buffers of at least the specified size are preferred. The default value is zero. 
        /// The alpha mask buffer is used only by OpenVG.
        /// </summary>
        AlphaMaskSize = EGL_ALPHA_MASK_SIZE,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired size of the alpha component of the color buffer, in bits. If this value is zero, color buffers with the smallest alpha component size are preferred. Otherwise, color buffers with the largest alpha component of at least the specified size are preferred. The default value is zero.
        /// </summary>
        AlphaSize = EGL_ALPHA_SIZE,
        /// <summary>
        /// Must be followed by EGL_DONT_CARE, EGL_TRUE, or EGL_FALSE. If EGL_TRUE is specified, then only frame buffer configurations that support binding of color buffers to an OpenGL ES RGB texture will be considered. Currently only frame buffer configurations that support pbuffers allow this. The default value is EGL_DONT_CARE.
        /// </summary>
        BindToTextureRGB = EGL_BIND_TO_TEXTURE_RGB,
        /// <summary>
        /// Must be followed by one of EGL_DONT_CARE, EGL_TRUE, or EGL_FALSE. If EGL_TRUE is specified, then only frame buffer configurations that support binding of color buffers to an OpenGL ES RGBA texture will be considered. Currently only frame buffer configurations that support pbuffers allow this. The default value is EGL_DONT_CARE.
        /// </summary>
        BindToTextureRGBA = EGL_BIND_TO_TEXTURE_RGBA,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired size of the blue component of the color buffer, in bits. If this value is zero, color buffers with the smallest blue component size are preferred. Otherwise, color buffers with the largest blue component of at least the specified size are preferred. The default value is zero.
        /// </summary>
        BlueSize = EGL_BLUE_SIZE,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired color buffer size, in bits. The smallest color buffers of at least the specified size are preferred. The default value is zero. 
        /// The color buffer size is the sum of EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, and EGL_ALPHA_SIZE, and does not include any padding bits which may be present in the pixel format. It is usually preferable to specify desired sizes for these color components individually. 
        /// </summary>
        BufferSize = EGL_BUFFER_SIZE,
        /// <summary>
        /// Must be followed by one of EGL_RGB_BUFFER or EGL_LUMINANCE_BUFFER. 
        /// EGL_RGB_BUFFER indicates an RGB color buffer; in this case, attributes EGL_RED_SIZE, EGL_GREEN_SIZE and EGL_BLUE_SIZE must be non-zero, and EGL_LUMINANCE_SIZE must be zero. 
        /// EGL_LUMINANCE_BUFFER indicates a luminance color buffer. In this case EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE must be zero, and EGL_LUMINANCE_SIZE must be non-zero. 
        /// For both RGB and luminance color buffers, EGL_ALPHA_SIZE may be zero or non-zero.
        /// </summary>
        ColorBufferType = EGL_COLOR_BUFFER_TYPE,
        /// <summary>
        /// Must be followed by EGL_DONT_CARE, EGL_NONE, EGL_SLOW_CONFIG, or EGL_NON_CONFORMANT_CONFIG. 
        /// If EGL_DONT_CARE is specified, then configs are not matched for this attribute. The default value is EGL_DONT_CARE. 
        /// If EGL_NONE is specified, then configs are matched for this attribute, but only configs with no caveats (neither EGL_SLOW_CONFIG or EGL_NON_CONFORMANT_CONFIG) will be considered. 
        /// If EGL_SLOW_CONFIG is specified, then only slow configs configurations will be considered. The meaning of``slow'' is implementation-dependent, but typically indicates a non-hardware-accelerated (software) implementation. 
        /// If EGL_NON_CONFORMANT_CONFIG is specified, then only configs supporting non-conformant OpenGL ES contexts will be considered. 
        /// If the EGL version is 1.3 or later, caveat EGL_NON_CONFORMANT_CONFIG is obsolete, since the same information can be specified via the EGL_CONFORMANT attribute on a per-client-API basis, not just for OpenGL ES.
        /// </summary>
        ConfigCaveat = EGL_CONFIG_CAVEAT,
        /// <summary>
        /// Must be followed by a valid integer ID that indicates the desired EGL frame buffer configuration. When a EGL_CONFIG_ID is specified, all other attributes are ignored. The default value is EGL_DONT_CARE. 
        /// The meaning of config IDs is implementation-dependent. They are used only to uniquely identify different frame buffer configurations. 
        /// </summary>
        ConfigID = EGL_CONFIG_ID,
        /// <summary>
        /// Must be followed by a bitmask indicating which types of client API contexts created with respect to the frame buffer configuration config must pass the required conformance tests for that API. Mask bits include: 
        /// EGL_OPENGL_BIT
        /// Config supports creating OpenGL contexts. 
        /// EGL_OPENGL_ES_BIT
        /// Config supports creating OpenGL ES 1.0 and/or 1.1 contexts. 
        /// EGL_OPENGL_ES2_BIT
        /// Config supports creating OpenGL ES 2.0 contexts. 
        /// EGL_OPENVG_BIT
        /// Config supports creating OpenVG contexts. 
        /// For example, if the bitmask is set to EGL_OPENGL_ES_BIT, only frame buffer configurations that support creating conformant OpenGL ES contexts will match. The default value is zero. 
        /// Most EGLConfigs should be conformant for all supported client APIs, and it is rarely desirable to select a nonconformant config. Conformance requirements limit the number of non-conformant configs that an implementation can define.
        /// </summary>
        Conformant = EGL_CONFORMANT,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired depth buffer size, in bits. The smallest depth buffers of at least the specified size is preferred. If the desired size is zero, frame buffer configurations with no depth buffer are preferred. The default value is zero. 
        /// The depth buffer is used only by OpenGL and OpenGL ES client APIs. 
        /// </summary>
        DepthSize = EGL_DEPTH_SIZE,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired size of the green component of the color buffer, in bits. If this value is zero, color buffers with the smallest green component size are preferred. Otherwise, color buffers with the largest green component of at least the specified size are preferred. The default value is zero.
        /// </summary>
        GreenSize = EGL_GREEN_SIZE,
        /// <summary>
        /// Must be followed by an integer buffer level specification. This specification is honored exactly. Buffer level zero corresponds to the default frame buffer of the display. Buffer level one is the first overlay frame buffer, level two the second overlay frame buffer, and so on. Negative buffer levels correspond to underlay frame buffers. The default value is zero. 
        /// Most platforms do not support buffer levels other than zero. The behavior of windows placed in overlay and underlay planes depends on the underlying platform. 
        /// </summary>
        Level = EGL_LEVEL,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired size of the luminance component of the color buffer, in bits. If this value is zero, color buffers with the smallest luminance component size are preferred. Otherwise, color buffers with the largest luminance component of at least the specified size are preferred. The default value is zero.
        /// </summary>
        LuminanceSize = EGL_LUMINANCE_SIZE,
        /// <summary>
        /// Must be followed by the handle of a valid native pixmap, cast to EGLint, or EGL_NONE. If the value is not EGL_NONE, only configs which support creating pixmap surfaces with this pixmap using eglCreatePixmapSurface will match this attribute. If the value is EGL_NONE, then configs are not matched for this attribute. The default value is EGL_NONE. 
        /// EGL_MATCH_NATIVE_PIXMAP was introduced due to the difficulty of determining an EGLConfig compatibile with a native pixmap using only color component sizes.
        /// </summary>
        MatchNativePixmap = EGL_MATCH_NATIVE_PIXMAP,
        /// <summary>
        /// Must be followed by EGL_DONT_CARE, EGL_TRUE, or EGL_FALSE. If EGL_TRUE is specified, then only frame buffer configurations that allow native rendering into the surface will be considered. The default value is EGL_DONT_CARE.
        /// </summary>
        NativeRenderable = EGL_NATIVE_RENDERABLE,
        /// <summary>
        /// Must be followed by a integer that indicates the maximum value that can be passed to eglSwapInterval. The default value is EGL_DONT_CARE.
        /// </summary>
        MaxSwapInterval = EGL_MAX_SWAP_INTERVAL,
        /// <summary>
        /// Must be followed by a integer that indicates the minimum value that can be passed to eglSwapInterval. The default value is EGL_DONT_CARE.
        /// </summary>
        MinSwapInterval = EGL_MIN_SWAP_INTERVAL,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired size of the red component of the color buffer, in bits. If this value is zero, color buffers with the smallest red component size are preferred. Otherwise, color buffers with the largest red component of at least the specified size are preferred. The default value is zero.
        /// </summary>
        RedSize = EGL_RED_SIZE,
        /// <summary>
        /// Must be followed by the minimum acceptable number of multisample buffers. Configurations with the smallest number of multisample buffers that meet or exceed this minimum number are preferred. Currently operation with more than one multisample buffer is undefined, so only values of zero or one will produce a match. The default value is zero.
        /// </summary>
        SampleBuffers = EGL_SAMPLE_BUFFERS,
        /// <summary>
        /// Must be followed by the minimum number of samples required in multisample buffers. Configurations with the smallest number of samples that meet or exceed the specified minimum number are preferred. Note that it is possible for color samples in the multisample buffer to have fewer bits than colors in the main color buffers. However, multisampled colors maintain at least as much color resolution in aggregate as the main color buffers.
        /// </summary>
        Samples = EGL_SAMPLES,
        /// <summary>
        /// Must be followed by a nonnegative integer that indicates the desired stencil buffer size, in bits. The smallest stencil buffers of at least the specified size are preferred. If the desired size is zero, frame buffer configurations with no stencil buffer are preferred. The default value is zero. 
        /// The stencil buffer is used only by OpenGL and OpenGL ES client APIs.
        /// </summary>
        StencilSize = EGL_STENCIL_SIZE,
        /// <summary>
        /// Must be followed by a bitmask indicating which types of client API contexts the frame buffer configuration must support creating with eglCreateContext). Mask bits are the same as for attribute EGL_CONFORMANT. The default value is EGL_OPENGL_ES_BIT.
        /// </summary>
        RenderableType = EGL_RENDERABLE_TYPE,
        /// <summary>
        /// Must be followed by a bitmask indicating which EGL surface types and capabilities the frame buffer configuration must support. Mask bits include: 
        /// EGL_MULTISAMPLE_RESOLVE_BOX_BIT
        /// Config allows specifying box filtered multisample resolve behavior with eglSurfaceAttrib. 
        /// EGL_PBUFFER_BIT
        /// Config supports creating pixel buffer surfaces. 
        /// EGL_PIXMAP_BIT
        /// Config supports creating pixmap surfaces. 
        /// EGL_SWAP_BEHAVIOR_PRESERVED_BIT
        /// Config allows setting swap behavior for color buffers with eglSurfaceAttrib. 
        /// EGL_VG_ALPHA_FORMAT_PRE_BIT
        /// Config allows specifying OpenVG rendering with premultiplied alpha values at surface creation time (see eglCreatePbufferSurface, eglCreatePixmapSurface, and eglCreateWindowSurface). 
        /// EGL_VG_COLORSPACE_LINEAR_BIT
        /// Config allows specifying OpenVG rendering in a linear colorspace at surface creation time (see eglCreatePbufferSurface, eglCreatePixmapSurface, and eglCreateWindowSurface). 
        /// EGL_WINDOW_BIT
        /// Config supports creating window surfaces. 
        /// For example, if the bitmask is set to EGL_WINDOW_BIT | EGL_PIXMAP_BIT, only frame buffer configurations that support both windows and pixmaps will be considered. The default value is EGL_WINDOW_BIT.
        /// </summary>
        SurfaceType = EGL_SURFACE_TYPE,
        /// <summary>
        /// Must be followed by one of EGL_NONE or EGL_TRANSPARENT_RGB. If EGL_NONE is specified, then only opaque frame buffer configurations will be considered. If EGL_TRANSPARENT_RGB is specified, then only transparent frame buffer configurations will be considered. The default value is EGL_NONE. 
        /// Most implementations support only opaque frame buffer configurations. 
        /// </summary>
        TransparentType = EGL_TRANSPARENT_TYPE,
        /// <summary>
        /// Must be followed by an integer value indicating the transparent red value. The value must be between zero and the maximum color buffer value for red. Only frame buffer configurations that use the specified transparent red value will be considered. The default value is EGL_DONT_CARE. 
        /// This attribute is ignored unless EGL_TRANSPARENT_TYPE is included in attrib_list and specified as EGL_TRANSPARENT_RGB.
        /// </summary>
        TransparentRedValue = EGL_TRANSPARENT_RED_VALUE,
        /// <summary>
        /// Must be followed by an integer value indicating the transparent green value. The value must be between zero and the maximum color buffer value for green. Only frame buffer configurations that use the specified transparent green value will be considered. The default value is EGL_DONT_CARE. 
        /// This attribute is ignored unless EGL_TRANSPARENT_TYPE is included in attrib_list and specified as EGL_TRANSPARENT_RGB.
        /// </summary>
        TransparentGreenValue = EGL_TRANSPARENT_GREEN_VALUE,
        /// <summary>
        /// Must be followed by an integer value indicating the transparent blue value. The value must be between zero and the maximum color buffer value for blue. Only frame buffer configurations that use the specified transparent blue value will be considered. The default value is EGL_DONT_CARE. 
        /// This attribute is ignored unless EGL_TRANSPARENT_TYPE is included in attrib_list and specified as EGL_TRANSPARENT_RGB. 
        /// </summary>
        TransparentBlueValue = EGL_TRANSPARENT_BLUE_VALUE,
    }
}
