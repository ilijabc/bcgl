#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include "../bcgl_internal.h"

typedef struct
{
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
} BCGLESWindow;

static struct
{
    const char *key;
    int code;
} s_KeyMap[] = {
    { "Space", BC_KEY_SPACE },
    { "Backquote", BC_KEY_APOSTROPHE },
    { "Comma", BC_KEY_COMMA },
    { "Minus", BC_KEY_MINUS },
    { "Period", BC_KEY_PERIOD },
    { "Slash", BC_KEY_SLASH },
    { "Digit0", BC_KEY_0 },
    { "Digit1", BC_KEY_1 },
    { "Digit2", BC_KEY_2 },
    { "Digit3", BC_KEY_3 },
    { "Digit4", BC_KEY_4 },
    { "Digit5", BC_KEY_5 },
    { "Digit6", BC_KEY_6 },
    { "Digit7", BC_KEY_7 },
    { "Digit8", BC_KEY_8 },
    { "Digit9", BC_KEY_9 },
    { "Semicolon", BC_KEY_SEMICOLON },
    { "Equal", BC_KEY_EQUAL },
    { "KeyA", BC_KEY_A },
    { "KeyB", BC_KEY_B },
    { "KeyC", BC_KEY_C },
    { "KeyD", BC_KEY_D },
    { "KeyE", BC_KEY_E },
    { "KeyF", BC_KEY_F },
    { "KeyG", BC_KEY_G },
    { "KeyH", BC_KEY_H },
    { "KeyI", BC_KEY_I },
    { "KeyJ", BC_KEY_J },
    { "KeyK", BC_KEY_K },
    { "KeyL", BC_KEY_L },
    { "KeyM", BC_KEY_M },
    { "KeyN", BC_KEY_N },
    { "KeyO", BC_KEY_O },
    { "KeyP", BC_KEY_P },
    { "KeyQ", BC_KEY_Q },
    { "KeyR", BC_KEY_R },
    { "KeyS", BC_KEY_S },
    { "KeyT", BC_KEY_T },
    { "KeyU", BC_KEY_U },
    { "KeyV", BC_KEY_V },
    { "KeyW", BC_KEY_W },
    { "KeyX", BC_KEY_X },
    { "KeyY", BC_KEY_Y },
    { "KeyZ", BC_KEY_Z },
    { "BracketLeft", BC_KEY_LEFT_BRACKET },
    { "Backslash", BC_KEY_BACKSLASH },
    { "RightBracket", BC_KEY_RIGHT_BRACKET },
    { "Backquote", BC_KEY_GRAVE_ACCENT },
    { "Escape", BC_KEY_ESCAPE },
    { "Enter", BC_KEY_ENTER },
    { "Tab", BC_KEY_TAB },
    { "Backspace", BC_KEY_BACKSPACE },
    { "Insert", BC_KEY_INSERT },
    { "Delete", BC_KEY_DELETE },
    { "ArrowRight", BC_KEY_RIGHT },
    { "ArrowLeft", BC_KEY_LEFT },
    { "ArrowDown", BC_KEY_DOWN },
    { "ArrowUp", BC_KEY_UP },
    { "PageUp", BC_KEY_PAGE_UP },
    { "PageDown", BC_KEY_PAGE_DOWN },
    { "Home", BC_KEY_HOME },
    { "End", BC_KEY_END },
    { "CapsLock", BC_KEY_CAPS_LOCK },
    { "ScrollLock", BC_KEY_SCROLL_LOCK },
    { "NumLock", BC_KEY_NUM_LOCK },
    { "PrintScreen", BC_KEY_PRINT_SCREEN },
    { "Pause", BC_KEY_PAUSE },
    { "F1", BC_KEY_F1 },
    { "F2", BC_KEY_F2 },
    { "F3", BC_KEY_F3 },
    { "F4", BC_KEY_F4 },
    { "F5", BC_KEY_F5 },
    { "F6", BC_KEY_F6 },
    { "F7", BC_KEY_F7 },
    { "F8", BC_KEY_F8 },
    { "F9", BC_KEY_F9 },
    { "F10", BC_KEY_F10 },
    { "F11", BC_KEY_F11 },
    { "F12", BC_KEY_F12 },
    { "Numpad0", BC_KEY_KP_0 },
    { "Numpad1", BC_KEY_KP_1 },
    { "Numpad2", BC_KEY_KP_2 },
    { "Numpad3", BC_KEY_KP_3 },
    { "Numpad4", BC_KEY_KP_4 },
    { "Numpad5", BC_KEY_KP_5 },
    { "Numpad6", BC_KEY_KP_6 },
    { "Numpad7", BC_KEY_KP_7 },
    { "Numpad8", BC_KEY_KP_8 },
    { "Numpad9", BC_KEY_KP_9 },
    { "NumpadDecimal", BC_KEY_KP_DECIMAL },
    { "NumpadDivide", BC_KEY_KP_DIVIDE },
    { "NumpadMultiply", BC_KEY_KP_MULTIPLY },
    { "NumpadSubtract", BC_KEY_KP_SUBTRACT },
    { "NumpadAdd", BC_KEY_KP_ADD },
    { "NumpadEnter", BC_KEY_KP_ENTER },
    { "NumpadEqual", BC_KEY_KP_EQUAL },
    { "ShiftLeft", BC_KEY_LEFT_SHIFT },
    { "ControlLeft", BC_KEY_LEFT_CONTROL },
    { "AltLeft", BC_KEY_LEFT_ALT },
    { "ShiftRight", BC_KEY_RIGHT_SHIFT },
    { "ControlRight", BC_KEY_RIGHT_CONTROL },
    { "AltRight", BC_KEY_RIGHT_ALT },
    { "ContextMenu", BC_KEY_MENU },
    { NULL, BC_KEY_COUNT },
};

static int convertWebKeyCode(const char *key)
{
    for (int i = 0; s_KeyMap[i].key; i++)
    {
        if (strcmp(s_KeyMap[i].key, key) == 0)
        {
            return s_KeyMap[i].code;
        }
    }
    return BC_KEY_UNKNOWN;
}

// Window

void bcCloseWindow(BCWindow *window)
{
}

bool bcIsWindowOpened(BCWindow *window)
{
    return true;
}

void bcPullWindowEvents(BCWindow *window)
{
}

// App

void bcShowKeyboard(bool show)
{
}

float bcGetDisplayDensity()
{
    return 1.0f;
}

static EM_BOOL s_key_callback_func(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    int code = convertWebKeyCode(keyEvent->code);
    switch (eventType)
    {
    case EMSCRIPTEN_EVENT_KEYDOWN:
        bcSendEvent(keyEvent->repeat ? BC_EVENT_KEYREPEAT : BC_EVENT_KEYPRESS, code, keyEvent->keyCode, 0);
        break;
    case EMSCRIPTEN_EVENT_KEYUP:
        bcSendEvent(BC_EVENT_KEYRELEASE, code, keyEvent->keyCode, 0);
        break;
    default:
        bcLogWarning("Unhandled event: %d", eventType);
    }
    return true;
}

static EM_BOOL s_mouse_callback_func(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    switch (eventType)
    {
    case EMSCRIPTEN_EVENT_MOUSEDOWN:
        bcSendEvent(BC_EVENT_MOUSEPRESS, mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
        break;
    case EMSCRIPTEN_EVENT_MOUSEUP:
        bcSendEvent(BC_EVENT_MOUSERELEASE, mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
        break;
    case EMSCRIPTEN_EVENT_MOUSEMOVE:
        bcSendEvent(BC_EVENT_MOUSEMOVE, mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
        break;
    }
    return true;
}

static EM_BOOL s_wheel_callback_func(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData)
{
    bcSendEvent(BC_EVENT_MOUSEWHEEL, 0, wheelEvent->deltaX, wheelEvent->deltaY);
    return true;
}

static EM_BOOL s_webgl_context_callback(int eventType, const void *reserved, void *userData)
{
    switch (eventType)
    {
    case EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST:
        bcLog("WebGL LOST");
        break;
    case EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED:
        bcLog("WebGL RESTORED");
        break;
    }
    return true;
}

static void s_main_loop()
{
    bcAppWrapperUpdate();
}

int bcRunMain()
{
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

    if (!bcAppWrapperStart(&config))
    {
        return -99;
    }

    const char *target = "canvas";
    void *userData = NULL;
    EM_BOOL useCapture = true;

    emscripten_set_keydown_callback(NULL, userData, useCapture, s_key_callback_func);
    emscripten_set_keyup_callback(NULL, userData, useCapture, s_key_callback_func);
    emscripten_set_mousedown_callback(target, userData, useCapture, s_mouse_callback_func);
    emscripten_set_mouseup_callback(target, userData, useCapture, s_mouse_callback_func);
    emscripten_set_mousemove_callback(target, userData, useCapture, s_mouse_callback_func);
    emscripten_set_wheel_callback(target, userData, useCapture, s_wheel_callback_func);
    emscripten_set_webglcontextlost_callback(target, userData, useCapture, s_webgl_context_callback);
    emscripten_set_webglcontextrestored_callback(target, userData, useCapture, s_webgl_context_callback);

    emscripten_set_main_loop(s_main_loop, 0, 1);

    // bcAppWrapperStop();

    bcTermFiles();

    return 0;
}

int main(int argc, char **argv)
{
    BC_main();
    return bcRunMain();
}
