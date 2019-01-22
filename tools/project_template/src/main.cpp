#include <bcgl.h>
#include <bcmath.h>

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

//
// BCGL interface
//

static void BC_onConfig(BCConfig *config)
{
    float aspect = (float) config->width / (float) config->height;
#ifndef __ANDROID__
    config->height = 480;
    config->width = aspect * config->height;
    config->vsync = false;
    config->msaa = 4;
#endif
    config->title = "{{bcapp:title}}";
}

static void BC_onCreate()
{
}

static void BC_onDestroy()
{
}

static void BC_onStart()
{
}

static void BC_onStop()
{
}

static void BC_onUpdate(float dt)
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

    // scene
    bcSetObjectColor(COLOR_BLUE);
    bcDrawCube(0, 0, 0.5f, 1, 1, 1);
}

static void BC_onEvent(BCEvent event)
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
        case BC_KEY_R:
            camera.auto_rotate = !camera.auto_rotate;
            break;
        }
    }
}

extern "C" void BC_main()
{
    BCCallbacks callbacks = {
        BC_onConfig,
        BC_onCreate,
        BC_onDestroy,
        BC_onStart,
        BC_onStop,
        BC_onUpdate,
        BC_onEvent,
    };
    bcInit(callbacks);
}
