#include "bcgl_internal.h"

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
    if (g_Context->ReusableSolidMesh == NULL)
    {
        g_Context->ReusableSolidMesh = bcCreateMesh(1024, 1024, MESH_FLAGS_POS3 | MESH_FLAGS_NORM | MESH_FLAGS_TEX2 | MESH_FLAGS_COL4);
    }
    return bcBeginMesh(g_Context->ReusableSolidMesh, mode);
}

void bcEnd()
{
    bcEndMesh(g_Context->ReusableSolidMesh);
    bcUploadMesh(g_Context->ReusableSolidMesh, VBO_DYNAMIC);
    bcDrawMesh(g_Context->ReusableSolidMesh);
}

bool bcBeginMesh(BCMesh *mesh, enum BCDrawMode mode)
{
    if (g_Context->TempMesh != NULL)
    {
        bcLogWarning("Mesh already locked!");
        return false;
    }
    if (mesh == NULL)
    {
        bcLogWarning("Mesh not provided!");
        return false;
    }
    g_Context->TempMesh = mesh;
    g_Context->TempMesh->draw_count = 0;
    g_Context->TempMesh->draw_mode = convertDrawMode(mode);
    g_Context->TempVertexData[VERTEX_ATTR_POSITIONS] = vec4(0, 0, 0, 0);
    g_Context->TempVertexData[VERTEX_ATTR_NORMALS] = vec4(0, 0, 1, 0);
    g_Context->TempVertexData[VERTEX_ATTR_TEXCOORDS] = vec4(0, 0, 0, 0);
    g_Context->TempVertexData[VERTEX_ATTR_COLORS] = vec4(1, 1, 1, 1);
    g_Context->VertexCounter = 0;
    g_Context->IndexCounter = 0;
    g_Context->DrawMode = mode;
    return true;
}

void bcEndMesh(BCMesh *mesh)
{
    if (mesh != g_Context->TempMesh)
    {
        bcLogError("Wrong mesh!");
    }
    // generate indices
    if (g_Context->TempMesh->num_indices > 0 && g_Context->IndexCounter == 0)
    {
        for (int i = 0; i < g_Context->VertexCounter; i++)
        {
            if (g_Context->DrawMode == BC_QUADS && i % 4 == 3)
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
    g_Context->TempMesh->draw_count = (g_Context->TempMesh->num_indices > 0) ? g_Context->IndexCounter : g_Context->VertexCounter;
    g_Context->TempMesh = NULL;
}

int bcVertex3f(float x, float y, float z)
{
    if (g_Context->TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return -1;
    }
    if (g_Context->VertexCounter == g_Context->TempMesh->num_vertices)
    {
        bcLogWarning("Mesh limit reached!");
        return -1;
    }
    g_Context->TempVertexData[VERTEX_ATTR_POSITIONS] = vec4(x, y, z, 0);
    float *vert_ptr = &(g_Context->TempMesh->vertices[g_Context->VertexCounter * g_Context->TempMesh->total_comps]);
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        if (g_Context->TempMesh->comps[i] > 0)
        {
            memcpy(vert_ptr, g_Context->TempVertexData[i].v, g_Context->TempMesh->comps[i] * sizeof(float));
            vert_ptr += g_Context->TempMesh->comps[i];
        }
    }
    return g_Context->VertexCounter++;
}

int bcVertex2f(float x, float y)
{
    return bcVertex3f(x, y, 0);
}

void bcIndexi(int i)
{
    if (g_Context->TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return;
    }
    if (g_Context->IndexCounter == g_Context->TempMesh->num_indices)
    {
        bcLogWarning("Mesh limit reached!");
        return;
    }
    g_Context->TempMesh->indices[g_Context->IndexCounter++] = i;
}

void bcTexCoord2f(float u, float v)
{
    g_Context->TempVertexData[VERTEX_ATTR_TEXCOORDS] = vec4(u, v, 0, 0);
}

void bcNormal3f(float x, float y, float z)
{
    g_Context->TempVertexData[VERTEX_ATTR_NORMALS] = vec4(x, y, z, 0);
}

void bcColor4f(float r, float g, float b, float a)
{
    g_Context->TempVertexData[VERTEX_ATTR_COLORS] = vec4(r, g, b, a);
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
    if (g_Context->MatrixCounter == MATRIX_STACK_SIZE - 1)
    {
        bcLogWarning("Max matrix stack reached!");
        return;
    }
    g_Context->MatrixStack[g_Context->MatrixCounter] = getCurrentMatrix();
    g_Context->MatrixCounter++;
}

void bcPopMatrix()
{
    if (g_Context->MatrixCounter == 0)
    {
        bcLogWarning("Min matrix stack reached!");
        return;
    }
    g_Context->MatrixCounter--;
    bcSetModelViewMatrix(g_Context->MatrixStack[g_Context->MatrixCounter].v);
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

void bcMultMatrixf(float *m)
{
    bcSetModelViewMatrix(mat4_multiply(getCurrentMatrix(), mat4_from_array(m)).v);
}

bool bcScreenToWorldCoords(int winX, int winY, float out[3])
{
    int viewport[4] = { 0, 0, bcGetDisplayWidth(), bcGetDisplayHeight() };
    winY = bcGetDisplayHeight() - winY;
    float winZ = 0;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    mat4_t mvp = mat4_from_array(bcGetModelViewProjectionMatrix());
    vec4_t v = mat4_unproject(mvp, winX, winY, winZ, viewport);
    if (out)
    {
        out[0] = v.x;
        out[1] = v.y;
        out[2] = v.z;
    }
    return true;
}

bool bcWorldToScreenCoords(float x, float y, float z, float out[2])
{
    int viewport[4] = { 0, 0, bcGetDisplayWidth(), bcGetDisplayHeight() };
    mat4_t mvp = mat4_from_array(bcGetModelViewProjectionMatrix());
    vec4_t v = mat4_project(mvp, x, y, z, viewport);
    if (out)
    {
        out[0] = v.x;
        out[1] = bcGetDisplayHeight() - v.y;
    }
    return true;
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

void bcPrepareScene2D(float height, bool center)
{
    float width = bcGetDisplayAspectRatio() * height;
    if (center)
        bcSetOrtho(-width / 2, width / 2, -height / 2, height / 2, -1, 1);
    else
        bcSetOrtho(0, width, 0, height, -1, 1);
    bcIdentity();
    bcSetBlend(true);
    bcSetDepthTest(false);
    bcSetLighting(false);
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

void bcDrawRect2D(float x, float y, float w, float h, bool fill)
{
    bcBegin(fill ? BC_QUADS : BC_LINE_LOOP);
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

void bcDrawCircle2D(float x, float y, float r, int segments, bool fill)
{
    bcBegin(fill ? BC_TRIANGLE_FAN : BC_LINE_LOOP);
    int n = segments;
    if (fill)
    {
        bcVertex2f(x, y);
        n++;
    }
    for (int i = 0; i < n; i++)
    {
        float t = (float) i / (float) segments * M_PI * 2;
        bcVertex2f(x + cosf(t) * r, y + sinf(t) * r);
    }
    bcEnd();
}

void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z)
{
    if (g_Context->ReusableCubeMesh == NULL)
        g_Context->ReusableCubeMesh = bcUploadMesh(bcCreateMeshCube(), VBO_STATIC);
    bcPushMatrix();
    bcTranslatef(x, y, z);
    bcScalef(size_x, size_y, size_z);
    bcDrawMesh(g_Context->ReusableCubeMesh);
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
