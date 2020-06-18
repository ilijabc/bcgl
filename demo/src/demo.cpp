#include <bcgl.h>
#include <bcmath.h>

#include <vector>

static struct
{
    vec3_t pos;
    vec3_t rot;
    bool auto_rotate;
} camera;

static BCFont *font = NULL;

struct Object
{
    BCMesh *mesh;
    vec3_t pos;
    BCColor color;
};

static std::vector<Object> objs;

//
// BCGL interface
//

extern "C" void BC_onConfig(BCConfig *config)
{
    bcLogDebug("BC_onConfig");

    float aspect = (float) config->width / (float) config->height;
#ifndef __ANDROID__
    config->height = 480;
    config->width = aspect * config->height;
    config->vsync = false;
    config->msaa = 4;
#endif
    config->title = "BCGL Demo";
}

extern "C" void BC_onCreate()
{
    bcLogDebug("BC_onCreate");

    bcLogDebug("Build time: %s %s", __DATE__, __TIME__);

    camera =
    {
        { 0, 0, -6 },
        { -45, 0, 40},
        false
    };
}

extern "C" void BC_onDestroy()
{
    bcLogDebug("BC_onDestroy");
}

extern "C" void BC_onStart()
{
    bcLogDebug("BC_onStart");

    font = bcCreateFontTTF(ASSETS_DIR"vera.ttf", 30);

    objs.push_back(
    {
        bcCreateMeshBox(-1, -1, 0, 1, 1, 2),
        vec3(-2, 2, 0),
        BC_COLOR_RED
    });
    objs.push_back(
    {
        bcCreateCylinder(1, 0.5f, 16),
        vec3(4, 0, 0),
        BC_COLOR_GREEN
    });
    objs.push_back(
    {
        bcCreateMeshSphere(1, 12, 12),
        vec3(-1, -1, 0),
        BC_COLOR_BLUE
    });
}

extern "C" void BC_onStop()
{
    bcLogDebug("BC_onStop");

    bcDestroyFont(font);
    for (Object & obj : objs)
    {
        bcDestroyMesh(obj.mesh);
    }
    objs.clear();
}

extern "C" void BC_onUpdate(float dt)
{
    if (camera.auto_rotate)
    {
        camera.rot.z += dt * 50;
    }
}

extern "C" void BC_onDraw1()
{
    // scene 3D
    bcClear(BC_COLOR_GRAY);
    bcPrepareScene3D(60);

    // camera
    bcTranslatef(0, 0, camera.pos.z);
    bcRotatef(camera.rot.x, 1, 0, 0);
    bcRotatef(camera.rot.y, 0, 1, 0);
    bcRotatef(camera.rot.z, 0, 0, 1);
    bcTranslatef(camera.pos.x, camera.pos.y, 0);

    // grid
    bcSetColor(BC_COLOR_WHITE, BC_COLOR_TYPE_PRIMARY);
    bcPushMatrix();
    bcTranslatef(-5, -5, 0);
    bcDrawGrid(10, 10);
    bcPopMatrix();

    // scene
    for (const Object & obj : objs)
    {
        bcPushMatrix();
        bcTranslatef(obj.pos.x, obj.pos.y, obj.pos.z);
        bcSetColor(obj.color, BC_COLOR_TYPE_PRIMARY);
        bcDrawMesh(obj.mesh);
        bcPopMatrix();
    }

    bcPrepareSceneGUI();
    bcSetColor(BC_COLOR_YELLOW, BC_COLOR_TYPE_PRIMARY);
    bcDrawText(font, 10, 30, "Text is YELLOW");
    bcDrawText(font, 10, 60, "Box is RED");
    bcDrawText(font, 10, 90, "Cylinder is GREEN");
    bcDrawText(font, 10, 120, "Sphere is BLUE");
    bcDrawText(font, 10, 150, "Grid is WHITE");
}

extern "C" void BC_onEvent(BCEvent event)
{
    static bool wire = false;
    static bool do_rotate = false;
    static vec2i_t last = { 0, 0 };

    switch (event.type)
    {
    case BC_EVENT_KEY_RELEASE:
        switch (event.id)
        {
        case BC_KEY_ESCAPE:
            bcQuit(0);
            break;
        case BC_KEY_W:
            wire = !wire;
            bcSetWireframe(wire);
            break;
        case BC_KEY_R:
            camera.auto_rotate = !camera.auto_rotate;
            break;
        }
        break;
    case BC_EVENT_MOUSE_PRESS:
    case BC_EVENT_TOUCH_DOWN:
        if (event.id == 0)
        {
            do_rotate = true;
            last = { event.x, event.y };
        }
        break;
    case BC_EVENT_MOUSE_RELEASE:
    case BC_EVENT_TOUCH_UP:
        if (event.id == 0)
        {
            do_rotate = false;
        }
        break;
    case BC_EVENT_MOUSE_MOVE:
    case BC_EVENT_TOUCH_MOVE:
        if (do_rotate)
        {
            int dx = event.x - last.x;
            int dy = event.y - last.y;
            camera.rot.z += dx;
            camera.rot.x += dy;
            last = { event.x, event.y };
        }
        break;
    case BC_EVENT_MOUSE_WHEEL:
        camera.pos.z += event.y * 0.1f;
        break;
    }
}
