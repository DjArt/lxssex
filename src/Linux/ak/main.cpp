#include <stdlib.h>
#include <unistd.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

static EGLint const attribute_list[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_NONE
};

int main(int argc, char** argv)
{
    int major = 255;
    int minor = 128;
    eglInitialize(NULL, &major, &minor);
    return EXIT_SUCCESS;
}