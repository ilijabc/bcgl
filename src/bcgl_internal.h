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
bool bcInitApp();
void bcTermApp();

// Window
BCWindow * bcCreateWindow(BCConfig *config);
void bcDestroyWindow(BCWindow *window);
void bcUpdateWindow(BCWindow *window);
void bcCloseWindow(BCWindow *window);
bool bcIsWindowOpened(BCWindow *window);
BCWindow * bcGetWindow();

// Events
BCEvent * bcSendEvent(int type, int x, int y);
int bcPullEvents();
BCEvent * bcGetEvent(int index);

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
