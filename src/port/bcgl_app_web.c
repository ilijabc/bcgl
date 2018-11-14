#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>
#include <emscripten/emscripten.h>

#include "../bcgl_internal.h"

typedef struct
{
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
} BCGLESWindow;

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

static void rendererThread()
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

    emscripten_set_main_loop(rendererThread, 0, 1);

    // bcAppWrapperStop();

    bcTermFiles();

    return 0;
}
