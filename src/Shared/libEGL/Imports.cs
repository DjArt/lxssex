using EGL;
using EGL.Enums;
using System;
using System.Runtime.InteropServices;

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

namespace libEGL
{
    public static class Imports
    {
        [DllImport("libEGL")]
        public static extern bool eglChooseConfig(EGLDisplay display, ConfigAttribute[] attrib_list, ref EGLConfig[] configs, int configSize, out int numConfig);
        [DllImport("libEGL")]
        public static extern bool eglCopyBuffers(EGLDisplay display, EGLSurface surface, EGLNativePixmapType target);
        [DllImport("libEGL")]
        public static extern EGLContext eglCreateContext(EGLDisplay display, EGLConfig config, EGLContext share_context, ContextAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern EGLSurface eglCreatePbufferSurface(EGLDisplay display, EGLConfig config, PBufferSurfaceAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern EGLSurface eglCreatePixmapSurface(EGLDisplay display, EGLConfig config, EGLNativePixmapType pixmap, PixmapSurfaceAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig config, EGLNativeWindowType win, WindowSurfaceAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern bool eglDestroyContext(EGLDisplay display, EGLContext ctx);
        [DllImport("libEGL")]
        public static extern bool eglDestroySurface(EGLDisplay display, EGLSurface surface);
        [DllImport("libEGL")]
        public static extern bool eglGetConfigAttrib(EGLDisplay display, EGLConfig config, ConfigAttribute attribute, out uint value);
        [DllImport("libEGL")]
        public static extern bool eglGetConfigs(EGLDisplay display, ref EGLConfig[] configs, int configSize, out int numConfig);
        [DllImport("libEGL")]
        public static extern EGLDisplay eglGetCurrentDisplay();
        [DllImport("libEGL")]
        public static extern EGLSurface eglGetCurrentSurface(SurfaceAccessType readdraw);
        [DllImport("libEGL")]
        public static extern EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id);
        [DllImport("libEGL")]
        public static extern ErrorType eglGetError();
        [DllImport("libEGL")]
        public static extern bool eglInitialize(EGLDisplay display, ref int majorVersion, ref int minorVersion);
        [DllImport("libEGL")]
        public static extern bool eglMakeCurrent(EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext ctx);
        [DllImport("libEGL")]
        public static extern bool eglQueryContext(EGLDisplay display, EGLContext ctx, QueryContextAttribute attribute, out uint value);
        [DllImport("libEGL")]
        public static extern string eglQueryString(EGLDisplay display, QueryStringType name);
        [DllImport("libEGL")]
        public static extern bool eglQuerySurface(EGLDisplay display, EGLSurface surface, QuerySurfaceAttribute attribute, out uint value);
        [DllImport("libEGL")]
        public static extern bool eglSwapBuffers(EGLDisplay display, EGLSurface surface);
        [DllImport("libEGL")]
        public static extern bool eglTerminate(EGLDisplay display);
        [DllImport("libEGL")]
        public static extern bool eglWaitGL();
        [DllImport("libEGL")]
        public static extern bool eglWaitNative(WaitNativeEngine engine);
        [DllImport("libEGL")]
        public static extern bool eglBindTexImage(EGLDisplay display, EGLSurface surface, BufferType buffer);
        [DllImport("libEGL")]
        public static extern bool eglReleaseTexImage(EGLDisplay display, EGLSurface surface, BufferType buffer);
        [DllImport("libEGL")]
        public static extern bool eglSurfaceAttrib(EGLDisplay display, EGLSurface surface, SurfaceAttribute attribute, uint value);
        [DllImport("libEGL")]
        public static extern bool eglSwapInterval(EGLDisplay display, int interval);
        [DllImport("libEGL")]
        public static extern bool eglBindAPI(API api);
        [DllImport("libEGL")]
        public static extern API eglQueryAPI();
        [DllImport("libEGL")]
        public static extern EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay display, Buffer2Type buftype, EGLClientBuffer buffer, EGLConfig config, PBufferAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern bool eglReleaseThread();
        [DllImport("libEGL")]
        public static extern bool eglWaitClient();
        [DllImport("libEGL")]
        public static extern EGLContext eglGetCurrentContext();
        [DllImport("libEGL")]
        public static extern EGLSync eglCreateSync(EGLDisplay display, SyncType type, SyncAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern bool eglDestroySync(EGLDisplay display, EGLSync sync);
        [DllImport("libEGL")]
        public static extern WaitSyncResult eglClientWaitSync(EGLDisplay display, EGLSync sync, WaitSyncFlags flags, ulong nanosecondsTimeout);
        [DllImport("libEGL")]
        public static extern bool eglGetSyncAttrib(EGLDisplay display, EGLSync sync, SyncAttribute attribute, out uint value);
        [DllImport("libEGL")]
        public static extern EGLImage eglCreateImage(EGLDisplay display, EGLContext ctx, TextureType target, EGLClientBuffer buffer, ImageAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern bool eglDestroyImage(EGLDisplay display, EGLImage image);
        [DllImport("libEGL")]
        public static extern EGLDisplay eglGetPlatformDisplay(uint platform, IntPtr nativeDisplay, uint[] attrib_list);
        [DllImport("libEGL")]
        public static extern EGLSurface eglCreatePlatformWindowSurface(EGLDisplay display, EGLConfig config, IntPtr native_window, WindowSurfaceAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay display, EGLConfig config, IntPtr native_pixmap, PixmapSurfaceAttribute[] attrib_list);
        [DllImport("libEGL")]
        public static extern bool eglWaitSync(EGLDisplay display, EGLSync sync, WaitSyncFlags flags);
    }
}
