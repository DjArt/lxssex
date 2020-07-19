using EGL.Enums;
using lxssex.RPC;
using Microsoft.Win32.SafeHandles;
using System;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Unix;
using System.Threading;

//using EGLSyncKHR = System.IntPtr;
//using EGLImageKHR = System.IntPtr;
//using EGLDeviceEXT = System.IntPtr;
//using EGLObjectKHR = System.IntPtr;
//using EGLSetBlobFuncANDROID = System.IntPtr;
//using EGLGetBlobFuncANDROID = System.IntPtr;
//using AHardwareBuffer = System.IntPtr;
//using EGLFrameTokenANGLE = System.IntPtr;
//using EGLnsecsANDROID = System.IntPtr;
//using EGLLabelKHR = System.IntPtr;
//using EGLStreamKHR = System.IntPtr;
//using EGLDEBUGPROCKHR = System.IntPtr;

namespace EGL
{
    public static class Exports
    {
        private static Channel RPC { get; }
        private static SafeUnixHandle lxssex { get; }

        static Exports()
        {
            lxssex = LibC.Open("/dev/lxssex", 0, 0);
            EpollWaitHandle lxssexEvent = new EpollWaitHandle();
            lxssexEvent.AddTracking(lxssex, EpollEventType.In | EpollEventType.Out);
            SafeMemoryMappedViewUnixHandle buf = LibC.MMap(IntPtr.Zero, 1024, MappingProtection.Read | MappingProtection.Write, MappingFlags.Shared, lxssex, 0);
            RPC = new Channel(typeof(Exports), buf, lxssexEvent, GetEvent, SetEvent);
        }

        private static ChannelEventType GetEvent()
        {
            ChannelEventType result = ChannelEventType.ChannelSync;
            LibC.Ioctl(lxssex, 1, ref result);
            return result;
        }

        private static void SetEvent(ChannelEventType eventType)
        {
            LibC.Ioctl(lxssex, 2, ref eventType);
        }

        private static void EnsureEGLLoaded() { }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglChooseConfig))]
        public static bool eglChooseConfig(EGLDisplay display, ConfigAttribute[] attrib_list, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)] EGLConfig[] configs, int configSize, out int numConfig)
        {
            EnsureEGLLoaded();
            object[] @params = new object[] { display, attrib_list, configs, configSize, null };
            bool result = (bool)RPC.SendMessage(@params);
            numConfig = (int)@params[4];
            return result;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCopyBuffers))]
        public static bool eglCopyBuffers(EGLDisplay display, EGLSurface surface, EGLNativePixmapType target)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreateContext))]
        public static EGLContext eglCreateContext(EGLDisplay display, EGLConfig config, EGLContext share_context, ContextAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePbufferSurface))]
        public static EGLSurface eglCreatePbufferSurface(EGLDisplay display, EGLConfig config, PBufferSurfaceAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePixmapSurface))]
        public static EGLSurface eglCreatePixmapSurface(EGLDisplay display, EGLConfig config, EGLNativePixmapType pixmap, PixmapSurfaceAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreateWindowSurface))]
        public static EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig config, EGLNativeWindowType win, WindowSurfaceAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglDestroyContext))]
        public static bool eglDestroyContext(EGLDisplay display, EGLContext ctx)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglDestroySurface))]
        public static bool eglDestroySurface(EGLDisplay display, EGLSurface surface)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetConfigAttrib))]
        public static bool eglGetConfigAttrib(EGLDisplay display, EGLConfig config, ConfigAttribute attribute, out uint value)
        {
            EnsureEGLLoaded();
            value = 0;
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetConfigs))]
        public static bool eglGetConfigs(EGLDisplay display, ref EGLConfig[] configs, int configSize, out int numConfig)
        {
            EnsureEGLLoaded();
            numConfig = 0;
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetCurrentDisplay))]
        public static EGLDisplay eglGetCurrentDisplay()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetCurrentSurface))]
        public static EGLSurface eglGetCurrentSurface(SurfaceAccessType readdraw)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetDisplay))]
        public static EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetError))]
        public static ErrorType eglGetError()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglInitialize))]
        public static bool eglInitialize(IntPtr display, ref int majorVersion, ref int minorVersion)
        {
            EnsureEGLLoaded();
            return (bool)RPC.SendMessage(new object[] { display, majorVersion, minorVersion });
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglMakeCurrent))]
        public static bool eglMakeCurrent(EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext ctx)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglQueryContext))]
        public static bool eglQueryContext(EGLDisplay display, EGLContext ctx, QueryContextAttribute attribute, out uint value)
        {
            EnsureEGLLoaded();
            value = 0;
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglQueryString))]
        public static string eglQueryString(EGLDisplay display, QueryStringType name)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglQuerySurface))]
        public static bool eglQuerySurface(EGLDisplay display, EGLSurface surface, QuerySurfaceAttribute attribute, out uint value)
        {
            EnsureEGLLoaded();
            value = 0;
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglSwapBuffers))]
        public static bool eglSwapBuffers(EGLDisplay display, EGLSurface surface)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglTerminate))]
        public static bool eglTerminate(EGLDisplay display)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglWaitGL))]
        public static bool eglWaitGL()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglWaitNative))]
        public static bool eglWaitNative(WaitNativeEngine engine)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglBindTexImage))]
        public static bool eglBindTexImage(EGLDisplay display, EGLSurface surface, BufferType buffer)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglReleaseTexImage))]
        public static bool eglReleaseTexImage(EGLDisplay display, EGLSurface surface, BufferType buffer)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglSurfaceAttrib))]
        public static bool eglSurfaceAttrib(EGLDisplay display, EGLSurface surface, SurfaceAttribute attribute, uint value)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglSwapInterval))]
        public static bool eglSwapInterval(EGLDisplay display, int interval)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglBindAPI))]
        public static bool eglBindAPI(API api)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglQueryAPI))]
        public static API eglQueryAPI()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePbufferFromClientBuffer))]
        public static EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay display, Buffer2Type buftype, EGLClientBuffer buffer, EGLConfig config, PBufferAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglReleaseThread))]
        public static bool eglReleaseThread()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglWaitClient))]
        public static bool eglWaitClient()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetCurrentContext))]
        public static EGLContext eglGetCurrentContext()
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreateSync))]
        public static EGLSync eglCreateSync(EGLDisplay display, SyncType type, SyncAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglDestroySync))]
        public static bool eglDestroySync(EGLDisplay display, EGLSync sync)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglClientWaitSync))]
        public static WaitSyncResult eglClientWaitSync(EGLDisplay display, EGLSync sync, WaitSyncFlags flags, ulong nanosecondsTimeout)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetSyncAttrib))]
        public static bool eglGetSyncAttrib(EGLDisplay display, EGLSync sync, SyncAttribute attribute, out uint value)
        {
            EnsureEGLLoaded();
            value = 0;
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreateImage))]
        public static EGLImage eglCreateImage(EGLDisplay display, EGLContext ctx, TextureType target, EGLClientBuffer buffer, ImageAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglDestroyImage))]
        public static bool eglDestroyImage(EGLDisplay display, EGLImage image)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglGetPlatformDisplay))]
        public static EGLDisplay eglGetPlatformDisplay(uint platform, IntPtr nativeDisplay, uint[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePlatformWindowSurface))]
        public static EGLSurface eglCreatePlatformWindowSurface(EGLDisplay display, EGLConfig config, IntPtr native_window, WindowSurfaceAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePlatformPixmapSurface))]
        public static EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay display, EGLConfig config, IntPtr native_pixmap, PixmapSurfaceAttribute[] attrib_list)
        {
            EnsureEGLLoaded();
            return default;
        }

        [UnmanagedCallersOnly(EntryPoint = nameof(eglWaitSync))]
        public static bool eglWaitSync(EGLDisplay display, EGLSync sync, WaitSyncFlags flags)
        {
            EnsureEGLLoaded();
            return default;
        }

        #region Extension
        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQuerySurfacePointerANGLE))]
        //public static bool eglQuerySurfacePointerANGLE(EGLDisplay display, EGLSurface surface, EGLint attribute, ref IntPtr value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglPostSubBufferNV))]
        //public static bool eglPostSubBufferNV(EGLDisplay display, EGLSurface surface, int x, int y, int width, int height)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetPlatformDisplayEXT))]
        //public static EGLDisplay eglGetPlatformDisplayEXT(EGLenum platform, IntPtr native_display, ref EGLint attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePlatformWindowSurfaceEXT))]
        //public static EGLSurface eglCreatePlatformWindowSurfaceEXT(EGLDisplay display, EGLConfig config, IntPtr native_window, ref EGLint attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglCreatePlatformPixmapSurfaceEXT))]
        //public static EGLSurface eglCreatePlatformPixmapSurfaceEXT(EGLDisplay display, EGLConfig config, IntPtr native_pixmap, ref EGLint attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryDisplayAttribEXT))]
        //public static bool eglQueryDisplayAttribEXT(EGLDisplay display, EGLint attribute, EGLAttrib[] value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryDisplayAttribANGLE))]
        //public static bool eglQueryDisplayAttribANGLE(EGLDisplay display, EGLint attribute, EGLAttrib[] value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryDeviceAttribEXT))]
        //public static bool eglQueryDeviceAttribEXT(EGLDeviceEXT device, EGLint attribute, EGLAttrib[] value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryDeviceStringEXT))]
        //public static string eglQueryDeviceStringEXT(EGLDeviceEXT device, EGLint name)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglCreateImageKHR))]
        //public static EGLImageKHR eglCreateImageKHR(EGLDisplay display, EGLContext ctx, TextureType target, EGLClientBuffer buffer, ref EGLint attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglDestroyImageKHR))]
        //public static bool eglDestroyImageKHR(EGLDisplay display, EGLImageKHR image)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglCreateDeviceANGLE))]
        //public static EGLDeviceEXT eglCreateDeviceANGLE(EGLint device_type, IntPtr native_device, EGLAttrib[] attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglReleaseDeviceANGLE))]
        //public static bool eglReleaseDeviceANGLE(EGLDeviceEXT device)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetProcAddress))]
        //public static IntPtr eglGetProcAddress(string procname)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglCreateStreamKHR))]
        //public static EGLStreamKHR eglCreateStreamKHR(EGLDisplay display, ref EGLint attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglDestroyStreamKHR))]
        //public static bool eglDestroyStreamKHR(EGLDisplay display, EGLStreamKHR stream)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglStreamAttribKHR))]
        //public static bool eglStreamAttribKHR(EGLDisplay display, EGLStreamKHR stream, EGLenum attribute, EGLint value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryStreamKHR))]
        //public static bool eglQueryStreamKHR(EGLDisplay display, EGLStreamKHR stream, EGLenum attribute, ref EGLint value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryStreamu64KHR))]
        //public static bool eglQueryStreamu64KHR(EGLDisplay display, EGLStreamKHR stream, EGLenum attribute, ref ulong value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglStreamConsumerGLTextureExternalKHR))]
        //public static bool eglStreamConsumerGLTextureExternalKHR(EGLDisplay display, EGLStreamKHR stream)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglStreamConsumerAcquireKHR))]
        //public static bool eglStreamConsumerAcquireKHR(EGLDisplay display, EGLStreamKHR stream)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglStreamConsumerReleaseKHR))]
        //public static bool eglStreamConsumerReleaseKHR(EGLDisplay display, EGLStreamKHR stream)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglStreamConsumerGLTextureExternalAttribsNV))]
        //public static bool eglStreamConsumerGLTextureExternalAttribsNV(EGLDisplay display, EGLStreamKHR stream, EGLAttrib[] attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglCreateStreamProducerD3DTextureANGLE))]
        //public static bool eglCreateStreamProducerD3DTextureANGLE(EGLDisplay display, EGLStreamKHR stream, EGLAttrib[] attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglStreamPostD3DTextureANGLE))]
        //public static bool eglStreamPostD3DTextureANGLE(EGLDisplay display, EGLStreamKHR stream, IntPtr texture, EGLAttrib[] attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetSyncValuesCHROMIUM))]
        //public static bool eglGetSyncValuesCHROMIUM(EGLDisplay display, EGLSurface surface, ref ulong ust, ref ulong msc, ref ulong sbc)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetMscRateANGLE))]
        //public static bool eglGetMscRateANGLE(EGLDisplay display, EGLSurface surface, ref EGLint numerator, ref EGLint denominator)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglSwapBuffersWithDamageKHR))]
        //public static bool eglSwapBuffersWithDamageKHR(EGLDisplay display, EGLSurface surface, ref EGLint rects, EGLint n_rects)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglPresentationTimeANDROID))]
        //public static bool eglPresentationTimeANDROID(EGLDisplay display, EGLSurface surface, EGLnsecsANDROID time)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglProgramCacheGetAttribANGLE))]
        //public static EGLint eglProgramCacheGetAttribANGLE(EGLDisplay display, EGLenum attrib)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglProgramCacheQueryANGLE))]
        //public static void eglProgramCacheQueryANGLE(EGLDisplay display, EGLint index, IntPtr key, ref EGLint keysize, IntPtr binary, ref EGLint binarysize)
        //{
        //    EnsureEGLLoaded();
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglProgramCachePopulateANGLE))]
        //public static void eglProgramCachePopulateANGLE(EGLDisplay display, IntPtr key, EGLint keysize, IntPtr binary, EGLint binarysize)
        //{
        //    EnsureEGLLoaded();
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglProgramCacheResizeANGLE))]
        //public static EGLint eglProgramCacheResizeANGLE(EGLDisplay display, EGLint limit, EGLenum mode)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglDebugMessageControlKHR))]
        //public static EGLint eglDebugMessageControlKHR(EGLDEBUGPROCKHR callback, EGLAttrib[] attrib_list)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryDebugKHR))]
        //public static bool eglQueryDebugKHR(EGLint attribute, EGLAttrib[] value)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglLabelObjectKHR))]
        //public static EGLint eglLabelObjectKHR(EGLDisplay display, EGLenum objectType, EGLObjectKHR @object, EGLLabelKHR label)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglSetBlobCacheFuncsANDROID))]
        //public static void eglSetBlobCacheFuncsANDROID(EGLDisplay display, EGLSetBlobFuncANDROID set, EGLGetBlobFuncANDROID get)
        //{
        //    EnsureEGLLoaded();
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetCompositorTimingSupportedANDROID))]
        //public static bool eglGetCompositorTimingSupportedANDROID(EGLDisplay display, EGLSurface surface, EGLint name)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetCompositorTimingANDROID))]
        //public static bool eglGetCompositorTimingANDROID(EGLDisplay display, EGLSurface surface, EGLint numTimestamps, ref EGLint names, EGLnsecsANDROID[] values)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetNextFrameIdANDROID))]
        //public static bool eglGetNextFrameIdANDROID(EGLDisplay display, EGLSurface surface, ref ulong frameId)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetFrameTimestampSupportedANDROID))]
        //public static bool eglGetFrameTimestampSupportedANDROID(EGLDisplay display, EGLSurface surface, EGLint timestamp)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetFrameTimestampsANDROID))]
        //public static bool eglGetFrameTimestampsANDROID(EGLDisplay display, EGLSurface surface, ulong frameId, EGLint numTimestamps, ref EGLint timestamps, EGLnsecsANDROID[] values)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglQueryStringiANGLE))]
        //public static string eglQueryStringiANGLE(EGLDisplay display, EGLint name, EGLint index)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglGetNativeClientBufferANDROID))]
        //public static EGLClientBuffer eglGetNativeClientBufferANDROID(ref AHardwareBuffer buffer)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglDupNativeFenceFDANDROID))]
        //public static EGLint eglDupNativeFenceFDANDROID(EGLDisplay display, EGLSyncKHR sync)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}

        //[UnmanagedCallersOnly(EntryPoint = nameof(eglSwapBuffersWithFrameTokenANGLE))]
        //public static bool eglSwapBuffersWithFrameTokenANGLE(EGLDisplay display, EGLSurface surface, EGLFrameTokenANGLE frametoken)
        //{
        //    EnsureEGLLoaded();
        //    return default;
        //}
        #endregion
    }
}
