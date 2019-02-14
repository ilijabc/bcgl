#include <pthread.h>

#include "bcgl_internal.h"

static BCCallbacks s_Callbacks;
static BCWindow *s_Window = NULL;
static int s_ExitCode = 0;
static float s_StartTime;

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

static struct
{
    bool down;
    float x;
    float y;
} s_TouchState[10] = { 0 };

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
        s_MouseState.button[event->id] = (event->type == BC_EVENT_MOUSEPRESS);
        break;
    case BC_EVENT_MOUSEWHEEL:
        s_MouseState.wheel = event->y;
        break;
    case BC_EVENT_KEYPRESS:
    case BC_EVENT_KEYRELEASE:
        s_KeyState[event->id] = (event->type == BC_EVENT_KEYPRESS);
        break;
    case BC_EVENT_WINDOWSIZE:
        {
            BCWindow *win = bcGetWindow();
            win->width = event->x;
            win->height = event->y;
        }
        break;
    case BC_EVENT_TOUCH_DOWN:
        s_TouchState[event->id].down = true;
        break;
    case BC_EVENT_TOUCH_UP:
        s_TouchState[event->id].down = false;
        break;
    case BC_EVENT_TOUCH_MOVE:
        s_TouchState[event->id].x = event->x;
        s_TouchState[event->id].y = event->y;
        break;
    }
}

void bcInit(BCCallbacks callbacks)
{
    s_Callbacks = callbacks;
}

void bcAppWrapperConfigure(BCConfig *config)
{
    BCCallbacks callbacks = bcGetCallbacks();

    if (callbacks.onConfig)
        callbacks.onConfig(config);
    else
        bcLogWarning("Missing onConfig callback!");
}

bool bcAppWrapperStart(BCConfig *config)
{
    BCCallbacks callbacks = bcGetCallbacks();

    if (callbacks.onCreate)
        callbacks.onCreate();

    BCWindow *window = bcCreateWindow(config);
    if (window == NULL)
    {
        bcLogError("Unable to create window!");
        return false;
    }
    bcSetWindow(window);

    if (callbacks.onStart)
        callbacks.onStart();

    s_StartTime = bcGetTime();

    return true;
}

int bcAppWrapperStop()
{
    BCCallbacks callbacks = bcGetCallbacks();

    if (callbacks.onStop)
        callbacks.onStop();

    bcDestroyWindow(s_Window);

    if (callbacks.onDestroy)
        callbacks.onDestroy();

    return s_ExitCode;
}

bool bcAppWrapperIsRunning()
{
    return bcIsWindowOpened(s_Window);
}

void bcAppWrapperUpdate()
{
    BCCallbacks callbacks = bcGetCallbacks();

    float now = bcGetTime();
    float dt = now - s_StartTime;
    s_StartTime = now;

    // events
    bcPullWindowEvents(s_Window);
    int n = bcPullEvents();
    for (int i = 0; i < n; i++)
    {
        BCEvent *e = bcGetEvent(i);
        if (callbacks.onEvent)
            callbacks.onEvent(*e);
    }

    // update
    if (callbacks.onUpdate)
        callbacks.onUpdate(dt);

    bcUpdateWindow(s_Window);
}

int bcAppWrapperRun(BCConfig *config)
{
    bcAppWrapperConfigure(config);

    if (!bcAppWrapperStart(config))
    {
        return -99;
    }

    while (bcAppWrapperIsRunning())
    {
        bcAppWrapperUpdate();
    }

    return bcAppWrapperStop();
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

BCEvent * bcSendEvent(int type, int id, int x, int y)
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
    event->id = id;
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

void bcFlushEvents()
{
    pthread_mutex_lock(&s_Mutex);
    s_CurrentIndex = 0;
    pthread_mutex_unlock(&s_Mutex);
}

void bcQuit(int code)
{
    s_ExitCode = code;
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

bool bcIsTouchDown(int index)
{
    return s_TouchState[index].down;
}

float bcGetTouchX(int index)
{
    return s_TouchState[index].x;
}

float bcGetTouchY(int index)
{
    return s_TouchState[index].y;
}
