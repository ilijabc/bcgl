#include "demo.h"

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
}

static void BC_onStop()
{
}

static void BC_onUpdate(float dt)
{
    demo_prepare_scene(dt);

    bcSetObjectColor(COLOR_BLUE);
    bcDrawCube(0, 0, 0.5f, 1, 1, 1);

    demo_finish_scene();
}

static void BC_onEvent(BCEvent event)
{
    demo_event(event);
}

BCCallbacks example_01_box = {
    BC_onConfig,
    BC_onCreate,
    BC_onDestroy,
    BC_onStart,
    BC_onStop,
    BC_onUpdate,
    BC_onEvent,
};
