#include "demo.h"

static BCMesh *mesh = NULL;
static BCMeshPart p1, p2, p3;

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
    // merge test
    mesh = bcCreateMesh(0, 0, MESH_FLAGS_POS3 | MESH_FLAGS_NORM | MESH_FLAGS_TEX2);
    BCMesh *m1 = bcCreateMeshBox(0, 0, 0, 1, 1, 4);
    BCMesh *m2 = bcCreateMeshBox(0, 2, 0, 3, 3, 0.5f);
    BCMesh *m3 = bcCreateCylinder(0.5f, 0.1f, 12);
    // mesh = m1;
    p1 = bcAttachMesh(mesh, m1, true);
    // p1 = bcPartFromMesh(mesh);
    p2 = bcAttachMesh(mesh, m2, true);
    p3 = bcAttachMesh(mesh, m3, true);
    bcUploadMesh(mesh, VBO_STATIC);
}

static void BC_onStop()
{
    bcDestroyMesh(mesh);
}

static void BC_onUpdate(float dt)
{
    demo_prepare_scene(dt);

    bcPushMatrix();
    bcTranslatef(0, 0, 3);
    bcDrawMesh(mesh);
    bcPopMatrix();
    bcPushMatrix();
    bcTranslatef(4, 0, 0);
    bcDrawMeshPart(p1);
    bcTranslatef(4, 0, 0);
    bcDrawMeshPart(p2);
    bcTranslatef(-8, 4, 0);
    bcDrawMeshPart(p3);
    bcPopMatrix();

    demo_finish_scene();
}

static void BC_onEvent(BCEvent event)
{
    demo_event(event);
}

BCCallbacks example_02_mesh = {
    BC_onConfig,
    BC_onCreate,
    BC_onDestroy,
    BC_onStart,
    BC_onStop,
    BC_onUpdate,
    BC_onEvent,
};
