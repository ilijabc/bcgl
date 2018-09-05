#pragma once

#include <android/native_window.h>

#define EVENT_APP_CREATE    1
#define EVENT_APP_DESTROY   2
#define EVENT_APP_START     3
#define EVENT_APP_STOP      4
#define EVENT_APP_RESUME    5
#define EVENT_APP_PAUSE     6

#define EVENT_TOUCH_DOWN    1
#define EVENT_TOUCH_MOVE    2
#define EVENT_TOUCH_UP      3

#define EVENT_KEY_DOWN      1
#define EVENT_KEY_UP        2

#define MSG_FINISH_ACTIVITY 1
#define MSG_SHOW_KEYBOARD   2

void bcAndroidAcquireSurface(int id, ANativeWindow *window, int format, int width, int height);
void bcAndroidReleaseSurface(int id);
void bcAndroidAppChengeState(int state);
void bcAndroidTouchEvent(int event, int id, float x, float y);
void bcAndroidKeyEvent(int event, int key, int code);

void bcAndroidSetCallback(void (*callback)(int type, int x, int y));
void bcAndroidSendMessage(int type, int x, int y);
