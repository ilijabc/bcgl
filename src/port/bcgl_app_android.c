#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>

#include <android/asset_manager.h>
#include <android/keycodes.h>
#include <android/native_window.h>

#include "../bcgl_internal.h"

#define EVENT_APP_CREATE    1
#define EVENT_APP_DESTROY   2
#define EVENT_APP_START     3
#define EVENT_APP_STOP      4
#define EVENT_APP_RESUME    5
#define EVENT_APP_PAUSE     6

#define EVENT_TOUCH_DOWN    1
#define EVENT_TOUCH_MOVE    2
#define EVENT_TOUCH_UP      3

#define EVENT_KEY_DOWN      1
#define EVENT_KEY_UP        2

#define EVENT_TEXT_INPUT    1
#define EVENT_TEXT_CANCEL   2

#define MSG_FINISH_ACTIVITY     1
#define MSG_SHOW_KEYBOARD       2
#define MSG_SET_ORIENTATION     3
#define MSG_INPUT_TEXT_DIALOG   4
#define MSG_SET_WINDOW_TYPE     5

#define GET_NUMBER_DENSITY      1

#define GET_INTEGER_KEYBOARD    1

static struct
{
    int android_code;
    int app_code;
} s_KeyMap[] = {
    { AKEYCODE_UNKNOWN, BC_KEY_UNKNOWN },
    { AKEYCODE_SOFT_LEFT, BC_KEY_UNKNOWN },
    { AKEYCODE_SOFT_RIGHT, BC_KEY_UNKNOWN },
    { AKEYCODE_HOME, BC_KEY_UNKNOWN },
    { AKEYCODE_BACK, BK_KEY_BACK },
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

static pthread_t s_ThreadId;
static bool s_ThreadRunning;
static void (*s_MsgCallback)(int type, int x, int y, const char *text) = NULL;
static float (*s_NumCallback)(int key) = NULL;
static int (*s_IntCallback)(int key) = NULL;

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

static void bcAndroidSendMessage(int type, int x, int y, const char *text)
{
    if (s_MsgCallback)
        s_MsgCallback(type, x, y, text);
}

static float bcAndroidGetNumber(int key)
{
    if (s_NumCallback)
        return s_NumCallback(key);
    return 0.0f;
}

static int bcAndroidGetInteger(int key)
{
    if (s_IntCallback)
        return s_IntCallback(key);
    return 0;
}

// Window

void bcCloseWindow(BCWindow *window)
{
    bcAndroidSendMessage(MSG_FINISH_ACTIVITY, 0, 0, NULL);
}

bool bcIsWindowOpened(BCWindow *window)
{
    return s_ThreadRunning;
}

void bcPullWindowEvents(BCWindow *window)
{
}

// App

void bcShowKeyboard(bool show)
{
    bcAndroidSendMessage(MSG_SHOW_KEYBOARD, show, 0, NULL);
}

float bcGetDisplayDensity()
{
    return bcAndroidGetNumber(GET_NUMBER_DENSITY);
}

int bcGetCommandLineArgs()
{
    return 0;
}

const char * bcGetCommandLineArg(int index)
{
    return NULL;
}

void bcInputTextDialog(const char *text)
{
    bcAndroidSendMessage(MSG_INPUT_TEXT_DIALOG, 0, 0, text);
}

bool bcIsKeyboardConnected()
{
    return bcAndroidGetInteger(GET_INTEGER_KEYBOARD) == 2; // KEYBOARD_QWERTY
}

//
// BCGL Android API
//

static void * rendererThread(void *arg)
{
    BCConfig *config = (BCConfig *)arg;

    bcAppConfig(config);

    if (!bcAppStart(config))
    {
        return NULL;
    }

    bcAndroidSendMessage(MSG_SET_WINDOW_TYPE, config->mode, 0, NULL);

    bcAppMainLoop();

    bcAppStop();

    return NULL;
}

void bcAndroidInitFileSystem(AAssetManager *manager, const char *local_path, const char *external_path)
{
    bcInitFiles(manager);
    bcSetPathForType(BC_PATH_TYPE_LOCAL, local_path);
    bcSetPathForType(BC_PATH_TYPE_EXTERNAL, external_path);
}

void bcAndroidSurfaceCreated(ANativeWindow *window)
{
    BCConfig *config = NEW_OBJECT(BCConfig);
    config->width = 0;
    config->height = 0;
    config->format = 0;
    config->vsync = true;
    config->surface = window;

    s_ThreadRunning = true;
    pthread_create(&s_ThreadId, 0, rendererThread, config);
}

void bcAndroidSurfaceDestroyed()
{
    s_ThreadRunning = false;
    pthread_join(s_ThreadId, 0);
}

void bcAndroidSurfaceChanged(int format, int width, int height)
{
    bcSendEvent(BC_EVENT_WINDOW_SIZE, format, width, height, NULL);
}

void bcAndroidAppChengeState(int state)
{
    switch (state)
    {
    case EVENT_APP_CREATE:
        bcAppCreate();
        break;
    case EVENT_APP_DESTROY:
        bcAppDestroy();
        break;
    case EVENT_APP_PAUSE:
        bcSendEvent(BC_EVENT_WINDOW_FOCUS, 0, 0, 0, NULL);
        break;
    case EVENT_APP_RESUME:
        bcSendEvent(BC_EVENT_WINDOW_FOCUS, 1, 0, 0, NULL);
        break;
    }
}

void bcAndroidTouchEvent(int event, int id, float x, float y)
{
    switch (event)
    {
    case EVENT_TOUCH_DOWN:
        bcSendEvent(BC_EVENT_TOUCH_DOWN, id, x, y, NULL);
        break;
    case EVENT_TOUCH_UP:
        bcSendEvent(BC_EVENT_TOUCH_UP, id, x, y, NULL);
        break;
    case EVENT_TOUCH_MOVE:
        bcSendEvent(BC_EVENT_TOUCH_MOVE, id, x, y, NULL);
        break;
    default:
        bcLogWarning("Unhandled event: %d", event);
    }
}

void bcAndroidKeyEvent(int event, int key, int code, int deviceId)
{
    int appCode = convertAndroidKeyCode(key);
    switch (event)
    {
    case EVENT_KEY_DOWN:
        bcSendEvent(BC_EVENT_KEY_PRESS, appCode, deviceId, 0, NULL);
        break;
    case EVENT_KEY_UP:
        bcSendEvent(BC_EVENT_KEY_RELEASE, appCode, deviceId, 0, NULL);
        break;
    default:
        bcLogWarning("Unhandled event: %d", event);
    }
}

void bcAndroidTextEvent(int event, const char *text)
{
    static char data[1000];
    if (event == EVENT_TEXT_INPUT)
    {
        int n = strlen(text);
        strncpy(data, text, 1000);
        data[n > 1000 ? 1000 : n] = '\0';
        bcSendEvent(BC_EVENT_TEXT_INPUT, event, 0, 0, (void *) data);
    }
    else if (event == EVENT_TEXT_CANCEL)
    {
        bcSendEvent(BC_EVENT_TEXT_CANCEL, event, 0, 0, NULL);
    }
}

void bcAndroidSetCallbacks(void (*msg_callback)(int type, int x, int y, const char *text), float (*num_callback)(int key), int (*int_callback)(int key))
{
    s_MsgCallback = msg_callback;
    s_NumCallback = num_callback;
    s_IntCallback = int_callback;
}

//
// JNI
//

#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>

// Java virtual machine
static JavaVM *g_JVM = 0;
static JNIEnv *g_Env = 0;
static jclass g_Class = 0;
static jmethodID g_Method_onNativeMessage = 0;
static jmethodID g_Method_onNativeGetNumber = 0;
static jmethodID g_Method_onNativeGetInteger = 0;
static ANativeWindow *g_NativeWindow = NULL;

void BC_MsgCallback(int type, int x, int y, const char *text)
{
    if (g_Env && g_Class && g_Method_onNativeMessage) {
        if ((*g_JVM)->AttachCurrentThread(g_JVM, &g_Env, 0) == JNI_OK) {
            jstring j_text = (*g_Env)->NewStringUTF(g_Env, text);
            (*g_Env)->CallStaticVoidMethod(g_Env, g_Class, g_Method_onNativeMessage, type, x, y, j_text);
        }
    }
}

float BC_NumCallback(int key)
{
    if (g_Env && g_Class && g_Method_onNativeGetNumber) {
        if ((*g_JVM)->AttachCurrentThread(g_JVM, &g_Env, 0) == JNI_OK) {
            return (*g_Env)->CallStaticFloatMethod(g_Env, g_Class, g_Method_onNativeGetNumber, key);
        }
    }
    return 0.0f;
}

int BC_IntCallback(int key)
{
    if (g_Env && g_Class && g_Method_onNativeGetInteger) {
        if ((*g_JVM)->AttachCurrentThread(g_JVM, &g_Env, 0) == JNI_OK) {
            return (*g_Env)->CallStaticIntMethod(g_Env, g_Class, g_Method_onNativeGetInteger, key);
        }
    }
    return 0;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    g_JVM = vm;
    (*g_JVM)->GetEnv(g_JVM, (void **)&g_Env, JNI_VERSION_1_6);

    jclass localClass = (*g_Env)->FindClass(g_Env, "info/djukic/bcgl/BCGLLib");
    if (localClass) {
        g_Class = (*g_Env)->NewGlobalRef(g_Env, localClass);
        g_Method_onNativeMessage = (*g_Env)->GetStaticMethodID(g_Env, g_Class, "onNativeMessage", "(IIILjava/lang/String;)V");
        g_Method_onNativeGetNumber = (*g_Env)->GetStaticMethodID(g_Env, g_Class, "onNativeGetNumber", "(I)F");
        g_Method_onNativeGetInteger = (*g_Env)->GetStaticMethodID(g_Env, g_Class, "onNativeGetInteger", "(I)I");
    }

    bcAndroidSetCallbacks(BC_MsgCallback, BC_NumCallback, BC_IntCallback);

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeInitFileSystem(JNIEnv *env, jclass type, jobject manager, jstring local_path, jstring external_path)
{
    AAssetManager *aam = AAssetManager_fromJava(env, manager);
    const char *c_local_path = (*env)->GetStringUTFChars(env, local_path, NULL);
    const char *c_external_path = (*env)->GetStringUTFChars(env, external_path, NULL);
    bcAndroidInitFileSystem(aam, c_local_path, c_external_path);
    (*env)->ReleaseStringUTFChars(env, local_path, c_local_path);
    (*env)->ReleaseStringUTFChars(env, external_path, c_external_path);
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeSurfaceCreated(JNIEnv *env, jclass type, jint id, jobject surface)
{
    g_NativeWindow = ANativeWindow_fromSurface(env, surface);
    bcAndroidSurfaceCreated(g_NativeWindow);
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeSurfaceChanged(JNIEnv *env, jclass type, jint id, jobject surface, jint format, jint width, jint height)
{
    bcAndroidSurfaceChanged(format, width, height);
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeSurfaceDestroyed(JNIEnv *env, jclass type, jint id, jobject surface)
{
    bcAndroidSurfaceDestroyed();
    ANativeWindow_release(g_NativeWindow);
    g_NativeWindow = NULL;
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeAppChangeState(JNIEnv *env, jclass type, jint state)
{
    bcAndroidAppChengeState(state);
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeTouchEvent(JNIEnv *env, jclass type, jint event, jint id, jfloat x, jfloat y)
{
    bcAndroidTouchEvent(event, id, x, y);
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeKeyEvent(JNIEnv *env, jclass type, jint event, jint key, jint code, jint deviceId)
{
    bcAndroidKeyEvent(event, key, code, deviceId);
}

JNIEXPORT void JNICALL
Java_info_djukic_bcgl_BCGLLib_nativeTextEvent(JNIEnv *env, jclass type, jint event, jstring text)
{
    const char *c_text = text ? (*env)->GetStringUTFChars(env, text, NULL) : NULL;
    bcAndroidTextEvent(event, c_text);
    if (c_text)
        (*env)->ReleaseStringUTFChars(env, text, c_text);
}
