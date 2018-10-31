#include "demo.h"

static struct
{
    vec3_t pos;
    vec3_t rot;
    bool auto_rotate;
} camera =
{
    { 0, 0, -6 },
    { -60, 0, 0},
    false
};

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
    // camera control
    if (bcIsMouseDown(0))
    {
        camera.rot.z += bcGetMouseDeltaX();
        camera.rot.x += bcGetMouseDeltaY();
    }
    camera.pos.z += bcGetMouseWheel() * 0.1f;
    if (camera.auto_rotate)
    {
        camera.rot.z += dt * 50;
    }

    // scene 3D
    bcClear();
    bcPrepareScene3D(60);
    bcSetObjectColorf(1, 1, 1, 1);

    // camera
    bcTranslatef(0, 0, camera.pos.z);
    bcRotatef(camera.rot.x, 1, 0, 0);
    bcRotatef(camera.rot.y, 0, 1, 0);
    bcRotatef(camera.rot.z, 0, 0, 1);
    bcTranslatef(camera.pos.x, camera.pos.y, 0);

    // grid
    bcPushMatrix();
    bcTranslatef(-5, -5, 0);
    bcDrawGrid(10, 10);
    bcPopMatrix();
}

void demo_finish_scene()
{
}

bool demo_event(BCEvent event)
{
    static bool wire = false;
    if (event.type == BC_EVENT_KEYRELEASE)
    {
        switch (event.id)
        {
        case BC_KEY_ESCAPE:
            bcQuit(0);
            return true;
        case BC_KEY_SPACE:
            bcQuit(1);
            return true;
        case BC_KEY_W:
            wire = !wire;
            bcSetWireframe(wire);
            return true;
        case BC_KEY_R:
            camera.auto_rotate = !camera.auto_rotate;
            return true;
        }
    }
    return false;
}
