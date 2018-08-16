#include "bcgl_internal.h"

static BCMesh *s_Mesh = NULL;

static BCMesh * s_TempMesh = NULL;
static float s_TempVertexData[MESH_BUFFER_MAX][4];

void bcInitGfxDraw()
{
    s_Mesh = bcCreateMesh(512, 512, MESH_FLAGS_POS3 | MESH_FLAGS_TEX2 | MESH_FLAGS_NORM | MESH_FLAGS_COL4, true);
}

void bcTermGfxDraw()
{
    bcDestroyMesh(s_Mesh);
}

// IM

bool bcBegin(int type)
{
    return bcBeginMesh(s_Mesh);
}

void bcEnd()
{
    bcEndMesh(s_TempMesh);
}

bool bcBeginMesh(BCMesh *mesh)
{
    if (s_TempMesh != NULL)
    {
        bcLog("Mesh already locked!");
        return false;
    }
    if (mesh == NULL)
    {
        bcLog("Mesh not provided!");
        return false;
    }
    s_TempMesh = mesh;
    s_TempMesh->draw_count = 0;
    vec4(s_TempVertexData[MESH_BUFFER_POSITIONS], 0, 0, 0, 0);
    vec4(s_TempVertexData[MESH_BUFFER_NORMALS], 0, 0, 1, 0);
    vec4(s_TempVertexData[MESH_BUFFER_TEXCOORDS], 0, 0, 0, 0);
    vec4(s_TempVertexData[MESH_BUFFER_COLORS], 1, 1, 1, 1);
    return true;
}

void bcEndMesh()
{
    bcDrawMesh(s_TempMesh);
    s_TempMesh = NULL;
}

void bcVertex3f(float x, float y, float z)
{
    if (s_TempMesh == NULL)
    {
        bcLog("Mesh not locked!");
        return;
    }
    if (s_TempMesh->draw_count >= s_TempMesh->num_vertices)
    {
        bcLog("Mesh limit reached!");
        return;
    }
    vec4(s_TempVertexData[MESH_BUFFER_POSITIONS], x, y, z, 0);
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(s_TempMesh->buffers[i]);
        float * ptr = &(buff->vertices[buff->comps * s_TempMesh->draw_count]);
        memcpy(ptr, s_TempVertexData[i], buff->comps * sizeof(float));
    }
    s_TempMesh->indices[s_TempMesh->draw_count] = s_TempMesh->draw_count;
    s_TempMesh->draw_count++;
}

void bcVertex2f(float x, float y)
{
    bcVertex3f(x, y, 0);
}

void bcTexCoord2f(float u, float v)
{
    vec2(s_TempVertexData[MESH_BUFFER_TEXCOORDS], u, v);
}

void bcNormalf(float x, float y, float z)
{
    vec3(s_TempVertexData[MESH_BUFFER_NORMALS], x, y, z);
}

void bcColor4f(float r, float g, float b, float a)
{
    vec4(s_TempVertexData[MESH_BUFFER_COLORS], r, g, b, a);
}

void bcColor3f(float r, float g, float b)
{
    bcColor4f(r, g, b, 1.0f);
}

void bcColorHex(unsigned int argb)
{
}

// Camera

void bcPrepareScene3D(float fov)
{
    BCWindow *win = bcGetWindow();
    float aspect = (float) win->height / (float) win->width;
    float znear = 0.1f;
    float zfar = 10000.0f;
    bcSetPerspective(to_radians(fov), aspect, znear, zfar);
    bcIdentity();
    bcSetBlend(false);
    bcSetDepthTest(true);
    bcSetLighting(true);
}

void bcPrepareScene2D()
{
}

void bcPrepareSceneGUI()
{
    BCWindow *win = bcGetWindow();
    bcSetOrtho(0, win->width, win->height, 0, -1, 1);
    bcIdentity();
    bcSetBlend(true);
    bcSetDepthTest(false);
    bcSetLighting(false);
}

// Draw 2D
void bcDrawText2D(BCFont *font, float x, float y);
void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh);
void bcDrawRect2D(float x, float y, float w, float h);
void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z);
