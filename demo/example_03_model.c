#include "demo.h"

static BCModel *mdl = NULL;

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
    BCMesh *msh = bcCreateMeshCube();
    BCMaterial mtl = {
        RGBA_COLOR(0xffffffff),
        RGBA_COLOR(0xffffffff),
        RGBA_COLOR(0x0000ffff),
        NULL //bcCreateTextureFromFile(ASSETS_DIR"cursor.png", 0)
    };
    mdl = bcCreateModel(msh, mtl, 0);
}

static void BC_onStop()
{
    bcDestroyModel(mdl);
}

static void BC_onUpdate(float dt)
{
    demo_prepare_scene(dt);

    bcDrawModel(mdl);

    demo_finish_scene();
}

static void BC_onEvent(BCEvent event)
{
    demo_event(event);
}

BCCallbacks example_03_model = {
    BC_onConfig,
    BC_onCreate,
    BC_onDestroy,
    BC_onStart,
    BC_onStop,
    BC_onUpdate,
    BC_onEvent,
};
