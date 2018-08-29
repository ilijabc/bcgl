#include "bcgl_internal.h"

#define MATRIX_STACK_SIZE 32

static BCMesh *s_Mesh = NULL;

static BCMesh * s_TempMesh = NULL;
static vec4_t s_TempVertexData[VERTEX_ATTR_MAX];

static mat4_t s_ProjectionMatrix;
static mat4_t s_MatrixStack[MATRIX_STACK_SIZE];
static int s_CurrentMatrix = 0;

void bcInitGfxDraw()
{
    s_Mesh = bcCreateMesh(1024, 1024, MESH_FLAGS_POS3 | MESH_FLAGS_NORM | MESH_FLAGS_TEX2 | MESH_FLAGS_COL4);
    s_ProjectionMatrix = mat4_identity();
    s_MatrixStack[0] = mat4_identity();
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
    s_TempVertexData[VERTEX_ATTR_POSITIONS] = vec4(0, 0, 0, 0);
    s_TempVertexData[VERTEX_ATTR_NORMALS] = vec4(0, 0, 1, 0);
    s_TempVertexData[VERTEX_ATTR_TEXCOORDS] = vec4(0, 0, 0, 0);
    s_TempVertexData[VERTEX_ATTR_COLORS] = vec4(1, 1, 1, 1);
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
    s_TempVertexData[VERTEX_ATTR_POSITIONS] = vec4(x, y, z, 0);
    float *vert_ptr = &(s_TempMesh->vertices[s_TempMesh->draw_count * s_TempMesh->total_comps]);
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        memcpy(vert_ptr, s_TempVertexData[i].v, s_TempMesh->total_comps * sizeof(float));
        vert_ptr += s_TempMesh->comps[i];
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
    s_TempVertexData[VERTEX_ATTR_TEXCOORDS] = vec4(u, v, 0, 0);
}

void bcNormalf(float x, float y, float z)
{
    s_TempVertexData[VERTEX_ATTR_NORMALS] = vec4(x, y, z, 0);
}

void bcColor4f(float r, float g, float b, float a)
{
    s_TempVertexData[VERTEX_ATTR_COLORS] = vec4(r, g, b, a);
}

void bcColor3f(float r, float g, float b)
{
    bcColor4f(r, g, b, 1.0f);
}

// Matrix Stack

void bcSetPerspective(float fovy, float aspect, float znear, float zfar)
{
    s_ProjectionMatrix = mat4_perspective(fovy, aspect, znear, zfar);
    bcSetProjectionMatrix(s_ProjectionMatrix.v);
}

void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
    s_ProjectionMatrix = mat4_ortho(left, right, bottom, top, znear, zfar);
    bcSetProjectionMatrix(s_ProjectionMatrix.v);
}

void bcPushMatrix()
{
    if (s_CurrentMatrix == MATRIX_STACK_SIZE - 1)
    {
        bcLog("Max matrix stack reached!");
        return;
    }
    s_CurrentMatrix++;
    s_MatrixStack[s_CurrentMatrix] = s_MatrixStack[s_CurrentMatrix - 1];
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

void bcPopMatrix()
{
    if (s_CurrentMatrix == 0)
    {
        bcLog("Min matrix stack reached!");
        return;
    }
    s_CurrentMatrix--;
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

void bcIdentity()
{
    s_MatrixStack[s_CurrentMatrix] = mat4_identity();
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

void bcTranslatef(float x, float y, float z)
{
    s_MatrixStack[s_CurrentMatrix] = mat4_translate(s_MatrixStack[s_CurrentMatrix], x, y, z);
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

void bcRotatef(float deg, float x, float y, float z)
{
    s_MatrixStack[s_CurrentMatrix] = mat4_rotate_axis(s_MatrixStack[s_CurrentMatrix], to_radians(deg), x, y, z);
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

void bcScalef(float x, float y, float z)
{
    s_MatrixStack[s_CurrentMatrix] = mat4_scale(s_MatrixStack[s_CurrentMatrix], x, y, z);
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

void bcLoadMatrix(float *m)
{
    s_MatrixStack[s_CurrentMatrix] = mat4_from_array(m);
    bcSetModelViewMatrix(s_MatrixStack[s_CurrentMatrix].v);
}

float * bcGetMatrix()
{
    return s_MatrixStack[s_CurrentMatrix].v;
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
    // bcSetLighting(true);
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

void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh)
{
    bcBindTexture(texture);
    // TODO: bcBegin(GL_TRIANGLES);
    bcBegin(0);
    bcTexCoord2f(sx, sy);
    bcVertex2f(x, y);
    bcTexCoord2f(sx + sw, sy);
    bcVertex2f(x + w, y);
    bcTexCoord2f(sx + sw, sy + sh);
    bcVertex2f(x + w, y + h);
    bcTexCoord2f(sx + sw, sy + sh);
    bcVertex2f(x + w, y + h);
    bcTexCoord2f(sx, sy + sh);
    bcVertex2f(x, y + h);
    bcTexCoord2f(sx, sy);
    bcVertex2f(x, y);
    bcEnd();
}

void bcDrawRect2D(float x, float y, float w, float h);
void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z);
