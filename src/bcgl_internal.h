#pragma once

#include <bcgl.h>
#include <bcgl_opengl.h>
#include <bcmath.h>

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
int bcAppMain(BCConfig *config);

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

//
// bcgl_files module
//

void bcInitFiles(void *ctx);
void bcTermFiles();

//
// bcgl_gfx module
//

void bcInitGfx();
void bcTermGfx();

void bcInitGfxDraw();
void bcTermGfxDraw();
