#include "bcgl_internal.h"

#define MATRIX_STACK_SIZE 32

static BCMesh * s_TempMesh = NULL;
static vec4_t s_TempVertexData[VERTEX_ATTR_MAX];
static int s_VertexCounter = -1;
static int s_IndexCounter = -1;
static enum BCDrawMode s_DrawMode;

static mat4_t s_MatrixStack[MATRIX_STACK_SIZE];
static int s_MatrixCounter = 0;

static BCMesh *s_ReusableSolidMesh = NULL;
static BCMesh *s_ReusableCubeMesh = NULL;

void bcInitGfxDraw()
{
}

void bcTermGfxDraw()
{
    if (s_ReusableSolidMesh)
        bcDestroyMesh(s_ReusableSolidMesh);
    if (s_ReusableCubeMesh)
        bcDestroyMesh(s_ReusableCubeMesh);
}

static int convertDrawMode(enum BCDrawMode mode)
{
    switch (mode)
    {
    case BC_LINES:
        return GL_LINES;
    case BC_LINE_LOOP:
        return GL_LINE_LOOP;
    case BC_LINE_STRIP:
        return GL_LINE_STRIP;
    case BC_TRIANGLES:
        return GL_TRIANGLES;
    case BC_TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    case BC_TRIANGLE_FAN:
        return GL_TRIANGLE_FAN;
    case BC_QUADS:
        return GL_TRIANGLES;
    }
}

// IM

bool bcBegin(enum BCDrawMode mode)
{
    if (s_ReusableSolidMesh == NULL)
        s_ReusableSolidMesh = bcCreateMesh(1024, 1024, MESH_FLAGS_POS3 | MESH_FLAGS_NORM | MESH_FLAGS_TEX2 | MESH_FLAGS_COL4);
    return bcBeginMesh(s_ReusableSolidMesh, mode);
}

void bcEnd()
{
    bcEndMesh(s_ReusableSolidMesh);
    bcDrawMesh(s_ReusableSolidMesh);
}

bool bcBeginMesh(BCMesh *mesh, enum BCDrawMode mode)
{
    if (s_TempMesh != NULL)
    {
        bcLogWarning("Mesh already locked!");
        return false;
    }
    if (mesh == NULL)
    {
        bcLogWarning("Mesh not provided!");
        return false;
    }
    s_TempMesh = mesh;
    s_TempMesh->draw_count = 0;
    s_TempMesh->draw_mode = convertDrawMode(mode);
    s_TempVertexData[VERTEX_ATTR_POSITIONS] = vec4(0, 0, 0, 0);
    s_TempVertexData[VERTEX_ATTR_NORMALS] = vec4(0, 0, 1, 0);
    s_TempVertexData[VERTEX_ATTR_TEXCOORDS] = vec4(0, 0, 0, 0);
    s_TempVertexData[VERTEX_ATTR_COLORS] = vec4(1, 1, 1, 1);
    s_VertexCounter = 0;
    s_IndexCounter = 0;
    s_DrawMode = mode;
    return true;
}

void bcEndMesh()
{
    // generate indices
    if (s_TempMesh->num_indices > 0 && s_IndexCounter == 0)
    {
        for (int i = 0; i < s_VertexCounter; i++)
        {
            if (s_DrawMode == BC_QUADS && i % 4 == 3)
            {
                bcIndexi(i - 3);
                bcIndexi(i - 1);
                bcIndexi(i);
            }
            else
            {
                bcIndexi(i);
            }
        }
    }
    // assign counter
    s_TempMesh->draw_count = (s_TempMesh->num_indices > 0) ? s_IndexCounter : s_VertexCounter;
    s_TempMesh = NULL;
}

int bcVertex3f(float x, float y, float z)
{
    if (s_TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return -1;
    }
    if (s_VertexCounter == s_TempMesh->num_vertices)
    {
        bcLogWarning("Mesh limit reached!");
        return -1;
    }
    s_TempVertexData[VERTEX_ATTR_POSITIONS] = vec4(x, y, z, 0);
    float *vert_ptr = &(s_TempMesh->vertices[s_VertexCounter * s_TempMesh->total_comps]);
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        if (s_TempMesh->comps[i] > 0)
        {
            memcpy(vert_ptr, s_TempVertexData[i].v, s_TempMesh->comps[i] * sizeof(float));
            vert_ptr += s_TempMesh->comps[i];
        }
    }
    return s_VertexCounter++;
}

int bcVertex2f(float x, float y)
{
    return bcVertex3f(x, y, 0);
}

void bcIndexi(int i)
{
    if (s_TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return;
    }
    if (s_IndexCounter == s_TempMesh->num_indices)
    {
        bcLogWarning("Mesh limit reached!");
        return;
    }
    s_TempMesh->indices[s_IndexCounter++] = i;
}

void bcTexCoord2f(float u, float v)
{
    s_TempVertexData[VERTEX_ATTR_TEXCOORDS] = vec4(u, v, 0, 0);
}

void bcNormal3f(float x, float y, float z)
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
    bcSetProjectionMatrix(mat4_perspective(fovy, aspect, znear, zfar).v);
}

void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
    bcSetProjectionMatrix(mat4_ortho(left, right, bottom, top, znear, zfar).v);
}

static mat4_t getCurrentMatrix()
{
    return mat4_from_array(bcGetModelViewMatrix());
}

void bcPushMatrix()
{
    if (s_MatrixCounter == MATRIX_STACK_SIZE - 1)
    {
        bcLogWarning("Max matrix stack reached!");
        return;
    }
    s_MatrixStack[s_MatrixCounter] = getCurrentMatrix();
    s_MatrixCounter++;
}

void bcPopMatrix()
{
    if (s_MatrixCounter == 0)
    {
        bcLogWarning("Min matrix stack reached!");
        return;
    }
    s_MatrixCounter--;
    bcSetModelViewMatrix(s_MatrixStack[s_MatrixCounter].v);
}

void bcIdentity()
{
    bcSetModelViewMatrix(mat4_identity().v);
}

void bcTranslatef(float x, float y, float z)
{
    bcSetModelViewMatrix(mat4_translate(getCurrentMatrix(), x, y, z).v);
}

void bcRotatef(float deg, float x, float y, float z)
{
    bcSetModelViewMatrix(mat4_rotate_axis(getCurrentMatrix(), to_radians(deg), x, y, z).v);
}

void bcScalef(float x, float y, float z)
{
    bcSetModelViewMatrix(mat4_scale(getCurrentMatrix(), x, y, z).v);
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

void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh)
{
    bcBindTexture(texture);
    bcBegin(BC_TRIANGLES);
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

void bcDrawRect2D(enum BCDrawMode mode, float x, float y, float w, float h)
{
    bcBegin(mode);
    bcTexCoord2f(0, 0);
    bcVertex2f(x, y);
    bcTexCoord2f(1, 0);
    bcVertex2f(x + w, y);
    bcTexCoord2f(1, 1);
    bcVertex2f(x + w, y + h);
    bcTexCoord2f(0, 1);
    bcVertex2f(x, y + h);
    bcEnd();
}

void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z)
{
    if (s_ReusableCubeMesh == NULL)
        s_ReusableCubeMesh = bcCreateMeshCube();
    bcPushMatrix();
    bcTranslatef(x, y, z);
    bcScalef(size_x, size_y, size_z);
    bcDrawMesh(s_ReusableCubeMesh);
    bcPopMatrix();
}

void bcDrawGrid(int size_x, int size_y)
{
    bcBegin(BC_LINES);
    for (int x = 0; x < size_x + 1; x++)
    {
        bcVertex2f(x, 0);
        bcVertex2f(x, size_y);
    }
    for (int y = 0; y < size_y + 1; y++)
    {
        bcVertex2f(0, y);
        bcVertex2f(size_x, y);
    }
    bcEnd();
}

void bcDrawPlane(int size_x, int size_y)
{
    bcBegin(BC_QUADS);
    bcVertex2f(0, 0);
    bcVertex2f(size_x, 0);
    bcVertex2f(size_x, size_y);
    bcVertex2f(0, size_y);
    bcEnd();
}
