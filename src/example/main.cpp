#include <bcgl.h>
#include <bcmath.h>

static struct
{
    vec3_t pos;
    vec3_t rot;
} camera =
{
    { 0, 0, -6 },
    { -60, 0, 0},
};

BCColor cubeColor = BC_COLOR_BLUE;

//
// BCGL interface
//

extern "C" void BC_onConfig(BCConfig *config)
{
    float aspect = (float) config->width / (float) config->height;
#ifndef __ANDROID__
    config->height = 480;
    config->width = aspect * config->height;
    config->vsync = false;
    config->msaa = 4;
#endif
    config->title = "__bcgl_project_title__";
}

extern "C" void BC_onCreate()
{
}

extern "C" void BC_onDestroy()
{
}

extern "C" void BC_onStart()
{
}

extern "C" void BC_onStop()
{
}

extern "C" void BC_onUpdate(float dt)
{
    // camera control
    if (bcIsMouseDown(0))
    {
        camera.rot.z += bcGetMouseDeltaX();
        camera.rot.x += bcGetMouseDeltaY();
    }
    else
    {
        camera.rot.z += dt * 10;
    }
    camera.pos.z += bcGetMouseWheel() * 0.1f;
}

extern "C" void BC_onDraw()
{
    // scene 3D
    bcClear(BC_COLOR_GRAY);
    bcPrepareScene3D(60);
    bcSetColor(BC_COLOR_WHITE, BC_COLOR_TYPE_PRIMARY);

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
    bcSetColor(cubeColor, BC_COLOR_TYPE_PRIMARY);
    bcDrawCube(-1, -1, 0, 2, 2, 2, true);
}

extern "C" void BC_onEvent(BCEvent event)
{
    static bool wireframe = false;
    static bool lighting = true;
    if (event.type == BC_EVENT_KEY_RELEASE)
    {
        switch (event.id)
        {
        case BC_KEY_ESCAPE:
            bcQuit(0);
            break;
        case BC_KEY_W:
            wireframe = !wireframe;
            bcSetWireframe(wireframe);
            break;
        case BC_KEY_ENTER:
        case BC_KEY_SPACE:
            cubeColor.r = randomf();
            cubeColor.g = randomf();
            cubeColor.b = randomf();
            break;
        }
    }
}
