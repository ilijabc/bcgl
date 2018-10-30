#include "demo.h"

static BCMesh *s_Mesh = NULL;

static void BC_onConfig(BCConfig *config)
{
    demo_config(config);
}

static void BC_onCreate()
{
}

static void BC_onDestroy()
{
}

static void BC_onStart()
{
    s_Mesh = bcUploadMesh(bcCreateMeshCube(), VBO_STATIC);
}

static void BC_onStop()
{
    bcDestroyMesh(s_Mesh);
}

static void BC_onUpdate(float dt)
{
    demo_prepare_scene(dt);

    const float zoom = 0.3f;
    for (int x = -10; x < 10; x++)
    for (int y = -10; y < 10; y++)
    for (int z = -10; z < 10; z++)
    {
        bcPushMatrix();
        bcScalef(zoom, zoom, zoom);
        bcTranslatef(x, y, z);
        bcScalef(0.5f, 0.5f, 0.5f);
        bcDrawMesh(s_Mesh);
        bcPopMatrix();
    }

    demo_finish_scene();
}

static void BC_onEvent(BCEvent event)
{
    demo_event(event);
}

BCCallbacks example_04_performance = {
    BC_onConfig,
    BC_onCreate,
    BC_onDestroy,
    BC_onStart,
    BC_onStop,
    BC_onUpdate,
    BC_onEvent,
};
