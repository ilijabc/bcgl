#pragma once

#include <bcgl.h>
#include <bcgl_opengl.h>

#define BC_MATRIX_STACK_SIZE 32

typedef struct
{
    int width;
    int height;
    void *nativeWindow;
} BCWindow;

//
// bcgl_app module
//

// App
void bcAppCreate();
void bcAppDestroy();
void bcAppConfig(BCConfig *config);
bool bcAppStart(BCConfig *config);
void bcAppStop();
void bcAppUpdate();
void bcAppMainLoop();

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
BCEvent * bcDequeueEvent();
void bcQueueEvent(BCEvent *event);
void bcSendEvent(int type, int id, int x, int y);
int bcPullEvents();
BCEvent * bcGetEvent(int index);
void bcFlushEvents();
void bcInputStateReset();

//
// bcgl_file module
//

void bcInitFiles(void *ctx, const char *local_path, const char *external_path);
void bcTermFiles();

//
// bcgl_gfx module
//

void bcInitGfx();
void bcTermGfx();
