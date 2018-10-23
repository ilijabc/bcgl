#include <pthread.h>

#include "bcgl_internal.h"

static BCCallbacks s_Callbacks;
static BCWindow *s_Window = NULL;

// event queues
#define MAX_EVENTS 32
static BCEvent s_EventQueue[2][MAX_EVENTS];
static int s_CurrentIndex = 0;
static int s_CurrentQueue = 0;
static int s_PulledQueue = 1;
static pthread_mutex_t s_Mutex = PTHREAD_MUTEX_INITIALIZER;

static struct
{
    int x;
    int y;
    bool button[8];
    float wheel;
    float deltaX;
    float deltaY;
} s_MouseState = { 0 };

static bool s_KeyState[BC_KEY_COUNT] = { false };

static void processEvent(BCEvent *event)
{
    switch (event->type)
    {
    case BC_EVENT_MOUSEMOVE:
        s_MouseState.deltaX = event->x - s_MouseState.x;
        s_MouseState.deltaY = event->y - s_MouseState.y;
        s_MouseState.x = event->x;
        s_MouseState.y = event->y;
        break;
    case BC_EVENT_MOUSEPRESS:
    case BC_EVENT_MOUSERELEASE:
        s_MouseState.button[event->x] = (event->type == BC_EVENT_MOUSEPRESS);
        break;
    case BC_EVENT_MOUSEWHEEL:
        s_MouseState.wheel = event->y;
        break;
    case BC_EVENT_KEYPRESS:
    case BC_EVENT_KEYRELEASE:
        s_KeyState[event->x] = (event->type == BC_EVENT_KEYPRESS);
        break;
    case BC_EVENT_WINDOWSIZE:
        {
            BCWindow *win = bcGetWindow();
            win->width = event->x;
            win->height = event->y;
        }
        glViewport(0, 0, event->x, event->y);
        break;
    }
}

void bcInit(BCCallbacks callbacks)
{
    s_Callbacks = callbacks;
}

void bcAppMain(BCConfig *config)
{
    BCCallbacks callbacks = bcGetCallbacks();

    if (callbacks.onConfig)
        callbacks.onConfig(config);
    else
        bcLogWarning("Missing onConfig callback!");

    if (callbacks.onCreate)
        callbacks.onCreate();

    BCWindow *window = bcCreateWindow(config);
    if (window == NULL)
    {
        bcLogError("Unable to create window!");
        return;
    }
    bcSetWindow(window);

    if (callbacks.onStart)
        callbacks.onStart();

    // Main loop
    float lastTime = bcGetTime();
    while (bcIsWindowOpened(window))
    {
        float now = bcGetTime();
        float dt = now - lastTime;
        lastTime = now;
        // events
        bcPullWindowEvents(window);
        int n = bcPullEvents();
        for (int i = 0; i < n; i++)
        {
            BCEvent *e = bcGetEvent(i);
            if (callbacks.onEvent)
                callbacks.onEvent(e->type, e->x, e->y);
        }
        // update
        if (callbacks.onUpdate)
            callbacks.onUpdate(dt);
        bcUpdateWindow(window);
    }

    if (callbacks.onStop)
        callbacks.onStop();

    bcDestroyWindow(window);

    if (callbacks.onDestroy)
        callbacks.onDestroy();
}

int bcGetDisplayWidth()
{
    return bcGetWindow()->width;
}

int bcGetDisplayHeight()
{
    return bcGetWindow()->height;
}

float bcGetDisplayAspectRatio()
{
    return (float) bcGetWindow()->width / (float) bcGetWindow()->height;
}

BCCallbacks bcGetCallbacks()
{
    return s_Callbacks;
}

BCEvent * bcSendEvent(int type, int x, int y)
{
    pthread_mutex_lock(&s_Mutex);
    if (s_CurrentIndex == MAX_EVENTS)
    {
        pthread_mutex_unlock(&s_Mutex);
        bcLogWarning("Max app events reached!");
        return NULL;
    }
    BCEvent *event = &(s_EventQueue[s_CurrentQueue][s_CurrentIndex]);
    event->type = type;
    event->x = x;
    event->y = y;
    s_CurrentIndex++;
    pthread_mutex_unlock(&s_Mutex);
    return event;
}

int bcPullEvents()
{
    pthread_mutex_lock(&s_Mutex);
    int n = s_CurrentIndex;
    s_PulledQueue = s_CurrentQueue;
    s_CurrentQueue++;
    if (s_CurrentQueue == 2)
        s_CurrentQueue = 0;
    s_CurrentIndex = 0;
    // preliminear event processing
    for (int i = 0; i < n; i++)
    {
        processEvent(bcGetEvent(i));
    }
    pthread_mutex_unlock(&s_Mutex);
    return n;
}

BCEvent * bcGetEvent(int index)
{
    return &(s_EventQueue[s_PulledQueue][index]);
}

void bcQuit(int code)
{
    bcCloseWindow(s_Window);
}

BCWindow * bcGetWindow()
{
    return s_Window;
}

void bcSetWindow(BCWindow *window)
{
    s_Window = window;
}

//
// Input state
//

void bcResetStates()
{
    s_MouseState.wheel = 0;
    s_MouseState.deltaX = 0;
    s_MouseState.deltaY = 0;
}

void bcSetMousePosition(int x, int y)
{
    s_MouseState.x = x;
    s_MouseState.y = y;
}

bool bcIsKeyDown(int key)
{
    return s_KeyState[key];
}

int bcGetMouseX()
{
    return s_MouseState.x;
}

int bcGetMouseY()
{
    return s_MouseState.y;
}

bool bcIsMouseDown(int button)
{
    return s_MouseState.button[button];
}

float bcGetMouseWheel()
{
    return s_MouseState.wheel;
}

float bcGetMouseDeltaX()
{
    return s_MouseState.deltaX;
}

float bcGetMouseDeltaY()
{
    return s_MouseState.deltaY;
}
