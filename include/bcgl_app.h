#pragma once

#include "bcbase.h"

// Event Types
enum BCEventType
{
    BC_EVENT_KEY_PRESS,
    BC_EVENT_KEY_RELEASE,
    BC_EVENT_KEY_REPEAT,
    BC_EVENT_KEY_CHAR,
    BC_EVENT_MOUSE_PRESS,
    BC_EVENT_MOUSE_RELEASE,
    BC_EVENT_MOUSE_MOVE,
    BC_EVENT_MOUSE_WHEEL,
    BC_EVENT_WINDOW_SIZE,
    BC_EVENT_WINDOW_FOCUS,
    BC_EVENT_WINDOW_ICONIFY,
    BC_EVENT_TOUCH_DOWN,
    BC_EVENT_TOUCH_UP,
    BC_EVENT_TOUCH_MOVE,
    BC_EVENT_TEXT_INPUT,
    BC_EVENT_TEXT_CANCEL,
};

// Key Codes
enum BCKeyCode
{
    BC_KEY_UNKNOWN = -1,
    BC_KEY_SPACE,
    BC_KEY_APOSTROPHE,
    BC_KEY_COMMA,
    BC_KEY_MINUS,
    BC_KEY_PERIOD,
    BC_KEY_SLASH,
    BC_KEY_0,
    BC_KEY_1,
    BC_KEY_2,
    BC_KEY_3,
    BC_KEY_4,
    BC_KEY_5,
    BC_KEY_6,
    BC_KEY_7,
    BC_KEY_8,
    BC_KEY_9,
    BC_KEY_SEMICOLON,
    BC_KEY_EQUAL,
    BC_KEY_A,
    BC_KEY_B,
    BC_KEY_C,
    BC_KEY_D,
    BC_KEY_E,
    BC_KEY_F,
    BC_KEY_G,
    BC_KEY_H,
    BC_KEY_I,
    BC_KEY_J,
    BC_KEY_K,
    BC_KEY_L,
    BC_KEY_M,
    BC_KEY_N,
    BC_KEY_O,
    BC_KEY_P,
    BC_KEY_Q,
    BC_KEY_R,
    BC_KEY_S,
    BC_KEY_T,
    BC_KEY_U,
    BC_KEY_V,
    BC_KEY_W,
    BC_KEY_X,
    BC_KEY_Y,
    BC_KEY_Z,
    BC_KEY_LEFT_BRACKET,
    BC_KEY_BACKSLASH,
    BC_KEY_RIGHT_BRACKET,
    BC_KEY_GRAVE_ACCENT,
    BC_KEY_WORLD_1,
    BC_KEY_WORLD_2,
    BC_KEY_ESCAPE,
    BC_KEY_ENTER,
    BC_KEY_TAB,
    BC_KEY_BACKSPACE,
    BC_KEY_INSERT,
    BC_KEY_DELETE,
    BC_KEY_RIGHT,
    BC_KEY_LEFT,
    BC_KEY_DOWN,
    BC_KEY_UP,
    BC_KEY_PAGE_UP,
    BC_KEY_PAGE_DOWN,
    BC_KEY_HOME,
    BC_KEY_END,
    BC_KEY_CAPS_LOCK,
    BC_KEY_SCROLL_LOCK,
    BC_KEY_NUM_LOCK,
    BC_KEY_PRINT_SCREEN,
    BC_KEY_PAUSE,
    BC_KEY_F1,
    BC_KEY_F2,
    BC_KEY_F3,
    BC_KEY_F4,
    BC_KEY_F5,
    BC_KEY_F6,
    BC_KEY_F7,
    BC_KEY_F8,
    BC_KEY_F9,
    BC_KEY_F10,
    BC_KEY_F11,
    BC_KEY_F12,
    BC_KEY_F13,
    BC_KEY_F14,
    BC_KEY_F15,
    BC_KEY_F16,
    BC_KEY_F17,
    BC_KEY_F18,
    BC_KEY_F19,
    BC_KEY_F20,
    BC_KEY_F21,
    BC_KEY_F22,
    BC_KEY_F23,
    BC_KEY_F24,
    BC_KEY_F25,
    BC_KEY_KP_0,
    BC_KEY_KP_1,
    BC_KEY_KP_2,
    BC_KEY_KP_3,
    BC_KEY_KP_4,
    BC_KEY_KP_5,
    BC_KEY_KP_6,
    BC_KEY_KP_7,
    BC_KEY_KP_8,
    BC_KEY_KP_9,
    BC_KEY_KP_DECIMAL,
    BC_KEY_KP_DIVIDE,
    BC_KEY_KP_MULTIPLY,
    BC_KEY_KP_SUBTRACT,
    BC_KEY_KP_ADD,
    BC_KEY_KP_ENTER,
    BC_KEY_KP_EQUAL,
    BC_KEY_LEFT_SHIFT,
    BC_KEY_LEFT_CONTROL,
    BC_KEY_LEFT_ALT,
    BC_KEY_LEFT_SUPER,
    BC_KEY_RIGHT_SHIFT,
    BC_KEY_RIGHT_CONTROL,
    BC_KEY_RIGHT_ALT,
    BC_KEY_RIGHT_SUPER,
    BC_KEY_MENU,
    BC_KEY_BACK,
    BC_KEY_CHANNEL_UP,
    BC_KEY_CHANNEL_DOWN,
    BC_KEY_MEDIA_PLAY,
    BC_KEY_MEDIA_PAUSE,
    BC_KEY_MEDIA_STOP,
    BC_KEY_MEDIA_NEXT,
    BC_KEY_MEDIA_PREVIOUS,
    BC_KEY_MEDIA_REWIND,
    BC_KEY_MEDIA_FAST_FORWARD,
    BC_KEY_MEDIA_PLAY_PAUSE,
    BC_KEY_COUNT,
};

// Mouse Buttons
enum BCMouseButton
{
    BC_MOUSE_BUTTON_LEFT = 0,
    BC_MOUSE_BUTTON_RIGHT,
    BC_MOUSE_BUTTON_MIDDLE,
};

enum BCDisplayMode
{
    BC_DISPLAY_NORMAL,
    BC_DISPLAY_FULLSCREEN,
    BC_DISPLAY_DESKTOP,
    BC_DISPLAY_RESIZABLE
};

typedef struct
{
    int type;
    int id;
    int x;
    int y;
    void *data;
} BCEvent;

typedef struct
{
    const char *title;
    int width;
    int height;
    int format;
    int mode;
    bool vsync;
    int msaa;
    int orientation;
    void *surface;
} BCConfig;

#ifdef __cplusplus
extern "C" {
#endif

//
// !!! OVERRIDE THIS IN APP !!!
//
void BC_onConfig(BCConfig *config);
void BC_onCreate();
void BC_onDestroy();
void BC_onStart();
void BC_onStop();
void BC_onUpdate(float dt);
void BC_onDraw();
void BC_onEvent(BCEvent event);

// App
void bcQuit(int code);
float bcGetTime();
void bcShowKeyboard(bool show);
int bcGetDisplayWidth();
int bcGetDisplayHeight();
float bcGetDisplayAspectRatio();
float bcGetDisplayDensity();
int bcGetCommandLineArgs();
const char * bcGetCommandLineArg(int index);
void bcInputTextDialog(const char *text);
bool bcIsKeyboardConnected();
int bcGetAppKeyCode(int hwKeyCode);
bool bcSetAppKeyCode(int hwKeyCode, int appKeyCode);

// Input state
bool bcIsKeyDown(int key);
int bcGetMouseX();
int bcGetMouseY();
bool bcIsMouseDown(int button);
float bcGetMouseWheel();
float bcGetMouseDeltaX();
float bcGetMouseDeltaY();
bool bcIsTouchDown(int index);
float bcGetTouchX(int index);
float bcGetTouchY(int index);

#ifdef __cplusplus
}
#endif
