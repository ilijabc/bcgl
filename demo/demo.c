#include "demo.h"

void demo_config(BCConfig *config)
{
    float aspect = (float) config->width / (float) config->height;
#ifndef __ANDROID__
    config->height = 480;
    config->width = aspect * config->height;
    config->vsync = false;
#endif
    config->msaa = 4;
    config->title = "BCGL Demo";
}

void demo_prepare_scene(float dt)
{
    static float r = 0;
    r += dt * 50;
    bcClear();
    bcPrepareScene3D(60);
    bcSetObjectColorf(1, 1, 1, 1);
    // camera
    bcTranslatef(0, 0, -10);
    bcRotatef(-45, 1, 0, 0);
    bcRotatef(r, 0, 0, 1);
    // grid
    bcPushMatrix();
    bcTranslatef(-5, -5, 0);
    bcDrawGrid(10, 10);
    bcPopMatrix();
}

void demo_finish_scene()
{
}

void demo_event(BCEvent event)
{
    static bool wire = false;
    if (event.type == BC_EVENT_KEYRELEASE)
    {
        switch (event.id)
        {
        case BC_KEY_ESCAPE:
            bcQuit(0);
            break;
        case BC_KEY_W:
            wire = !wire;
            bcSetWireframe(wire);
            break;
        }
    }
}
