#include "demo.h"
#include <par/par_shapes.h>

typedef struct
{
    BCMesh *mesh;
    BCColor color;
    vec3_t pos;
} SceneObject;

#define NUM_OBJECTS 15
static SceneObject s_Objects[NUM_OBJECTS];

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
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        SceneObject *obj = &s_Objects[i];
        par_shapes_mesh *shape = par_shapes_create_rock(randomi(100), 2);
        obj->mesh = bcCreateMeshFromShape(shape);
        bcTransformMesh(obj->mesh, mat4_rotation_x(to_radians(90)).v);
        bcUploadMesh(obj->mesh, VBO_STATIC);
        par_shapes_free_mesh(shape);
        obj->color.r = randomf();
        obj->color.g = randomf();
        obj->color.b = randomf();
        obj->color.a = 1;
        obj->pos.x = randomf() * 10 - 5;
        obj->pos.y = randomf() * 10 - 5;
    }
}

static void BC_onStop()
{
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        SceneObject *obj = &s_Objects[i];
        bcDestroyMesh(obj->mesh);
    }
}

static void BC_onUpdate(float dt)
{
    demo_prepare_scene(0);

    static vec3_t pos = { 0, 0, 0 };
    static float th = 0;
    th -= dt * 0.5f;
    pos.x = sinf(th) * 10;
    pos.y = cosf(th) * 10;
    bcLightPosition(pos.x, pos.y, pos.z);

    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        SceneObject *obj = &s_Objects[i];
        bcPushMatrix();
        bcTranslatef(obj->pos.x, obj->pos.y, obj->pos.z);
        bcSetObjectColor(obj->color);
        bcDrawMesh(obj->mesh);
        bcPopMatrix();
    }

    demo_finish_scene();
}

static void BC_onEvent(BCEvent event)
{
    demo_event(event);
}

BCCallbacks example_05_lighting = {
    BC_onConfig,
    BC_onCreate,
    BC_onDestroy,
    BC_onStart,
    BC_onStop,
    BC_onUpdate,
    BC_onEvent,
};
