#pragma once

#include <bcgl.h>
#include <bcgl_opengl.h>
#include <bcmath.h>

//
// bcgl_app module
//

// App
bool bcInit();
void bcTerm();

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
// bcgl_gfx module
//

void bcInitGfx();
void bcTermGfx();

void bcInitGfxDraw();
void bcTermGfxDraw();
