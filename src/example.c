#include <stdio.h>

#include "bcgl.h"
#include <mathc/mathc.h>

static struct
{
    BCImage *image;
    BCTexture *texPieces;
    struct vec2 pos;
    float rot;
} app;

void BC_onConfig(BCConfig *config)
{
    config->width = 640;
    config->height = 480;
}

void BC_onStart()
{
    bcLog("~~~");
    app.image = bcCreateImageFromFile("data/vpn-error.png");
    bcLog("image: { w=%d h=%d c=%d }", app.image->width, app.image->height, app.image->comps);
    app.texPieces = bcCreateTextureFromImage(app.image, 0);
    app.pos = svec2(10, 10);
}

void BC_onStop()
{
    bcLog("~~~");
    bcDestroyImage(app.image);
    bcDestroyTexture(app.texPieces);
}

void BC_onUpdate(float dt)
{
    bcClear();
    bcPrepareSceneGUI();
    bcTranslatef(app.pos.x, app.pos.y, 0);
    bcDrawTexture(app.texPieces);
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
    bcLog("%s: [ %d, %d ]", s_EventNames[event], x, y);
    if (event == BC_EVENT_KEYPRESS)
    {
        if (x == BC_KEY_ESCAPE)
            bcQuit(0);
    }
    else if (event == BC_EVENT_MOUSEMOVE)
    {
        app.pos = svec2(x, y);
    }
}
