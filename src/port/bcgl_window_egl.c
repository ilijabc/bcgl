#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <sys/time.h>

#ifdef __ANDROID__
#include <android/native_window.h>
#endif

#include "../bcgl_internal.h"

typedef struct
{
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
} BCEGLWindow;

static long s_StartTimeSec = -1;


static void initClock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    s_StartTimeSec = tv.tv_sec;
}

float bcGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec - s_StartTimeSec) + (0.000001f * tv.tv_usec);
}

// Window
BCWindow * bcCreateWindow(BCConfig *inconfig)
{
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_SAMPLE_BUFFERS, (inconfig->msaa ? 1 : 0),
        EGL_SAMPLES, inconfig->msaa,
        EGL_NONE
    };

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLDisplay display;
    EGLConfig config;
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;
    GLfloat ratio;

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        bcLogError("eglGetDisplay() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglInitialize(display, 0, 0)) {
        bcLogError("eglInitialize() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        bcLogError("eglChooseConfig() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        bcLogError("eglGetConfigAttrib() returned error %d", eglGetError());
        goto window_create_error;
    }

#ifdef __ANDROID__
    ANativeWindow_setBuffersGeometry(inconfig->surface, 0, 0, format);
#endif

    if (!(surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType) inconfig->surface, 0))) {
        bcLogError("eglCreateWindowSurface() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!(context = eglCreateContext(display, config, 0, context_attribs))) {
        bcLogError("eglCreateContext() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        bcLogError("eglMakeCurrent() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
            !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        bcLogError("eglQuerySurface() returned error %d", eglGetError());
        goto window_create_error;
    }

    BCEGLWindow *nativeWindow = NEW_OBJECT(BCEGLWindow);
    nativeWindow->_display = display;
    nativeWindow->_surface = surface;
    nativeWindow->_context = context;

    inconfig->width = width;
    inconfig->height = height;

    BCWindow *window = NEW_OBJECT(BCWindow);
    window->width = inconfig->width;
    window->height = inconfig->height;
    window->nativeWindow = nativeWindow;

    bcInitGfx();

    // Enable v-sync
    eglSwapInterval(nativeWindow->_display, inconfig->vsync ? 1 : 0);

    initClock();

    return window;

window_create_error:
    bcLogError("Failed creating window!");
    return NULL;
}

void bcDestroyWindow(BCWindow *window)
{
    BCEGLWindow *nativeWindow = (BCEGLWindow *) window->nativeWindow;

    eglMakeCurrent(nativeWindow->_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(nativeWindow->_display, nativeWindow->_context);
    eglDestroySurface(nativeWindow->_display, nativeWindow->_surface);
    eglTerminate(nativeWindow->_display);

    free(window->nativeWindow);
    free(window);
}

void bcUpdateWindow(BCWindow *window)
{
    BCEGLWindow *nativeWindow = (BCEGLWindow *) window->nativeWindow;
    if (!eglSwapBuffers(nativeWindow->_display, nativeWindow->_surface))
    {
        bcLogError("eglSwapBuffers() returned error %d", eglGetError());
    }
    bcResetStates();
}
