#include "bcgl_internal.h"

// event queues
#define MAX_EVENTS 32
static BCEvent s_EventQueue[2][MAX_EVENTS];
static int s_CurrentIndex = 0;
static int s_CurrentQueue = 0;
static int s_PulledQueue = 1;

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
    }
}

BCEvent * bcSendEvent(int type, int x, int y)
{
    if (s_CurrentIndex == MAX_EVENTS)
    {
        bcLogWarning("Max app events reached!");
        return NULL;
    }
    BCEvent *event = &(s_EventQueue[s_CurrentQueue][s_CurrentIndex]);
    event->type = type;
    event->x = x;
    event->y = y;
    s_CurrentIndex++;
    return event;
}

int bcPullEvents()
{
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
    return n;
}

BCEvent * bcGetEvent(int index)
{
    return &(s_EventQueue[s_PulledQueue][index]);
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
