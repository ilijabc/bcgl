#include <bcgl.h>

static void BC_onConfig(BCConfig *config)
{
    bcLog("screen: [%d %d]", config->width, config->height);
    float aspect = (float) config->width / (float) config->height;
#ifndef __ANDROID__
    config->height = 480;
    config->width = aspect * config->height;
    config->vsync = false;
#endif
    config->msaa = 4;
    config->title = "Angry Pixelbots 2";
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
    static float r = 0;
    r += dt * 50;
    bcClear();
    // scene
    bcPrepareScene3D(60);
    bcSetObjectColorf(1, 1, 1, 1);
    bcPushMatrix();
    bcTranslatef(0, 0, -10);
    bcRotatef(-45, 1, 0, 0);
    bcRotatef(r, 0, 0, 1);
    bcTranslatef(-5, -5, 0);
    bcDrawCube(5, 5, 0.5f, 1, 1, 1);
    bcDrawGrid(10, 10);
    bcPopMatrix();
    // gui
    bcPrepareSceneGUI();
    bcSetObjectColorf(1, 1, 1, 1);
}

static void BC_onEvent(BCEvent event)
{
    if (event.type == BC_EVENT_KEYPRESS && event.id == BC_KEY_ESCAPE)
    {
        bcQuit(0);
    }
}

BCCallbacks get_example_box()
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
    return callbacks;
}
