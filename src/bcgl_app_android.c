#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>

#include <android/keycodes.h>
#include <android/native_window.h>

#include "bcgl_internal.h"
#include <bcgl_android.h>

typedef struct
{
    ANativeWindow * _window;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
} BCAndroidWindow;

#define BC_KEY_UNKNOWN -1

static struct
{
    int android_code;
    int app_code;
} s_KeyMap[] = {
    { AKEYCODE_UNKNOWN, BC_KEY_UNKNOWN },
    { AKEYCODE_SOFT_LEFT, BC_KEY_UNKNOWN },
    { AKEYCODE_SOFT_RIGHT, BC_KEY_UNKNOWN },
    { AKEYCODE_HOME, BC_KEY_UNKNOWN },
    { AKEYCODE_BACK, BC_KEY_ESCAPE },
    { AKEYCODE_CALL, BC_KEY_UNKNOWN },
    { AKEYCODE_ENDCALL, BC_KEY_UNKNOWN },
    { AKEYCODE_0, BC_KEY_0 },
    { AKEYCODE_1, BC_KEY_1 },
    { AKEYCODE_2, BC_KEY_2 },
    { AKEYCODE_3, BC_KEY_3 },
    { AKEYCODE_4, BC_KEY_4 },
    { AKEYCODE_5, BC_KEY_5 },
    { AKEYCODE_6, BC_KEY_6 },
    { AKEYCODE_7, BC_KEY_7 },
    { AKEYCODE_8, BC_KEY_8 },
    { AKEYCODE_9, BC_KEY_9 },
    { AKEYCODE_STAR, BC_KEY_UNKNOWN },
    { AKEYCODE_POUND, BC_KEY_UNKNOWN },
    { AKEYCODE_DPAD_UP, BC_KEY_UP },
    { AKEYCODE_DPAD_DOWN, BC_KEY_DOWN },
    { AKEYCODE_DPAD_LEFT, BC_KEY_LEFT },
    { AKEYCODE_DPAD_RIGHT, BC_KEY_RIGHT },
    { AKEYCODE_DPAD_CENTER, BC_KEY_ENTER },
    { AKEYCODE_VOLUME_UP, BC_KEY_UNKNOWN },
    { AKEYCODE_VOLUME_DOWN, BC_KEY_UNKNOWN },
    { AKEYCODE_POWER, BC_KEY_UNKNOWN },
    { AKEYCODE_CAMERA, BC_KEY_UNKNOWN },
    { AKEYCODE_CLEAR, BC_KEY_BACKSPACE },
    { AKEYCODE_A, BC_KEY_A },
    { AKEYCODE_B, BC_KEY_B },
    { AKEYCODE_C, BC_KEY_C },
    { AKEYCODE_D, BC_KEY_D },
    { AKEYCODE_E, BC_KEY_E },
    { AKEYCODE_F, BC_KEY_F },
    { AKEYCODE_G, BC_KEY_G },
    { AKEYCODE_H, BC_KEY_H },
    { AKEYCODE_I, BC_KEY_I },
    { AKEYCODE_J, BC_KEY_J },
    { AKEYCODE_K, BC_KEY_K },
    { AKEYCODE_L, BC_KEY_L },
    { AKEYCODE_M, BC_KEY_M },
    { AKEYCODE_N, BC_KEY_N },
    { AKEYCODE_O, BC_KEY_O },
    { AKEYCODE_P, BC_KEY_P },
    { AKEYCODE_Q, BC_KEY_Q },
    { AKEYCODE_R, BC_KEY_R },
    { AKEYCODE_S, BC_KEY_S },
    { AKEYCODE_T, BC_KEY_T },
    { AKEYCODE_U, BC_KEY_U },
    { AKEYCODE_V, BC_KEY_V },
    { AKEYCODE_W, BC_KEY_W },
    { AKEYCODE_X, BC_KEY_X },
    { AKEYCODE_Y, BC_KEY_Y },
    { AKEYCODE_Z, BC_KEY_Z },
    { AKEYCODE_COMMA, BC_KEY_COMMA },
    { AKEYCODE_PERIOD, BC_KEY_PERIOD },
    { AKEYCODE_ALT_LEFT, BC_KEY_LEFT_ALT },
    { AKEYCODE_ALT_RIGHT, BC_KEY_RIGHT_ALT },
    { AKEYCODE_SHIFT_LEFT, BC_KEY_LEFT_SHIFT },
    { AKEYCODE_SHIFT_RIGHT, BC_KEY_RIGHT_SHIFT },
    { AKEYCODE_TAB, BC_KEY_TAB },
    { AKEYCODE_SPACE, BC_KEY_SPACE },
    { AKEYCODE_SYM, BC_KEY_UNKNOWN },
    { AKEYCODE_EXPLORER, BC_KEY_UNKNOWN },
    { AKEYCODE_ENVELOPE, BC_KEY_UNKNOWN },
    { AKEYCODE_ENTER, BC_KEY_ENTER },
    { AKEYCODE_DEL, BC_KEY_DELETE },
    { AKEYCODE_GRAVE, BC_KEY_GRAVE_ACCENT },
    { AKEYCODE_MINUS, BC_KEY_MINUS },
    { AKEYCODE_EQUALS, BC_KEY_EQUAL },
    { AKEYCODE_LEFT_BRACKET, BC_KEY_LEFT_BRACKET },
    { AKEYCODE_RIGHT_BRACKET, BC_KEY_RIGHT_BRACKET },
    { AKEYCODE_BACKSLASH, BC_KEY_BACKSLASH },
    { AKEYCODE_SEMICOLON, BC_KEY_SEMICOLON },
    { AKEYCODE_APOSTROPHE, BC_KEY_APOSTROPHE },
    { AKEYCODE_SLASH, BC_KEY_SLASH },
    { AKEYCODE_AT, BC_KEY_UNKNOWN },
    { AKEYCODE_NUM, BC_KEY_UNKNOWN },
    { AKEYCODE_HEADSETHOOK, BC_KEY_UNKNOWN },
    { AKEYCODE_FOCUS, BC_KEY_UNKNOWN },
    { AKEYCODE_PLUS, BC_KEY_UNKNOWN },
    { AKEYCODE_MENU, BC_KEY_MENU },
    { AKEYCODE_NOTIFICATION, BC_KEY_UNKNOWN },
    { AKEYCODE_SEARCH, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_STOP, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_NEXT, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_PREVIOUS, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_REWIND, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_FAST_FORWARD, BC_KEY_UNKNOWN },
    { AKEYCODE_MUTE, BC_KEY_UNKNOWN },
    { AKEYCODE_PAGE_UP, BC_KEY_PAGE_UP },
    { AKEYCODE_PAGE_DOWN, BC_KEY_PAGE_DOWN },
    { AKEYCODE_PICTSYMBOLS, BC_KEY_UNKNOWN },
    { AKEYCODE_SWITCH_CHARSET, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_A, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_B, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_C, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_X, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_Y, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_Z, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_L1, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_R1, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_L2, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_R2, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_THUMBL, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_THUMBR, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_START, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_SELECT, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_MODE, BC_KEY_UNKNOWN },
    { AKEYCODE_ESCAPE, BC_KEY_ESCAPE },
    { AKEYCODE_FORWARD_DEL, BC_KEY_BACKSPACE },
    { AKEYCODE_CTRL_LEFT, BC_KEY_LEFT_CONTROL },
    { AKEYCODE_CTRL_RIGHT, BC_KEY_RIGHT_CONTROL },
    { AKEYCODE_CAPS_LOCK, BC_KEY_CAPS_LOCK },
    { AKEYCODE_SCROLL_LOCK, BC_KEY_SCROLL_LOCK },
    { AKEYCODE_META_LEFT, BC_KEY_UNKNOWN },
    { AKEYCODE_META_RIGHT, BC_KEY_UNKNOWN },
    { AKEYCODE_FUNCTION, BC_KEY_UNKNOWN },
    { AKEYCODE_SYSRQ, BC_KEY_UNKNOWN },
    { AKEYCODE_BREAK, BC_KEY_PAUSE },
    { AKEYCODE_MOVE_HOME, BC_KEY_HOME },
    { AKEYCODE_MOVE_END, BC_KEY_END },
    { AKEYCODE_INSERT, BC_KEY_INSERT },
    { AKEYCODE_FORWARD, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_PLAY, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_PAUSE, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_CLOSE, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_EJECT, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_RECORD, BC_KEY_UNKNOWN },
    { AKEYCODE_F1, BC_KEY_F1 },
    { AKEYCODE_F2, BC_KEY_F2 },
    { AKEYCODE_F3, BC_KEY_F3 },
    { AKEYCODE_F4, BC_KEY_F4 },
    { AKEYCODE_F5, BC_KEY_F5 },
    { AKEYCODE_F6, BC_KEY_F6 },
    { AKEYCODE_F7, BC_KEY_F7 },
    { AKEYCODE_F8, BC_KEY_F8 },
    { AKEYCODE_F9, BC_KEY_F9 },
    { AKEYCODE_F10, BC_KEY_F10 },
    { AKEYCODE_F11, BC_KEY_F11 },
    { AKEYCODE_F12, BC_KEY_F12 },
    { AKEYCODE_NUM_LOCK, BC_KEY_NUM_LOCK },
    { AKEYCODE_NUMPAD_0, BC_KEY_KP_0 },
    { AKEYCODE_NUMPAD_1, BC_KEY_KP_1 },
    { AKEYCODE_NUMPAD_2, BC_KEY_KP_2 },
    { AKEYCODE_NUMPAD_3, BC_KEY_KP_3 },
    { AKEYCODE_NUMPAD_4, BC_KEY_KP_4 },
    { AKEYCODE_NUMPAD_5, BC_KEY_KP_5 },
    { AKEYCODE_NUMPAD_6, BC_KEY_KP_6 },
    { AKEYCODE_NUMPAD_7, BC_KEY_KP_7 },
    { AKEYCODE_NUMPAD_8, BC_KEY_KP_8 },
    { AKEYCODE_NUMPAD_9, BC_KEY_KP_9 },
    { AKEYCODE_NUMPAD_DIVIDE, BC_KEY_KP_DIVIDE },
    { AKEYCODE_NUMPAD_MULTIPLY, BC_KEY_KP_MULTIPLY },
    { AKEYCODE_NUMPAD_SUBTRACT, BC_KEY_KP_SUBTRACT },
    { AKEYCODE_NUMPAD_ADD, BC_KEY_KP_ADD },
    { AKEYCODE_NUMPAD_DOT, BC_KEY_KP_DECIMAL },
    { AKEYCODE_NUMPAD_COMMA, BC_KEY_UNKNOWN },
    { AKEYCODE_NUMPAD_ENTER, BC_KEY_KP_ENTER },
    { AKEYCODE_NUMPAD_EQUALS, BC_KEY_KP_EQUAL },
    { AKEYCODE_NUMPAD_LEFT_PAREN, BC_KEY_UNKNOWN },
    { AKEYCODE_NUMPAD_RIGHT_PAREN, BC_KEY_UNKNOWN },
    { AKEYCODE_VOLUME_MUTE, BC_KEY_UNKNOWN },
    { AKEYCODE_INFO, BC_KEY_UNKNOWN },
    { AKEYCODE_CHANNEL_UP, BC_KEY_UNKNOWN },
    { AKEYCODE_CHANNEL_DOWN, BC_KEY_UNKNOWN },
    { AKEYCODE_ZOOM_IN, BC_KEY_UNKNOWN },
    { AKEYCODE_ZOOM_OUT, BC_KEY_UNKNOWN },
    { AKEYCODE_TV, BC_KEY_UNKNOWN },
    { AKEYCODE_WINDOW, BC_KEY_UNKNOWN },
    { AKEYCODE_GUIDE, BC_KEY_UNKNOWN },
    { AKEYCODE_DVR, BC_KEY_UNKNOWN },
    { AKEYCODE_BOOKMARK, BC_KEY_UNKNOWN },
    { AKEYCODE_CAPTIONS, BC_KEY_UNKNOWN },
    { AKEYCODE_SETTINGS, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_POWER, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT, BC_KEY_UNKNOWN },
    { AKEYCODE_STB_POWER, BC_KEY_UNKNOWN },
    { AKEYCODE_STB_INPUT, BC_KEY_UNKNOWN },
    { AKEYCODE_AVR_POWER, BC_KEY_UNKNOWN },
    { AKEYCODE_AVR_INPUT, BC_KEY_UNKNOWN },
    { AKEYCODE_PROG_RED, BC_KEY_UNKNOWN },
    { AKEYCODE_PROG_GREEN, BC_KEY_UNKNOWN },
    { AKEYCODE_PROG_YELLOW, BC_KEY_UNKNOWN },
    { AKEYCODE_PROG_BLUE, BC_KEY_UNKNOWN },
    { AKEYCODE_APP_SWITCH, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_1, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_2, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_3, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_4, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_5, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_6, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_7, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_8, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_9, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_10, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_11, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_12, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_13, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_14, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_15, BC_KEY_UNKNOWN },
    { AKEYCODE_BUTTON_16, BC_KEY_UNKNOWN },
    { AKEYCODE_LANGUAGE_SWITCH, BC_KEY_UNKNOWN },
    { AKEYCODE_MANNER_MODE, BC_KEY_UNKNOWN },
    { AKEYCODE_3D_MODE, BC_KEY_UNKNOWN },
    { AKEYCODE_CONTACTS, BC_KEY_UNKNOWN },
    { AKEYCODE_CALENDAR, BC_KEY_UNKNOWN },
    { AKEYCODE_MUSIC, BC_KEY_UNKNOWN },
    { AKEYCODE_CALCULATOR, BC_KEY_UNKNOWN },
    { AKEYCODE_ZENKAKU_HANKAKU, BC_KEY_UNKNOWN },
    { AKEYCODE_EISU, BC_KEY_UNKNOWN },
    { AKEYCODE_MUHENKAN, BC_KEY_UNKNOWN },
    { AKEYCODE_HENKAN, BC_KEY_UNKNOWN },
    { AKEYCODE_KATAKANA_HIRAGANA, BC_KEY_UNKNOWN },
    { AKEYCODE_YEN, BC_KEY_UNKNOWN },
    { AKEYCODE_RO, BC_KEY_UNKNOWN },
    { AKEYCODE_KANA, BC_KEY_UNKNOWN },
    { AKEYCODE_ASSIST, BC_KEY_UNKNOWN },
    { AKEYCODE_BRIGHTNESS_DOWN, BC_KEY_UNKNOWN },
    { AKEYCODE_BRIGHTNESS_UP, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_AUDIO_TRACK, BC_KEY_UNKNOWN },
    { AKEYCODE_SLEEP, BC_KEY_UNKNOWN },
    { AKEYCODE_WAKEUP, BC_KEY_UNKNOWN },
    { AKEYCODE_PAIRING, BC_KEY_UNKNOWN },
    { AKEYCODE_MEDIA_TOP_MENU, BC_KEY_UNKNOWN },
    { AKEYCODE_11, BC_KEY_UNKNOWN },
    { AKEYCODE_12, BC_KEY_UNKNOWN },
    { AKEYCODE_LAST_CHANNEL, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_DATA_SERVICE, BC_KEY_UNKNOWN },
    { AKEYCODE_VOICE_ASSIST, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_RADIO_SERVICE, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_TELETEXT, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_NUMBER_ENTRY, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_TERRESTRIAL_ANALOG, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_TERRESTRIAL_DIGITAL, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_SATELLITE, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_SATELLITE_BS, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_SATELLITE_CS, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_SATELLITE_SERVICE, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_NETWORK, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_ANTENNA_CABLE, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_HDMI_1, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_HDMI_2, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_HDMI_3, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_HDMI_4, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_COMPOSITE_1, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_COMPOSITE_2, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_COMPONENT_1, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_COMPONENT_2, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_INPUT_VGA_1, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_AUDIO_DESCRIPTION, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_UP, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_DOWN, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_ZOOM_MODE, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_CONTENTS_MENU, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_MEDIA_CONTEXT_MENU, BC_KEY_UNKNOWN },
    { AKEYCODE_TV_TIMER_PROGRAMMING, BC_KEY_UNKNOWN },
    { AKEYCODE_HELP, BC_KEY_UNKNOWN },
    { -1, -1 }
};

static BCWindow * s_Window = NULL;
static long s_StartTimeSec;

static pthread_t s_ThreadId;
static pthread_mutex_t s_Mutex;
static bool s_ThreadRunning;
static void (*s_JniCallback)(int type, int x, int y) = NULL;

static int convertAndroidKeyCode(int keyCode)
{
    for (int i = 0; s_KeyMap[i].android_code != -1; i++)
    {
        if (s_KeyMap[i].android_code == keyCode)
        {
            return s_KeyMap[i].app_code;
        }
    }
    return BC_KEY_UNKNOWN;
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

    ANativeWindow_setBuffersGeometry(inconfig->surface, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, inconfig->surface, 0))) {
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
    bcLogError("Failed creating window!");
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
        bcLogError("eglSwapBuffers() returned error %d", eglGetError());
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
bool bcInitApp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    s_StartTimeSec = tv.tv_sec;
    return true;
}

void bcTermApp()
{
    bcTermFiles();
}

void bcQuit(int code)
{
    bcAndroidSendMessage(MSG_FINISH_ACTIVITY, code, 0);
}

float bcGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec - s_StartTimeSec) + (0.000001f * tv.tv_usec);
}

void bcShowKeyboard(bool show)
{
    bcAndroidSendMessage(MSG_SHOW_KEYBOARD, show, 0);
}

//
// BCGL Android API
//

static void * rendererThread(void *arg)
{
    BCCallbacks callbacks = bcGetCallbacks();

    if (!bcInitApp())
    {
        return NULL;
    }

    bool isRunning = true;

    BCConfig *config = (BCConfig *) arg;
    if (callbacks.onConfig)
        callbacks.onConfig(config);
    else
        bcLogWarning("Missing onConfig callback!");

    BCWindow *window = bcCreateWindow(config);
    if (window == NULL)
    {
        bcTermApp();
        return NULL;
    }

    if (callbacks.onStart)
        callbacks.onStart();

    bcLog("Android : start main loop");

    // Main loop
    float lastTime = bcGetTime();
    while (s_ThreadRunning && bcIsWindowOpened(window))
    {
        // events
        pthread_mutex_lock(&s_Mutex);
        int n = bcPullEvents();
        for (int i = 0; i < n; i++)
        {
            BCEvent *e = bcGetEvent(i);
            if (callbacks.onEvent)
                callbacks.onEvent(e->type, e->x, e->y);
        }
        pthread_mutex_unlock(&s_Mutex);
        // update
        if (callbacks.onUpdate)
            callbacks.onUpdate(bcGetTime() - lastTime);
        lastTime = bcGetTime();
        bcUpdateWindow(window);
    }

    bcLog("Android : stop main loop");

    if (callbacks.onStop)
        callbacks.onStop();

    bcDestroyWindow(window);
    free(config);
    
    bcTermApp();

    return NULL;
}

void bcAndroidSetAssetManager(AAssetManager *manager)
{
    bcInitFiles(manager);
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

    s_ThreadRunning = true;

    pthread_mutex_init(&s_Mutex, 0);
    pthread_create(&s_ThreadId, 0, rendererThread, config);
}

void bcAndroidReleaseSurface(int id)
{
    bcLog("id=%d", id);
    s_ThreadRunning = false;
    pthread_join(s_ThreadId, 0);
}

void bcAndroidAppChengeState(int state)
{
    // bcLog("state=%d", state);
    pthread_mutex_lock(&s_Mutex);
    if (state == EVENT_APP_PAUSE)
        bcSendEvent(BC_EVENT_WINDOWFOCUS, 0, 0);
    else if (state == EVENT_APP_RESUME)
        bcSendEvent(BC_EVENT_WINDOWFOCUS, 1, 0);
    pthread_mutex_unlock(&s_Mutex);
}

void bcAndroidTouchEvent(int event, int id, float x, float y)
{
    // bcLog("event=%d id=%d x=%f y=%f", event, id, x, y);
    pthread_mutex_lock(&s_Mutex);
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
    default:
        bcLogWarning("Unhandled event: %d", event);
    }
    pthread_mutex_unlock(&s_Mutex);
}

void bcAndroidKeyEvent(int event, int key, int code)
{
    int appKey = convertAndroidKeyCode(key);
    // bcLog("event=%d key=%d code=%d appKey=%d", event, key, code, appKey);
    pthread_mutex_lock(&s_Mutex);
    switch (event)
    {
    case EVENT_KEY_DOWN:
        bcSendEvent(BC_EVENT_KEYPRESS, appKey, key);
        break;
    case EVENT_KEY_UP:
        bcSendEvent(BC_EVENT_KEYRELEASE, appKey, key);
        break;
    default:
        bcLogWarning("Unhandled event: %d", event);
    }
    pthread_mutex_unlock(&s_Mutex);
}


void bcAndroidSetCallback(void (*callback)(int type, int x, int y))
{
    s_JniCallback = callback;
}

void bcAndroidSendMessage(int type, int x, int y)
{
    // pthread_mutex_lock(&s_Mutex);
    if (s_JniCallback)
        s_JniCallback(type, x, y);
    // pthread_mutex_unlock(&s_Mutex);
}
