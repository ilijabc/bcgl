#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <sys/time.h>
#include <pthread.h>
#include <emscripten/emscripten.h>

#include "bcgl_internal.h"

typedef struct
{
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
} BCGLESWindow;

static long s_StartTimeSec = -1;


void bcCloseWindow(BCWindow *window)
{
}

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

    // ANativeWindow_setBuffersGeometry(s_Surface, 0, 0, format);
    EGLNativeWindowType s_Surface = 0;

    if (!(surface = eglCreateWindowSurface(display, config, s_Surface, 0))) {
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

    glViewport(0, 0, width, height);

    BCGLESWindow *nativeWindow = NEW_OBJECT(BCGLESWindow);
    // nativeWindow->_window = display;
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

    return window;

window_create_error:
    bcLogError("Failed creating window!");
    return NULL;
}

void bcDestroyWindow(BCWindow *window)
{
    BCGLESWindow *nativeWindow = (BCGLESWindow *) window->nativeWindow;

    eglMakeCurrent(nativeWindow->_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(nativeWindow->_display, nativeWindow->_context);
    eglDestroySurface(nativeWindow->_display, nativeWindow->_surface);
    eglTerminate(nativeWindow->_display);

    free(window->nativeWindow);
    free(window);
}

float bcGetDisplayDensity()
{
    return 1.0f;
}

float bcGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (s_StartTimeSec == -1)
    {
        s_StartTimeSec = tv.tv_sec;
    }
    return (tv.tv_sec - s_StartTimeSec) + (0.000001f * tv.tv_usec);
}

bool bcIsWindowOpened(BCWindow *window)
{
    return true;
}

void bcPullWindowEvents(BCWindow *window)
{
    // usleep(100);
}

void bcUpdateWindow(BCWindow *window)
{
    BCGLESWindow *nativeWindow = (BCGLESWindow *) window->nativeWindow;
    if (!eglSwapBuffers(nativeWindow->_display, nativeWindow->_surface))
    {
        bcLogError("eglSwapBuffers() returned error %d", eglGetError());
    }
    bcResetStates();
}
BCWindow *window = NULL;

// static void * rendererThread(void *arg)
static void rendererThread()
{
    static float lastTime = 0;
    float now = bcGetTime();
    float dt = now - lastTime;
    lastTime = now;

    BCCallbacks callbacks = bcGetCallbacks();

    // events
    // bcPullWindowEvents(window);
    // int n = bcPullEvents();
    // for (int i = 0; i < n; i++)
    // {
    //     BCEvent *e = bcGetEvent(i);
    //     if (callbacks.onEvent)
    //         callbacks.onEvent(*e);
    // }
    // update
    if (callbacks.onUpdate)
        callbacks.onUpdate(dt);
    bcUpdateWindow(window);
}

void _bcAppMain(BCConfig *config)
{
    BCCallbacks callbacks = bcGetCallbacks();

    if (callbacks.onConfig)
        callbacks.onConfig(config);
    else
        bcLogWarning("Missing onConfig callback!");

    if (callbacks.onCreate)
        callbacks.onCreate();

    window = bcCreateWindow(config);
    if (window == NULL)
    {
        bcLogError("Unable to create window!");
        return;
    }
    bcSetWindow(window);

    if (callbacks.onStart)
        callbacks.onStart();

    emscripten_set_main_loop(rendererThread, 0, 1);

    // if (callbacks.onStop)
    //     callbacks.onStop();

    // bcDestroyWindow(window);

    // if (callbacks.onDestroy)
    //     callbacks.onDestroy();
}

int bcDesktopMain(int argc, char **argv)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    s_StartTimeSec = tv.tv_sec;

    bcInitFiles(NULL);

    // Default config
    BCConfig config;
    config.title = NULL;
    config.width = 800;
    config.height = 600;
    config.format = 0;
    config.mode = 0;
    config.vsync = true;
    config.msaa = 0;
    config.orientation = 0;

    _bcAppMain(&config);

    bcTermFiles();

    return 0;
}
