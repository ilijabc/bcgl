#include <stdio.h>

#include "bcgl.h"

#include <mathc/mathc.h>

typedef struct vec2 vec2_t;
typedef struct vec3 vec3_t;
typedef struct vec4 vec4_t;
typedef struct quat quat_t;
typedef struct mat2 mat2_t;
typedef struct mat3 mat3_t;
typedef struct mat4 mat4_t;

static BCTexture *gameTexture = NULL;


void BC_onConfig(BCConfig *config)
{
    config->width = 640;
    config->height = 480;
}

void BC_onStart()
{
    bcLog("~~~");
    gameTexture = bcCreateTextureFromFile("data/vpn-error.png", 0);
}

void BC_onStop()
{
    bcLog("~~~");
    bcDestroyTexture(gameTexture);
}

static void DrawTexture(BCTexture *texture)
{
    float w = texture->width/2;
    float h = texture->height/2;
    bcBindTexture(texture);
    bcBegin(0);
    // bcColor4f(0.5f, 1, 1, 0.5f);
    bcTexCoord2f(0, 0);
    bcVertex2f(0, 0);
    bcTexCoord2f(1, 0);
    bcVertex2f(w, 0);
    bcTexCoord2f(1, 1);
    bcVertex2f(w, h);
    // bcColor4f(1, 0.5f, 1, 0.5f);
    bcTexCoord2f(1, 1);
    bcVertex2f(w, h);
    bcTexCoord2f(0, 1);
    bcVertex2f(0, h);
    bcTexCoord2f(0, 0);
    bcVertex2f(0, 0);
    bcEnd();
}

void BC_onUpdate(float dt)
{
    // Game logic
    static float r = 0;
    static float s = 1;
    float x = bcGetMouseX();
    float y = bcGetMouseY();
    if (bcIsMouseDown(1))
        r += dt*100;
    else if (bcIsMouseDown(0))
        r -= dt*100;
    s += bcGetMouseWheel() * 0.01f;
    if (bcIsKeyDown(BC_KEY_SPACE))
        r += dt*1000;
    // Game graphics
    bcClear();
    bcPrepareSceneGUI();
    float points[] = {
        0,0,0,
        1,0,0,
        1,1,0
    };
    
    bcTranslatef(x, y, 0);
    bcRotatef(r, 0, 0, 1);
    bcPushMatrix();
    bcTranslatef(-gameTexture->width/2*s, -gameTexture->height/2*s, 0);
    bcScalef(s, s, 1);
    DrawTexture(gameTexture);
    bcPopMatrix();
    DrawTexture(gameTexture);

}

static const char *s_EventNames[] = {
    "BC_EVENT_KEYPRESS",
    "BC_EVENT_KEYRELEASE",
    "BC_EVENT_MOUSEPRESS",
    "BC_EVENT_MOUSERELEASE",
    "BC_EVENT_MOUSEMOVE",
    "BC_EVENT_MOUSEWHEEL",
    "BC_EVENT_WINDOWSIZE",
    "BC_EVENT_WINDOWFOCUS",
    "BC_EVENT_WINDOWICONIFY",
};

void BC_onEvent(int event, int x, int y)
{
    // bcLog("%s: [ %d, %d ]", s_EventNames[event], x, y);
    if (event == BC_EVENT_KEYPRESS)
    {
        if (x == BC_KEY_ESCAPE)
            bcQuit(0);
    }
}
