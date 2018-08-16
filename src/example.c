#include <stdio.h>

#include "bcgl.h"

#include <mathc/mathc.h>
#include <par/par_shapes.h>

typedef struct vec2 vec2_t;
typedef struct vec3 vec3_t;
typedef struct vec4 vec4_t;
typedef struct quat quat_t;
typedef struct mat2 mat2_t;
typedef struct mat3 mat3_t;
typedef struct mat4 mat4_t;

static BCTexture *texAlert = NULL;
static BCTexture *texGrass = NULL;
static struct
{
    float x, y, z;
    float rx, ry, rz;
} camera = { 0 };
static BCMesh *meshCylinder = NULL;


static void DrawTexture(BCTexture *texture, float w, float h)
{
    bcBindTexture(texture);
    bcBegin(0);
    // bcColor4f(0.5f, 1, 1, 0.5f);
    bcTexCoord2f(0, 0);
    bcVertex2f(0, 0);
    bcTexCoord2f(1, 0);
    bcVertex2f(w, 0);
    bcTexCoord2f(1, 1);
    bcVertex2f(w, h);
    // bcColor4f(1, 0.5f, 1, 0.5f);
    bcTexCoord2f(1, 1);
    bcVertex2f(w, h);
    bcTexCoord2f(0, 1);
    bcVertex2f(0, h);
    bcTexCoord2f(0, 0);
    bcVertex2f(0, 0);
    bcEnd();
    bcBindTexture(NULL);
}

void BC_onConfig(BCConfig *config)
{
    config->width = 640;
    config->height = 480;
}

void BC_onStart()
{
    texAlert = bcCreateTextureFromFile("data/vpn-error.png", 0);
    texGrass = bcCreateTextureFromFile("data/grass.png", 0);
    camera.z = -2;
    // par_shapes_mesh *shape = par_shapes_create_cube();
    par_shapes_mesh *shape1 = par_shapes_create_cylinder(100, 2);
    par_shapes_mesh *shape2 = par_shapes_create_rock(100, 2);
    par_shapes_merge(shape1, shape2);
    meshCylinder = bcCreateMeshFromShape(shape1);
    par_shapes_free_mesh(shape1);
    par_shapes_free_mesh(shape2);
}

void BC_onStop()
{
    bcDestroyTexture(texAlert);
}

void BC_onUpdate(float dt)
{
    // Game logic
    if (bcIsMouseDown(0))
    {
        camera.rz += bcGetMouseDeltaX();
        camera.rx += bcGetMouseDeltaY();
    }
    camera.z += bcGetMouseWheel() * 0.1f;

    // Game graphics
    bcClear();
    // game
    bcPrepareScene3D(60);
    bcTranslatef(camera.x, camera.y, camera.z);
    bcRotatef(camera.rx, 1, 0, 0);
    bcRotatef(camera.ry, 0, 1, 0);
    bcRotatef(camera.rz, 0, 0, 1);
    bcPushMatrix();
    bcTranslatef(-1, -1, 0);
    DrawTexture(texGrass, 2, 2);
    bcPopMatrix();
    // bcSetWireframe(true);
    bcDrawMesh(meshCylinder);
    // bcSetWireframe(false);
    // gui
    BCWindow *win = bcGetWindow();
    bcPrepareSceneGUI();
    DrawTexture(texAlert, texAlert->width / 2, texAlert->height / 2);
}

static const char *s_EventNames[] = {
    "BC_EVENT_KEYPRESS",
    "BC_EVENT_KEYRELEASE",
    "BC_EVENT_MOUSEPRESS",
    "BC_EVENT_MOUSERELEASE",
    "BC_EVENT_MOUSEMOVE",
    "BC_EVENT_MOUSEWHEEL",
    "BC_EVENT_WINDOWSIZE",
    "BC_EVENT_WINDOWFOCUS",
    "BC_EVENT_WINDOWICONIFY",
};

void BC_onEvent(int event, int x, int y)
{
    static bool wire = false;
    // bcLog("%s: [ %d, %d ]", s_EventNames[event], x, y);
    if (event == BC_EVENT_KEYPRESS)
    {
        switch (x)
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
