#include "bcgl.h"
#include "bcgl_gl.h"

#include <stb/stb_image.h>
#include <mathc/mathc.h>

#define MATRIX_STACK_SIZE 32

static float s_BackgroundColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
static float s_ProjectionMatrix[16];
static float s_MatrixStack[MATRIX_STACK_SIZE][16];
static int s_CurrentMatrix = 0;

//
// Image
//

BCImage * bcCreateImage(int width, int height, int comps)
{
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = width;
    image->height = height;
    image->comps = comps;
    image->data = calloc(1, width * height * comps);
    return image;
}

BCImage * bcCreateImageFromFile(const char *filename)
{
    int x, y, comp;
    unsigned char *data = stbi_load(filename, &x, &y, &comp, 0);
    if (data == NULL)
    {
        bcLog("Image file '%s' not found!", filename);
        return NULL;
    }
    bcLog("Image file '%s' loaded OK", filename);
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = x;
    image->height = y;
    image->comps = comp;
    image->data = data;
    return image;
}

BCImage * bcCreateImageFromMemory(void *buffer, int size)
{
    int x, y, comp;
    unsigned char *data = stbi_load_from_memory(buffer, size, &x, &y, &comp, 0);
    if (data == NULL)
    {
        bcLog("Image data not valid!");
        return NULL;
    }
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = x;
    image->height = y;
    image->comps = comp;
    image->data = data;
    return image;
}

void bcDestroyImage(BCImage *image)
{
    free(image->data);
    free(image);
}

//
// Texture
//

BCTexture * bcCreateTextureFromFile(const char *filename, int flags)
{
    BCImage *image = bcCreateImageFromFile(filename);
    if (image == NULL)
        return NULL;
    BCTexture *texture = bcCreateTextureFromImage(image, flags);
    bcDestroyImage(image);
    return texture;
}

static int get_glFormat(int comps)
{
    switch (comps)
    {
    case 1: return GL_ALPHA;
    case 3: return GL_RGB;
    case 4: return GL_RGBA;
    }
    return -1;
}

BCTexture * bcCreateTextureFromImage(BCImage *image, int flags)
{
    BCTexture *texture = NEW_OBJECT(BCTexture);
    texture->width = image->width;
    texture->height = image->height;
    switch (image->comps)
    {
    case 1: texture->format = GL_ALPHA;
    case 3: texture->format = GL_RGB;
    case 4: texture->format = GL_RGBA;
    }
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &(texture->id));
    glBindTexture(GL_TEXTURE_2D, (texture->id));
    // filter flags
    if (flags & TEXTURE_PARAM_LINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (flags & TEXTURE_PARAM_NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (flags & TEXTURE_PARAM_MIPMAP)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        // mMipmaps = true;
    }
    else
    {
        // linear filter by default
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // wrap flags
    if (flags & TEXTURE_PARAM_REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if (flags & TEXTURE_PARAM_CLAMP)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        texture->format,
        texture->width,
        texture->height,
        0,
        texture->format,
        GL_UNSIGNED_BYTE,
        image->data);
    // if (mMipmaps) {
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void bcDestroyTexture(BCTexture *texture)
{
    glDeleteTextures(1, &(texture->id));
    free(texture);
}

void bcBindTexture(BCTexture *texture)
{
#ifdef SUPPORT_GLSL
    getShader()->setTextureEnabledUniform(enable);
#else
    if (texture)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
#endif
    if (texture)
        glBindTexture(GL_TEXTURE_2D, texture->id);
}

void bcDrawTexture(BCTexture *texture)
{
    float w = texture->width;
    float h = texture->height;

    bcBindTexture(texture);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(w, 0);
    glTexCoord2f(1, 1);
    glVertex2f(w, h);
    glTexCoord2f(1, 1);
    glVertex2f(w, h);
    glTexCoord2f(0, 1);
    glVertex2f(0, h);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glEnd();
}

// Font
BCFont * bcCreateFontFromFile(const char *filename);
BCFont * bcCreateFontFromMemory(void *buffer);
void bcDestroyFont(BCFont *font);

// Shader
BCShader * bcCreateShaderFromFile(const char *filename);
BCShader * bcCreateShaderFromCode(const char *code);
void bcDestroyShader(BCShader *shader);

// View State

void bcInitGL()
{
#ifdef SUPPORT_GLSL
    setShader(mCurrentShader);
#else
    glAlphaFunc(GL_GREATER, 0.1f);
    glDisable(GL_LIGHTING);
#endif
    // default state
    glClearColor(s_BackgroundColor[0], s_BackgroundColor[1], s_BackgroundColor[2], s_BackgroundColor[3]);
    // gl default
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // matrix stack
    mat2_identity(s_ProjectionMatrix);
    mat2_identity(s_MatrixStack[0]);
}

void bcClear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void bcSetColor(BCColor color)
{
    glColor4f(color.r, color.g, color.b, color.a);
}

void bcSetBlend(bool enable)
{
    if (enable)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void bcSetDepthTest(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

// Matrix Stack

void bcSetPerspective(float fovy, float aspect, float znear, float zfar)
{
    mat4_perspective(s_ProjectionMatrix, fovy, aspect, znear, zfar);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(s_ProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}

void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
    mat4_ortho(s_ProjectionMatrix, left, right, bottom, top, znear, zfar);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(s_ProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}

static void applyCurrentMatrix()
{
    glLoadMatrixf(s_MatrixStack[s_CurrentMatrix]);
}

void bcPushMatrix()
{
    if (s_CurrentMatrix == MATRIX_STACK_SIZE - 1)
    {
        bcLog("Max matrix stack reached!");
        return;
    }
    s_CurrentMatrix++;
    mat4_assign(s_MatrixStack[s_CurrentMatrix], s_MatrixStack[s_CurrentMatrix - 1]);
    applyCurrentMatrix();
}

void bcPopMatrix()
{
    if (s_CurrentMatrix == 1)
    {
        bcLog("Min matrix stack reached!");
        return;
    }
    s_CurrentMatrix--;
    applyCurrentMatrix();
}

void bcIdentity()
{
    mat4_identity(s_MatrixStack[s_CurrentMatrix]);
    applyCurrentMatrix();
}

void bcTranslatef(float x, float y, float z)
{
    float m[16];
    float v[3] = { x, y, z };
    mat4_identity(m);
    mat4_translation(s_MatrixStack[s_CurrentMatrix], m, v);
    applyCurrentMatrix();
}

void bcRotatef(float angle, float x, float y, float z)
{
}

void bcScalef(float x, float y, float z)
{
}

// Camera

void bcPrepareScene3D()
{
    BCWindow *win = bcGetWindow();
    float fovy = 60.0f;
    float aspect = (float) win->width / (float) win->height;
    float znear = 0.1f;
    float zfar = 10000.0f;
    bcSetPerspective(fovy, aspect, znear, zfar);
    bcSetBlend(false);
    bcSetDepthTest(true);
    bcIdentity();
}

void bcPrepareScene2D()
{
}

void bcPrepareSceneGUI()
{
    BCWindow *win = bcGetWindow();
    bcSetOrtho(0, win->width, win->height, 0, -1, 1);
    bcSetBlend(true);
    bcSetDepthTest(false);
    bcIdentity();
}

// Mesh
BCMesh * bcCreateMeshFromFile(const char *filename);
BCMesh * bcCreateMeshFromMemory(void *buffer);
BCMesh * bcCreateMesh(int flags);
void bcDestroyFont(BCMesh *mesh);
void bcBeginMesh(BCMesh *mesh);
void bcVertex3f(float c, float y, float z);
void bcTextureCoord2f(float u, float v);
void bcTextureCoord3f(float u, float v);
void bcNormalf(float x, float y, float z);
void bcColor(BCColor color);
void bcColorHex(unsigned int argb);
void bcEndMesh();
void bcDrawMesh(BCMesh *mesh);

// Draw 2D
void bcDrawText2D(BCFont *font, float x, float y);
void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh);
void bcDrawRect2D(float x, float y, float w, float h);
void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z);
