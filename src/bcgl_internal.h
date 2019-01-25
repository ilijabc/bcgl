#pragma once

#include <bcgl.h>
#include <bcgl_opengl.h>

#define MATRIX_STACK_SIZE 32

typedef struct
{
    int width;
    int height;
    void *nativeWindow;
} BCWindow;

//
// bcgl_app module
//

BCCallbacks bcGetCallbacks();

// App
bool bcAppWrapperStart(BCConfig *config);
int bcAppWrapperStop();
bool bcAppWrapperIsRunning();
void bcAppWrapperUpdate();
int bcAppWrapperRun(BCConfig *config);

// Window
BCWindow * bcCreateWindow(BCConfig *config);
void bcDestroyWindow(BCWindow *window);
void bcUpdateWindow(BCWindow *window);
void bcCloseWindow(BCWindow *window);
bool bcIsWindowOpened(BCWindow *window);
void bcPullWindowEvents(BCWindow *window);
BCWindow * bcGetWindow();
void bcSetWindow(BCWindow *window);

// Events
BCEvent * bcSendEvent(int type, int id, int x, int y);
int bcPullEvents();
BCEvent * bcGetEvent(int index);
void bcFlushEvents();
