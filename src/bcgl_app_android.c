#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include <pthread.h>

#include "bcgl_internal.h"

typedef struct
{
    ANativeWindow * _window;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
} BCAndroidWindow;

static BCWindow * s_Window = NULL;
static long s_StartTimeSec;

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
        bcLog("eglGetDisplay() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglInitialize(display, 0, 0)) {
        bcLog("eglInitialize() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        bcLog("eglChooseConfig() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        bcLog("eglGetConfigAttrib() returned error %d", eglGetError());
        goto window_create_error;
    }

    ANativeWindow_setBuffersGeometry(inconfig->surface, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, inconfig->surface, 0))) {
        bcLog("eglCreateWindowSurface() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!(context = eglCreateContext(display, config, 0, context_attribs))) {
        bcLog("eglCreateContext() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        bcLog("eglMakeCurrent() returned error %d", eglGetError());
        goto window_create_error;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
            !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        bcLog("eglQuerySurface() returned error %d", eglGetError());
        goto window_create_error;
    }

    glViewport(0, 0, width, height);

    BCAndroidWindow *nativeWindow = NEW_OBJECT(BCAndroidWindow);
    // nativeWindow->_window = display;
    nativeWindow->_display = display;
    nativeWindow->_surface = surface;
    nativeWindow->_context = context;

    s_Window = NEW_OBJECT(BCWindow);
    s_Window->width = inconfig->width;
    s_Window->height = inconfig->height;
    s_Window->nativeWindow = nativeWindow;

    bcInitGfx();

    // Enable v-sync
    eglSwapInterval(nativeWindow->_display, inconfig->vsync ? 1 : 0);

    return s_Window;

window_create_error:
    bcLog("Failed creating window!");
    return NULL;
}

void bcDestroyWindow(BCWindow *window)
{
    BCAndroidWindow *nativeWindow = (BCAndroidWindow *) window->nativeWindow;

    eglMakeCurrent(nativeWindow->_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(nativeWindow->_display, nativeWindow->_context);
    eglDestroySurface(nativeWindow->_display, nativeWindow->_surface);
    eglTerminate(nativeWindow->_display);

    free(window->nativeWindow);
    free(window);
}

void bcUpdateWindow(BCWindow *window)
{
    BCAndroidWindow *nativeWindow = (BCAndroidWindow *) window->nativeWindow;
    if (!eglSwapBuffers(nativeWindow->_display, nativeWindow->_surface))
    {
        bcLog("eglSwapBuffers() returned error %d", eglGetError());
    }
    bcResetStates();
}

void bcCloseWindow(BCWindow *window)
{
    BCAndroidWindow *nativeWindow = (BCAndroidWindow *) window->nativeWindow;
}

bool bcIsWindowOpened(BCWindow *window)
{
    BCAndroidWindow *nativeWindow = (BCAndroidWindow *) window->nativeWindow;
    return true;
}

BCWindow * bcGetWindow()
{
    return s_Window;
}

// App
bool bcInit()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    s_StartTimeSec = tv.tv_sec;
    return true;
}

void bcTerm()
{
}

void bcQuit(int code)
{
}

float bcGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec - s_StartTimeSec) + (0.000001f * tv.tv_usec);
}

//
// BCGL Android API
//

#define EVENT_APP_START     1
#define EVENT_APP_RESUME    2
#define EVENT_APP_PAUSE     3
#define EVENT_APP_STOP      4
#define EVENT_TOUCH_DOWN    5
#define EVENT_TOUCH_MOVE    6
#define EVENT_TOUCH_UP      7
#define EVENT_KEY_DOWN      8
#define EVENT_KEY_UP        9

enum BCAMsg
{
    MSG_APP_START,
    MSG_APP_RESUME,
    MSG_APP_PAUSE,
    MSG_APP_STOP,
    MSG_RENDER_LOOP_EXIT,
    MSG_INVALID
};

static pthread_t _threadId;
static pthread_mutex_t _mutex;
static enum BCAMsg _msg = MSG_INVALID;

static enum BCAMsg getCurrentMessage()
{
    pthread_mutex_lock(&_mutex);
    enum BCAMsg msg = _msg;
    _msg = MSG_INVALID;
    pthread_mutex_unlock(&_mutex);
    return msg;
}

static void * rendererThread(void *arg)
{
    if (!bcInit())
    {
        return NULL;
    }

    bool isRunning = true;

    BCConfig *config = (BCConfig *) arg;
    BC_onConfig(config);

    BCWindow *window = bcCreateWindow(config);
    if (window == NULL)
    {
        bcTerm();
        return NULL;
    }

    BC_onStart();

    bcLog("Android : start main loop");

    // Main loop
    float lastTime = bcGetTime();
    while (bcIsWindowOpened(window))
    {
        // handle message
        enum BCAMsg msg = getCurrentMessage();
        if (msg != MSG_INVALID)
        {
            bcLog("new message: %d", _msg);
            switch (msg)
            {
            case MSG_APP_START:
                break;
            case MSG_APP_RESUME:
                break;
            case MSG_APP_PAUSE:
                break;
            case MSG_APP_STOP:
                break;
            case MSG_RENDER_LOOP_EXIT:
                isRunning = false;
                break;
            default:
                bcLog("Unhandled message: %d", msg);
            }
        }
        if (!isRunning)
            break;
        // events
        int n = bcPullEvents();
        for (int i = 0; i < n; i++)
        {
            BCEvent *e = bcGetEvent(i);
            BC_onEvent(e->type, e->x, e->y);
        }
        // update
        BC_onUpdate(bcGetTime() - lastTime);
        lastTime = bcGetTime();
        bcUpdateWindow(window);
    }

    bcLog("Android : stop main loop");

    BC_onStop();

    bcDestroyWindow(window);
    free(config);
    
    bcTerm();

    return NULL;
}

void bcAndroidAcquireSurface(int id, ANativeWindow *surface, int format, int width, int height)
{
    bcLog("id=%d surface=%p format=%d width=%d height=%d", id, surface, format, width, height);

    BCConfig *config = NEW_OBJECT(BCConfig);
    config->width = width;
    config->height = height;
    config->format = format;
    config->surface = surface;
    config->vsync = true;

    pthread_mutex_init(&_mutex, 0);
    pthread_create(&_threadId, 0, rendererThread, config);
}

void bcAndroidReleaseSurface(int id)
{
    bcLog("id=%d", id);

    pthread_mutex_lock(&_mutex);
    _msg = MSG_RENDER_LOOP_EXIT;
    pthread_mutex_unlock(&_mutex);

    pthread_join(_threadId, 0);
}

void bcAndroidAppChengeState(int state)
{
    bcLog("state=%d", state);

    pthread_mutex_lock(&_mutex);
    _msg = state;
    pthread_mutex_unlock(&_mutex);
}

void bcAndroidTouchEvent(int event, int id, float x, float y)
{
    bcLog("event=%d id=%d x=%f y=%f", event, id, x, y);
    switch (event)
    {
    case EVENT_TOUCH_DOWN:
        bcSetMousePosition(x, y);
        bcSendEvent(BC_EVENT_MOUSEPRESS, id, 0);
        break;
    case EVENT_TOUCH_UP:
        bcSendEvent(BC_EVENT_MOUSERELEASE, id, 0);
        break;
    case EVENT_TOUCH_MOVE:
        bcSendEvent(BC_EVENT_MOUSEMOVE, x, y);
        break;
    }
}

void bcAndroidKeyEvent(int event, int key, int code)
{
    bcLog("event=%d key=%d code=%d", event, key, code);
}
