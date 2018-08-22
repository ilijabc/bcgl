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

static BCShader *exampleShader = NULL;
static BCTexture *texAlert = NULL;
static BCTexture *texGrass = NULL;
static struct
{
    vec3_t pos;
    vec3_t rot;
} camera = { 0 };
static BCMesh *meshCylinder = NULL;
// light
static struct
{
    BCMesh *mesh;
    vec3_t pos, stored_pos;
    bool followCamera;
} light = { 0 };

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

static void DrawLight()
{
    bcSetLighting(false);
    bcPushMatrix();
    bcTranslatef(light.pos.x, light.pos.y, light.pos.z);
    bcScalef(0.1f, 0.1f, 0.1f);
    bcDrawMesh(light.mesh);
    bcPopMatrix();
    bcSetLighting(true);
}

static void dumpMatrix(float *m)
{
    char s[100] = "\n";
    for (int i = 0; i < 16; i++)
    {
        sprintf(s, "%s %.2f", s, m[i]);
        if ((i + 1) % 4 == 0)
            sprintf(s, "%s\n", s);
    }
    bcLog("[%s ]", s);
}

static vec3_t getMatrixPosition(float *mvm, float x, float y, float z)
{
    float offset[4] =
    {
        -mvm[12] + x,
        -mvm[13] + y,
        -mvm[14] + z,
        1
    };
    float temp_m[16];
    mat4_assign(temp_m, mvm);
    mat4_transpose(temp_m, temp_m);
    vec4_multiply_mat4(offset, offset, temp_m);
    return svec3(offset[0], offset[1], offset[2]);
}

void BC_onConfig(BCConfig *config)
{
    config->width = 640;
    config->height = 480;
}

void BC_onStart()
{
    exampleShader = bcCreateShaderFromFile("data/default.glsl");
    bcBindShader(exampleShader);
    texAlert = bcCreateTextureFromFile("data/vpn-error.png", 0);
    texGrass = bcCreateTextureFromFile("data/grass.png", 0);
    camera.pos.z = -6;
    // par_shapes_mesh *shape = par_shapes_create_cube();
    par_shapes_mesh *shape1 = par_shapes_create_rock(100, 2);
    // par_shapes_mesh *shape2 = par_shapes_create_cylinder(100, 2);
    // par_shapes_merge(shape1, shape2);
    meshCylinder = bcCreateMeshFromShape(shape1);
    par_shapes_free_mesh(shape1);
    // par_shapes_free_mesh(shape2);
    // light
    par_shapes_mesh *sphere = par_shapes_create_parametric_sphere(10, 10);
    light.mesh = bcCreateMeshFromShape(sphere);
    par_shapes_free_mesh(sphere);
    light.pos = svec3(0, 0, 1);
}

void BC_onStop()
{
    bcDestroyTexture(texAlert);
    bcDestroyShader(exampleShader);
}

void BC_onUpdate(float dt)
{
    //
    // Game logic
    //
    if (bcIsMouseDown(0))
    {
        camera.rot.z += bcGetMouseDeltaX();
        camera.rot.x += bcGetMouseDeltaY();
    }
    camera.pos.z += bcGetMouseWheel() * 0.1f;
    vec3_t *pobj = light.followCamera ? &(camera.pos) : &(light.pos);
    if (pobj)
    {
        const float lspeed = 5;
        if (bcIsKeyDown(BC_KEY_D)) pobj->x += dt * lspeed;
        if (bcIsKeyDown(BC_KEY_A)) pobj->x -= dt * lspeed;
        if (bcIsKeyDown(BC_KEY_W)) pobj->y += dt * lspeed;
        if (bcIsKeyDown(BC_KEY_S)) pobj->y -= dt * lspeed;
        if (bcIsKeyDown(BC_KEY_Q)) pobj->z += dt * lspeed;
        if (bcIsKeyDown(BC_KEY_E)) pobj->z -= dt * lspeed;
    }
    //
    // Game graphics
    //
    bcClear();
    // game
    bcLightPosition(light.pos.x, light.pos.y, light.pos.z);
    bcPrepareScene3D(60);
    // camera
    bcTranslatef(camera.pos.x, camera.pos.y, camera.pos.z);
    bcRotatef(camera.rot.x, 1, 0, 0);
    bcRotatef(camera.rot.y, 0, 1, 0);
    bcRotatef(camera.rot.z, 0, 0, 1);
    if (light.followCamera)
    {
        light.pos = getMatrixPosition(bcGetMatrix(), 0, 0, 0);
    }
    else
    {
        DrawLight();
    }
    // scene
    bcPushMatrix();
    bcTranslatef(-1, -1, 0);
    DrawTexture(texGrass, 2, 2);
    bcPopMatrix();
    bcDrawMesh(meshCylinder);
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
        case BC_KEY_F1:
            wire = !wire;
            bcSetWireframe(wire);
            break;
        case BC_KEY_F2:
            light.followCamera = !light.followCamera;
            if (light.followCamera)
                light.stored_pos = light.pos;
            else
                light.pos = light.stored_pos;
            break;
        }
    }
}
