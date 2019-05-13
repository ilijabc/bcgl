#include <pthread.h>

#include "bcgl_internal.h"

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

static void processEvent(BCEvent *event);

void bcAppWrapperConfigure(BCConfig *config)
{
    BC_onConfig(config);
}

bool bcAppWrapperStart(BCConfig *config)
{
    BC_onCreate();

    BCWindow *window = bcCreateWindow(config);
    if (window == NULL)
    {
        bcLogError("Unable to create window!");
        return false;
    }
    bcSetWindow(window);

    BC_onStart();

    s_StartTime = bcGetTime();

    return true;
}

int bcAppWrapperStop()
{
    BC_onStop();

    bcDestroyWindow(s_Window);

    BC_onDestroy();

    return s_ExitCode;
}

bool bcAppWrapperIsRunning()
{
    return bcIsWindowOpened(s_Window);
}

void bcAppWrapperUpdate()
{
    float now = bcGetTime();
    float dt = now - s_StartTime;
    s_StartTime = now;

    // events
    bcPullWindowEvents(s_Window);
    int n = bcPullEvents();
    for (int i = 0; i < n; i++)
    {
        BCEvent *e = bcGetEvent(i);
        BC_onEvent(*e);
    }

    // update
    BC_onUpdate(dt);

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

BCEvent * bcDequeueEvent()
{
    pthread_mutex_lock(&s_Mutex);
    if (s_CurrentIndex == MAX_EVENTS)
    {
        pthread_mutex_unlock(&s_Mutex);
        bcLogWarning("Max app events reached!");
        return NULL;
    }
    return &(s_EventQueue[s_CurrentQueue][s_CurrentIndex]);
}

void bcQueueEvent(BCEvent *event)
{
    if (event != &(s_EventQueue[s_CurrentQueue][s_CurrentIndex]))
    {
        pthread_mutex_unlock(&s_Mutex);
        bcLogWarning("Wrong dequeued event!");
        return;
    }
    s_CurrentIndex++;
    pthread_mutex_unlock(&s_Mutex);
}

void bcSendEvent(int type, int id, int x, int y)
{
    BCEvent *event = bcDequeueEvent();
    if (event)
    {
        event->type = type;
        event->id = id;
        event->x = x;
        event->y = y;
        bcQueueEvent(event);
    }
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

static struct
{
    struct {
        int x;
        int y;
        bool button[8];
        float wheel;
        float deltaX;
        float deltaY;
    } mouse;
    struct {
        bool down[BC_KEY_COUNT];
    } keyboard;
    struct
    {
        bool down;
        float x;
        float y;
    } touch[10];
} s_InputState = { 0 };

void bcInputStateReset()
{
    s_InputState.mouse.wheel = 0;
    s_InputState.mouse.deltaX = 0;
    s_InputState.mouse.deltaY = 0;
}

bool bcIsKeyDown(int key)
{
    return s_InputState.keyboard.down[key];
}

int bcGetMouseX()
{
    return s_InputState.mouse.x;
}

int bcGetMouseY()
{
    return s_InputState.mouse.y;
}

bool bcIsMouseDown(int button)
{
    return s_InputState.mouse.button[button];
}

float bcGetMouseWheel()
{
    return s_InputState.mouse.wheel;
}

float bcGetMouseDeltaX()
{
    return s_InputState.mouse.deltaX;
}

float bcGetMouseDeltaY()
{
    return s_InputState.mouse.deltaY;
}

bool bcIsTouchDown(int index)
{
    return s_InputState.touch[index].down;
}

float bcGetTouchX(int index)
{
    return s_InputState.touch[index].x;
}

float bcGetTouchY(int index)
{
    return s_InputState.touch[index].y;
}

void processEvent(BCEvent *event)
{
    switch (event->type)
    {
    case BC_EVENT_MOUSE_MOVE:
        s_InputState.mouse.deltaX = event->x - s_InputState.mouse.x;
        s_InputState.mouse.deltaY = event->y - s_InputState.mouse.y;
        s_InputState.mouse.x = event->x;
        s_InputState.mouse.y = event->y;
        break;
    case BC_EVENT_MOUSE_PRESS:
    case BC_EVENT_MOUSE_RELEASE:
        s_InputState.mouse.button[event->id] = (event->type == BC_EVENT_MOUSE_PRESS);
        break;
    case BC_EVENT_MOUSE_WHEEL:
        s_InputState.mouse.wheel = event->y;
        break;
    case BC_EVENT_KEY_PRESS:
    case BC_EVENT_KEY_RELEASE:
        s_InputState.keyboard.down[event->id] = (event->type == BC_EVENT_KEY_PRESS);
        break;
    case BC_EVENT_WINDOW_SIZE:
        {
            BCWindow *win = bcGetWindow();
            win->width = event->x;
            win->height = event->y;
        }
        break;
    case BC_EVENT_TOUCH_DOWN:
        s_InputState.touch[event->id].down = true;
        break;
    case BC_EVENT_TOUCH_UP:
        s_InputState.touch[event->id].down = false;
        break;
    case BC_EVENT_TOUCH_MOVE:
        s_InputState.touch[event->id].x = event->x;
        s_InputState.touch[event->id].y = event->y;
        break;
    }
}
