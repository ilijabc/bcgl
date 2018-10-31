#include "demo.h"
#include <par/par_shapes.h>

typedef struct
{
    BCMesh *mesh;
    BCColor color;
    vec3_t pos;
    float rot;
    float scale;
} GameObject;

#define MAX_OBJECTS 36
static GameObject *objects[MAX_OBJECTS] = { NULL };
static GameObject *player = NULL;

static const BCColor ColorWhite = {1,1,1,1};
static const BCColor ColorRed = {1,0,0,1};
static const BCColor ColorGreen = {0,1,0,1};
static const BCColor ColorBlue = {0,0,1,1};

// static BCShader *defaultShader = NULL;
static BCTexture *texGrass = NULL;
static BCTexture *texCursor = NULL;
static struct
{
    vec3_t pos;
    vec3_t rot;
} camera =
{
    { 0, 0, -6 },
    { -60, 0, 0}
};
// light
static struct
{
    BCMesh *mesh;
    vec3_t pos, stored_pos;
    bool followCamera;
    bool shader;
} light = { 0 };

static struct
{
    int counter;
    float stored_time;
} fpsCounter = { 0 };

static BCFont *myFont = NULL;

GameObject * createGameObject(float x, float y, const char *type)
{
    GameObject *obj = NEW_OBJECT(GameObject);
    par_shapes_mesh *objShape;
    if (strcmp(type, "player") == 0)
    {
        objShape = par_shapes_create_rock(10,2);
        par_shapes_mesh *s1 = par_shapes_create_parametric_sphere(10, 10);
        par_shapes_translate(s1, 0, 0, 1);
        par_shapes_merge(objShape, s1);
        par_shapes_free_mesh(s1);
        par_shapes_compute_normals(objShape);
    }
    else if (strcmp(type, "rock") == 0)
    {
        objShape = par_shapes_create_rock(randomi(100), 2);
    }
    else if (strcmp(type, "box") == 0)
    {
        objShape = par_shapes_create_cube();
    }
    else
    {
        objShape = par_shapes_create_parametric_sphere(10, 10);
    }
    obj->mesh = bcCreateMeshFromShape(objShape);
    obj->color.r = 1; //bcGetRandom();
    obj->color.g = 1; //bcGetRandom();
    obj->color.b = 1; //bcGetRandom();
    obj->color.a = 1;
    obj->pos = vec3(x, y, 0);
    // obj->rot = bcGetRandom() * 360;
    obj->scale = 1;// 0.1 + bcGetRandom() * 2;
    par_shapes_free_mesh(objShape);
    return obj;
}

void drawGameObject(mat4_t cm, GameObject *obj)
{
    // bcPushMatrix();
    // bcTranslatef(obj->pos.x, obj->pos.y, obj->pos.z);
    // bcRotatef(obj->rot, 0, 0, 1);
    // bcScalef(obj->scale, obj->scale, obj->scale);
    cm = mat4_translate(cm, obj->pos.x, obj->pos.y, obj->pos.z);
    cm = mat4_rotate_z(cm, obj->rot);
    cm = mat4_scale(cm, obj->scale, obj->scale, obj->scale);
    bcSetModelViewMatrix(cm.v);
    bcSetObjectColor(obj->color);
    bcDrawMesh(obj->mesh);
    // bcPopMatrix();
}

void destroyGameObject(GameObject *obj)
{
    bcDestroyMesh(obj->mesh);
    free(obj);
}

static void DrawTiles(BCTexture *texture, float w, float h, float tx, float ty)
{
    float ww = w / 2;
    float hh = h / 2;
    bcBindTexture(texture);
    bcBegin(BC_TRIANGLES);
    // bcColor4f(0.5f, 1, 1, 0.5f);
    bcTexCoord2f(0, 0);
    bcVertex2f(-ww, -hh);
    bcTexCoord2f(tx, 0);
    bcVertex2f(ww, -hh);
    bcTexCoord2f(tx, ty);
    bcVertex2f(ww, hh);
    // bcColor4f(1, 0.5f, 1, 0.5f);
    bcTexCoord2f(tx, ty);
    bcVertex2f(ww, hh);
    bcTexCoord2f(0, ty);
    bcVertex2f(-ww, hh);
    bcTexCoord2f(0, 0);
    bcVertex2f(-ww, -hh);
    bcEnd();
    bcBindTexture(NULL);
}

static void DrawLight(mat4_t cm)
{
    bcSetLighting(false);
    // bcPushMatrix();
    // bcTranslatef(light.pos.x, light.pos.y, light.pos.z);
    // bcScalef(0.1f, 0.1f, 0.1f);
    cm = mat4_translate(cm, light.pos.x, light.pos.y, light.pos.z);
    cm = mat4_scale(cm, 0.1f, 0.1f, 0.1f);
    bcSetModelViewMatrix(cm.v);
    bcDrawMesh(light.mesh);
    // bcPopMatrix();
    bcSetLighting(true);
}

static void BC_onConfig(BCConfig *config)
{
    bcLog("screen: [%d %d]", config->width, config->height);
    float aspect = (float) config->width / (float) config->height;
// #ifndef __ANDROID__
    config->height = 480;
    config->width = aspect * config->height;
    // config->vsync = true;
// #endif
}

static void BC_onCreate()
{
    bcLog("");
}

static void BC_onDestroy()
{
    bcLog("");
}

static void BC_onStart()
{
    bcLog("");
    // defaultShader = bcCreateShaderFromFile(PREFIX"assets/default.glsl");
    // bcBindShader(defaultShader);
    texGrass = bcCreateTextureFromFile("assets/grass.png", 0);
    texCursor = bcCreateTextureFromFile("assets/cursor.png", 0);
    camera.pos.z = -6;
    // light
    par_shapes_mesh *sphere = par_shapes_create_parametric_sphere(10, 10);
    light.mesh = bcCreateMeshFromShape(sphere);
    par_shapes_free_mesh(sphere);
    light.pos = vec3(0, 0, 1);
    light.followCamera = true;
    // font
    myFont = bcCreateFontTTF("assets/vera.ttf", 20);
    // init objects
    objects[0] = player = createGameObject(0, 0, "player");
    player->pos.z = 1;
    for (int i = 1; i < MAX_OBJECTS; i++)
    {
        objects[i] = createGameObject((i/6)*2+4-4, (i%6)*2+4-4, "ball");
    }
}

static void BC_onStop()
{
    bcLog("");
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        destroyGameObject(objects[i]);
    }
    bcDestroyFont(myFont);
    // bcDestroyShader(defaultShader);
}

static void BC_onUpdate(float dt)
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
    // camera.rot.z += dt * 60;
    //
    // Game graphics
    //
    bcClear();
    // game
    bcPrepareScene3D(60);
    bcSetObjectColor(ColorWhite);
    // bcSetLighting(true);
    // camera
#if 0
    bcTranslatef(0, 0, camera.pos.z);
    bcRotatef(camera.rot.x, 1, 0, 0);
    bcRotatef(camera.rot.y, 0, 1, 0);
    bcRotatef(camera.rot.z, 0, 0, 1);
    bcTranslatef(camera.pos.x, camera.pos.y, 0);
#else
    static mat4_t cm;
    cm = mat4_translation(0, 0, camera.pos.z);
    cm = mat4_rotate_x(cm, to_radians(camera.rot.x));
    cm = mat4_rotate_y(cm, to_radians(camera.rot.y));
    cm = mat4_rotate_z(cm, to_radians(camera.rot.z));
    cm = mat4_translate(cm, camera.pos.x, camera.pos.y, 0);
    bcSetModelViewMatrix(cm.v);
#endif
    if (!light.followCamera)
    {
        vec4_t pos = vec4_multiply_mat4(cm, vec4_from_vec3(light.pos, 1));
        bcLightPosition(pos.x, pos.y, pos.z);
        // DrawLight(cm);
    }
    else
    {
        bcLightPosition(light.pos.x, light.pos.y, light.pos.z);
    }
    // scene
    bcSetModelViewMatrix(cm.v);
    DrawTiles(texGrass, 20, 20, 20, 20);
    bcBindTexture(texGrass);
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        drawGameObject(cm, objects[i]);
    }
    bcBindTexture(NULL);
    // gui
    bcPrepareSceneGUI();
    bcSetObjectColor(ColorWhite);
    if (texCursor)
        bcDrawTexture2D(texCursor, bcGetMouseX(), bcGetMouseY(), texCursor->width, texCursor->height, 0, 0, 1, 1);
    // fps
    static char s_fps[50] = "BCGL";
    if (bcGetTime() > fpsCounter.stored_time + 1)
    {
        sprintf(s_fps, "FPS:%d", fpsCounter.counter);
        fpsCounter.counter = 0;
        fpsCounter.stored_time = bcGetTime();
    }
    fpsCounter.counter++;
    bcDrawText(myFont, 30, 30, s_fps);
    bcSetObjectColor(ColorRed);
    bcDrawText(myFont, 30, 60, "Red Text");
    bcSetObjectColor(ColorGreen);
    bcDrawText(myFont, 30, 90, "Green Text");
    bcSetObjectColor(ColorBlue);
    bcDrawText(myFont, 30, 120, "Blue Text");
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

static void BC_onEvent(BCEvent event)
{
    if (demo_event(event))
        return;
    if (event.type == BC_EVENT_KEYPRESS)
    {
        switch (event.id)
        {
        case BC_KEY_1:
            bcShowKeyboard(true);
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

BCCallbacks example_game = {
    BC_onConfig,
    BC_onCreate,
    BC_onDestroy,
    BC_onStart,
    BC_onStop,
    BC_onUpdate,
    BC_onEvent,
};
