#include "../bcgl_internal.h"
#include <GLFW/glfw3.h>

static struct
{
    int glfw_code;
    int app_code;
} s_KeyMap[] = {
    { GLFW_KEY_SPACE, BC_KEY_SPACE },
    { GLFW_KEY_APOSTROPHE, BC_KEY_APOSTROPHE },
    { GLFW_KEY_COMMA, BC_KEY_COMMA },
    { GLFW_KEY_MINUS, BC_KEY_MINUS },
    { GLFW_KEY_PERIOD, BC_KEY_PERIOD },
    { GLFW_KEY_SLASH, BC_KEY_SLASH },
    { GLFW_KEY_0, BC_KEY_0 },
    { GLFW_KEY_1, BC_KEY_1 },
    { GLFW_KEY_2, BC_KEY_2 },
    { GLFW_KEY_3, BC_KEY_3 },
    { GLFW_KEY_4, BC_KEY_4 },
    { GLFW_KEY_5, BC_KEY_5 },
    { GLFW_KEY_6, BC_KEY_6 },
    { GLFW_KEY_7, BC_KEY_7 },
    { GLFW_KEY_8, BC_KEY_8 },
    { GLFW_KEY_9, BC_KEY_9 },
    { GLFW_KEY_SEMICOLON, BC_KEY_SEMICOLON },
    { GLFW_KEY_EQUAL, BC_KEY_EQUAL },
    { GLFW_KEY_A, BC_KEY_A },
    { GLFW_KEY_B, BC_KEY_B },
    { GLFW_KEY_C, BC_KEY_C },
    { GLFW_KEY_D, BC_KEY_D },
    { GLFW_KEY_E, BC_KEY_E },
    { GLFW_KEY_F, BC_KEY_F },
    { GLFW_KEY_G, BC_KEY_G },
    { GLFW_KEY_H, BC_KEY_H },
    { GLFW_KEY_I, BC_KEY_I },
    { GLFW_KEY_J, BC_KEY_J },
    { GLFW_KEY_K, BC_KEY_K },
    { GLFW_KEY_L, BC_KEY_L },
    { GLFW_KEY_M, BC_KEY_M },
    { GLFW_KEY_N, BC_KEY_N },
    { GLFW_KEY_O, BC_KEY_O },
    { GLFW_KEY_P, BC_KEY_P },
    { GLFW_KEY_Q, BC_KEY_Q },
    { GLFW_KEY_R, BC_KEY_R },
    { GLFW_KEY_S, BC_KEY_S },
    { GLFW_KEY_T, BC_KEY_T },
    { GLFW_KEY_U, BC_KEY_U },
    { GLFW_KEY_V, BC_KEY_V },
    { GLFW_KEY_W, BC_KEY_W },
    { GLFW_KEY_X, BC_KEY_X },
    { GLFW_KEY_Y, BC_KEY_Y },
    { GLFW_KEY_Z, BC_KEY_Z },
    { GLFW_KEY_LEFT_BRACKET, BC_KEY_LEFT_BRACKET },
    { GLFW_KEY_BACKSLASH, BC_KEY_BACKSLASH },
    { GLFW_KEY_RIGHT_BRACKET, BC_KEY_RIGHT_BRACKET },
    { GLFW_KEY_GRAVE_ACCENT, BC_KEY_GRAVE_ACCENT },
    { GLFW_KEY_WORLD_1, BC_KEY_WORLD_1 },
    { GLFW_KEY_WORLD_2, BC_KEY_WORLD_2 },
    { GLFW_KEY_ESCAPE, BC_KEY_ESCAPE },
    { GLFW_KEY_ENTER, BC_KEY_ENTER },
    { GLFW_KEY_TAB, BC_KEY_TAB },
    { GLFW_KEY_BACKSPACE, BC_KEY_BACKSPACE },
    { GLFW_KEY_INSERT, BC_KEY_INSERT },
    { GLFW_KEY_DELETE, BC_KEY_DELETE },
    { GLFW_KEY_RIGHT, BC_KEY_RIGHT },
    { GLFW_KEY_LEFT, BC_KEY_LEFT },
    { GLFW_KEY_DOWN, BC_KEY_DOWN },
    { GLFW_KEY_UP, BC_KEY_UP },
    { GLFW_KEY_PAGE_UP, BC_KEY_PAGE_UP },
    { GLFW_KEY_PAGE_DOWN, BC_KEY_PAGE_DOWN },
    { GLFW_KEY_HOME, BC_KEY_HOME },
    { GLFW_KEY_END, BC_KEY_END },
    { GLFW_KEY_CAPS_LOCK, BC_KEY_CAPS_LOCK },
    { GLFW_KEY_SCROLL_LOCK, BC_KEY_SCROLL_LOCK },
    { GLFW_KEY_NUM_LOCK, BC_KEY_NUM_LOCK },
    { GLFW_KEY_PRINT_SCREEN, BC_KEY_PRINT_SCREEN },
    { GLFW_KEY_PAUSE, BC_KEY_PAUSE },
    { GLFW_KEY_F1, BC_KEY_F1 },
    { GLFW_KEY_F2, BC_KEY_F2 },
    { GLFW_KEY_F3, BC_KEY_F3 },
    { GLFW_KEY_F4, BC_KEY_F4 },
    { GLFW_KEY_F5, BC_KEY_F5 },
    { GLFW_KEY_F6, BC_KEY_F6 },
    { GLFW_KEY_F7, BC_KEY_F7 },
    { GLFW_KEY_F8, BC_KEY_F8 },
    { GLFW_KEY_F9, BC_KEY_F9 },
    { GLFW_KEY_F10, BC_KEY_F10 },
    { GLFW_KEY_F11, BC_KEY_F11 },
    { GLFW_KEY_F12, BC_KEY_F12 },
    { GLFW_KEY_F13, BC_KEY_F13 },
    { GLFW_KEY_F14, BC_KEY_F14 },
    { GLFW_KEY_F15, BC_KEY_F15 },
    { GLFW_KEY_F16, BC_KEY_F16 },
    { GLFW_KEY_F17, BC_KEY_F17 },
    { GLFW_KEY_F18, BC_KEY_F18 },
    { GLFW_KEY_F19, BC_KEY_F19 },
    { GLFW_KEY_F20, BC_KEY_F20 },
    { GLFW_KEY_F21, BC_KEY_F21 },
    { GLFW_KEY_F22, BC_KEY_F22 },
    { GLFW_KEY_F23, BC_KEY_F23 },
    { GLFW_KEY_F24, BC_KEY_F24 },
    { GLFW_KEY_F25, BC_KEY_F25 },
    { GLFW_KEY_KP_0, BC_KEY_KP_0 },
    { GLFW_KEY_KP_1, BC_KEY_KP_1 },
    { GLFW_KEY_KP_2, BC_KEY_KP_2 },
    { GLFW_KEY_KP_3, BC_KEY_KP_3 },
    { GLFW_KEY_KP_4, BC_KEY_KP_4 },
    { GLFW_KEY_KP_5, BC_KEY_KP_5 },
    { GLFW_KEY_KP_6, BC_KEY_KP_6 },
    { GLFW_KEY_KP_7, BC_KEY_KP_7 },
    { GLFW_KEY_KP_8, BC_KEY_KP_8 },
    { GLFW_KEY_KP_9, BC_KEY_KP_9 },
    { GLFW_KEY_KP_DECIMAL, BC_KEY_KP_DECIMAL },
    { GLFW_KEY_KP_DIVIDE, BC_KEY_KP_DIVIDE },
    { GLFW_KEY_KP_MULTIPLY, BC_KEY_KP_MULTIPLY },
    { GLFW_KEY_KP_SUBTRACT, BC_KEY_KP_SUBTRACT },
    { GLFW_KEY_KP_ADD, BC_KEY_KP_ADD },
    { GLFW_KEY_KP_ENTER, BC_KEY_KP_ENTER },
    { GLFW_KEY_KP_EQUAL, BC_KEY_KP_EQUAL },
    { GLFW_KEY_LEFT_SHIFT, BC_KEY_LEFT_SHIFT },
    { GLFW_KEY_LEFT_CONTROL, BC_KEY_LEFT_CONTROL },
    { GLFW_KEY_LEFT_ALT, BC_KEY_LEFT_ALT },
    { GLFW_KEY_LEFT_SUPER, BC_KEY_LEFT_SUPER },
    { GLFW_KEY_RIGHT_SHIFT, BC_KEY_RIGHT_SHIFT },
    { GLFW_KEY_RIGHT_CONTROL, BC_KEY_RIGHT_CONTROL },
    { GLFW_KEY_RIGHT_ALT, BC_KEY_RIGHT_ALT },
    { GLFW_KEY_RIGHT_SUPER, BC_KEY_RIGHT_SUPER },
    { GLFW_KEY_MENU, BC_KEY_MENU },
    { -1, BC_KEY_COUNT }
};

static struct
{
    int x, y;
} s_LastCursorPos = { 0, 0 };

static struct
{
    int argc;
    char **argv;
} s_CommandLine = { 0, 0 };

//
// Private Functions
//

static void glfw_ErrorCallback(int error, const char *description)
{
    bcLogError("GLFW error: %d:%s", error, description);
}

static void glfw_KeyCallback(GLFWwindow *nativeWindow, int keyCode, int scanCode, int action, int mods)
{
    int appCode = bcGetAppKeyCode(keyCode);
    if (appCode == BC_KEY_UNKNOWN)
    {
        bcLogWarning("Unknown key code: %d !", keyCode);
        return;
    }
    if (action == GLFW_PRESS)
        bcSendEvent(BC_EVENT_KEY_PRESS, appCode, keyCode, 0, NULL);
    else if (action == GLFW_RELEASE)
        bcSendEvent(BC_EVENT_KEY_RELEASE, appCode, keyCode, 0, NULL);
    else if (action == GLFW_REPEAT)
        bcSendEvent(BC_EVENT_KEY_REPEAT, appCode, keyCode, 0, NULL);
    // missing key chars
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (keyCode == GLFW_KEY_BACKSPACE)
            bcSendEvent(BC_EVENT_KEY_CHAR, 8, 0, 0, NULL);
        else if (keyCode == GLFW_KEY_ENTER)
            bcSendEvent(BC_EVENT_KEY_CHAR, 13, 0, 0, NULL);
    }
}

static void glfw_KeyCharCallback(GLFWwindow *nativeWindow, unsigned int code)
{
    bcSendEvent(BC_EVENT_KEY_CHAR, code, 0, 0, NULL);
}

static void glfw_CursorPosCallback(GLFWwindow *nativeWindow, double x, double y)
{
    s_LastCursorPos.x = x;
    s_LastCursorPos.y = y;
    bcSendEvent(BC_EVENT_MOUSE_MOVE, 0, x, y, NULL);
}

static void glfw_MouseButtonCallback(GLFWwindow *nativeWindow, int button, int action, int mods)
{
    bcSendEvent((action == GLFW_PRESS) ? BC_EVENT_MOUSE_PRESS : BC_EVENT_MOUSE_RELEASE, button, s_LastCursorPos.x, s_LastCursorPos.y, NULL);
}

static void glfw_ScrollCallback(GLFWwindow *nativeWindow, double dx, double dy)
{
    bcSendEvent(BC_EVENT_MOUSE_WHEEL, 0, dx, dy, NULL);
}

static void glfw_WindowSizeCallback(GLFWwindow *nativeWindow, int width, int height)
{
    bcFlushEvents();
    bcSendEvent(BC_EVENT_WINDOW_SIZE, 0, width, height, NULL);
}

static void glfw_WindowFocusCallback(GLFWwindow *nativeWindow, int focused)
{
    bcSendEvent(BC_EVENT_WINDOW_FOCUS, focused, 0, 0, NULL);
}

static void glfw_WindowIconifyCallback(GLFWwindow *nativeWindow, int iconified)
{
    bcSendEvent(BC_EVENT_WINDOW_ICONIFY, iconified, 0, 0, NULL);
}

static void glfw_WindowRefreshCallback(GLFWwindow* window)
{
    glfwSwapBuffers(window);
}

//
// App
//

float bcGetTime()
{
    return (float) glfwGetTime();
}

void bcShowKeyboard(bool show)
{
}

float bcGetDisplayDensity()
{
    return 1.0f;
}

int bcGetCommandLineArgs()
{
    return s_CommandLine.argc;
}

const char * bcGetCommandLineArg(int index)
{
    if (index < 0 || index >= s_CommandLine.argc)
        return NULL;
    return s_CommandLine.argv[index];
}

void bcInputTextDialog(const char *text)
{
}

bool bcIsKeyboardConnected()
{
    return true;
}

int bcGetAppKeyCode(int hwKeyCode)
{
    for (int i = 0; i < s_KeyMap[i].app_code != BC_KEY_COUNT; i++)
    {
        if (s_KeyMap[i].glfw_code == hwKeyCode)
        {
            return s_KeyMap[i].app_code;
        }
    }
    return BC_KEY_UNKNOWN;
}

bool bcSetAppKeyCode(int hwKeyCode, int appKeyCode)
{
    for (int i = 0; i < s_KeyMap[i].app_code != BC_KEY_COUNT; i++)
    {
        if (s_KeyMap[i].glfw_code == hwKeyCode)
        {
            s_KeyMap[i].app_code = appKeyCode;
            return true;
        }
    }
    return false;
}

//
// Window
//

BCWindow * bcCreateWindow(BCConfig *config)
{
    const GLFWvidmode *screen = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Configure our window
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, config->msaa);
    glfwWindowHint(GLFW_RESIZABLE, config->mode == BC_DISPLAY_RESIZABLE);
    if (config->mode == BC_DISPLAY_DESKTOP)
    {
        glfwWindowHint(GLFW_DECORATED, GL_FALSE);
        config->width = screen->width;
        config->height = screen->height;
    }

    // Create the window
    GLFWwindow * nativeWindow = glfwCreateWindow(
        config->width,
        config->height,
        config->title ? config->title : "BCGL",
        (config->mode == BC_DISPLAY_FULLSCREEN) ? glfwGetPrimaryMonitor() : NULL,
        NULL);
    if (nativeWindow == NULL)
    {
        bcLogError("Failed to create the GLFW window");
        return NULL;
    }

    // Setup callbacks
    glfwSetKeyCallback(nativeWindow, glfw_KeyCallback);
    glfwSetCharCallback(nativeWindow, glfw_KeyCharCallback);
    glfwSetCursorPosCallback(nativeWindow, glfw_CursorPosCallback);
    glfwSetMouseButtonCallback(nativeWindow, glfw_MouseButtonCallback);
    glfwSetScrollCallback(nativeWindow, glfw_ScrollCallback);
    glfwSetWindowSizeCallback(nativeWindow, glfw_WindowSizeCallback);
    glfwSetWindowFocusCallback(nativeWindow, glfw_WindowFocusCallback);
    glfwSetWindowIconifyCallback(nativeWindow, glfw_WindowIconifyCallback);
    glfwSetWindowRefreshCallback(nativeWindow, glfw_WindowRefreshCallback);

    // Center our window
    if (config->mode == 0)
    {
        glfwSetWindowPos(
            nativeWindow,
            (screen->width - config->width) / 2,
            (screen->height - config->height) / 2);
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(nativeWindow);

    // Load GL extensions
#ifdef SUPPORT_GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        bcLogError("Failed to initialize OpenGL context");
        return NULL;
    }
#endif

    // Enable v-sync
    glfwSwapInterval(config->vsync ? 1 : 0);

    // Make the window visible
    glfwShowWindow(nativeWindow);

    // get window size
    // {
    //     int width, height;
    //     glfwGetWindowSize(nativeWindow, &width, &height);
    //     window->setSize(width, height);
    // }

    BCWindow *window = NEW_OBJECT(BCWindow);
    window->width = config->width;
    window->height = config->height;
    window->nativeWindow = nativeWindow;
    return window;
}

void bcDestroyWindow(BCWindow *window)
{
    glfwDestroyWindow(window->nativeWindow);
    free(window);
}

void bcUpdateWindow(BCWindow *window)
{
    bcInputStateReset();
    glfwSwapBuffers(window->nativeWindow);
}

void bcCloseWindow(BCWindow *window)
{
    glfwSetWindowShouldClose(window->nativeWindow, 1);
}

bool bcIsWindowOpened(BCWindow *window)
{
    return !glfwWindowShouldClose(window->nativeWindow);
}

void bcPullWindowEvents(BCWindow *window)
{
    glfwPollEvents();
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    s_CommandLine.argc = argc;
    s_CommandLine.argv = argv;

    glfwSetErrorCallback(glfw_ErrorCallback);
    if (glfwInit() != GL_TRUE)
    {
        bcLogError("Unable to initialize GLFW");
        return false;
    }
    bcLog("GLFW: %s", glfwGetVersionString());

    bcInitFiles(NULL);

    bcAppCreate();

    // Get the resolution of the primary monitor
    const GLFWvidmode *screen = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Default config
    BCConfig config;
    config.title = NULL;
    config.width = screen->width;
    config.height = screen->height;
    config.format = 0;
    config.mode = 0;
    config.vsync = true;
    config.msaa = 0;
    config.orientation = 0;
    config.surface = NULL;

    bcAppConfig(&config);

    if (!bcAppStart(&config))
    {
        return 1;
    }

    bcAppMainLoop();

    bcAppStop();

    bcTermFiles();

    bcAppDestroy();

    glfwTerminate();

    return 0;
}
