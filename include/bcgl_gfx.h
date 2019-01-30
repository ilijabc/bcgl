#pragma once

#include "bcgl_common.h"

// Texture params
#define TEXTURE_PARAM_LINEAR    0x1
#define TEXTURE_PARAM_NEAREST   0x2
#define TEXTURE_PARAM_MIPMAP    0x4
#define TEXTURE_PARAM_REPEAT    0x8
#define TEXTURE_PARAM_CLAMP     0x10

// Mesh flags
#define MESH_FLAGS_POS2         0x1
#define MESH_FLAGS_POS3         0x2
#define MESH_FLAGS_POS4         0x4
#define MESH_FLAGS_NORM         0x8
#define MESH_FLAGS_TEX2         0x10
#define MESH_FLAGS_TEX3         0x20
#define MESH_FLAGS_COL3         0x40
#define MESH_FLAGS_COL4         0x80

#define MESH_FLAGS_DEFAULT      (MESH_FLAGS_POS3 | MESH_FLAGS_NORM | MESH_FLAGS_TEX2 | MESH_FLAGS_COL4)

// enums

enum BCDrawMode
{
    BC_LINES,
    BC_LINE_LOOP,
    BC_LINE_STRIP,
    BC_TRIANGLES,
    BC_TRIANGLE_STRIP,
    BC_TRIANGLE_FAN,
    BC_QUADS // works only on mashes with indices
};

enum BCVertexAttributes
{
    VERTEX_ATTR_POSITIONS,
    VERTEX_ATTR_NORMALS,
    VERTEX_ATTR_TEXCOORDS,
    VERTEX_ATTR_COLORS,
    VERTEX_ATTR_MAX
};

enum BCShaderUniforms
{
    SHADER_UNIFORM_PROJECTION,
    SHADER_UNIFORM_MODELVIEW,
    SHADER_UNIFORM_TEXTURE,
    SHADER_UNIFORM_USETEXTURE,
    SHADER_UNIFORM_ALPHAONLYTEXTURE,
    SHADER_UNIFORM_ALPHATEST,
    SHADER_UNIFORM_VERTEX_COLOR_ENABLED,
    SHADER_UNIFORM_OBJECT_COLOR,
    SHADER_UNIFORM_DIFFUSE_COLOR,
    SHADER_UNIFORM_AMBIENT_COLOR,
    SHADER_UNIFORM_LIGHT_ENABLED,
    SHADER_UNIFORM_LIGHT_POSITION,
    SHADER_UNIFORM_LIGHT_COLOR,
    SHADER_UNIFORM_MAX
};

enum BCFontType
{
    FONT_TYPE_TRUETYPE,
    FONT_TYPE_ANGELCODE,
    FONT_TYPE_BITMAP
};

enum BCVboStatus
{
    VBO_EMPTY,
    VBO_STATIC,
    VBO_DYNAMIC
};

typedef struct
{
    int width;
    int height;
    int comps;
    unsigned char *data;
} BCImage;

typedef struct
{
    unsigned int id;
    int width;
    int height;
    int format;
} BCTexture;

typedef struct
{
    unsigned int programId;
    int loc_uniforms[SHADER_UNIFORM_MAX];
} BCShader;

typedef struct
{
    int num_vertices;
    int num_indices;
    int format;
    int comps[VERTEX_ATTR_MAX];
    int total_comps;
    float *vertices;
    uint16_t *indices;
    int draw_mode;
    int draw_count;
    enum BCVboStatus vbo_status;
    unsigned int vbo_vertices;
    unsigned int vbo_indices;
} BCMesh;

typedef struct
{
    enum BCFontType type;
    int char_first;
    int char_count;
    void *cdata;
    BCTexture *texture;
} BCFont;

typedef struct
{
    float r, g, b, a;
} BCColor;

typedef struct
{
    BCColor objectColor;
    BCColor diffuseColor;
    BCColor ambientColor;
    BCTexture *texture;
} BCMaterial;

typedef struct
{
    BCMesh *mesh;
    char *name;
    int start;
    int count;
} BCMeshPart;

typedef struct
{
    BCMesh *mesh;
    BCMaterial material;
    int parts_count;
    BCMeshPart *parts_list;
} BCModel;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RGB_COLOR(C)  (BCColor) { (C >> 16 & 0xff) / 255.0f, (C >> 8 & 0xff) / 255.0f, (C & 0xff) / 255.0f, 1.0f }
#define RGBA_COLOR(C) (BCColor) { (C >> 24 & 0xff) / 255.0f, (C >> 16 & 0xff) / 255.0f, (C >> 8 & 0xff) / 255.0f, (C & 0xff) / 255.0f }
#define ARGB_COLOR(C) (BCColor) { (C >> 16 & 0xff) / 255.0f, (C >> 8 & 0xff) / 255.0f, (C & 0xff) / 255.0f, (C >> 24 & 0xff) / 255.0f }
#else
#define RGB_COLOR(C)  (BCColor) { (C >> 8 & 0xff) / 255.0f, (C >> 16 & 0xff) / 255.0f, (C >> 24 & 0xff) / 255.0f, 1.0f }
#define RGBA_COLOR(C) (BCColor) { (C & 0xff) / 255.0f, (C >> 8 & 0xff) / 255.0f, (C >> 16 & 0xff) / 255.0f, (C >> 24& 0xff) / 255.0f }
#define ARGB_COLOR(C) (BCColor) { (C >> 8 & 0xff) / 255.0f, (C >> 16 & 0xff) / 255.0f, (C >> 24 & 0xff) / 255.0f, (C & 0xff) / 255.0f }
#endif
#define SET_COLOR(r,g,b,a) (BCColor) { r, g, b, a }

static const BCColor COLOR_WHITE = {1,1,1,1};
static const BCColor COLOR_RED = {1,0,0,1};
static const BCColor COLOR_GREEN = {0,1,0,1};
static const BCColor COLOR_BLUE = {0,0,1,1};

#ifdef __cplusplus
extern "C" {
#endif

void bcInitGfx();
void bcTermGfx();

// Shader
BCShader * bcCreateShaderFromSingleFile(const char *filename);
BCShader * bcCreateShaderFromFile(const char *vsFilename, const char *fsFilename);
BCShader * bcCreateShaderFromCode(const char *vsCode, const char *fsCode);
void bcDestroyShader(BCShader *shader);
void bcBindShader(BCShader *shader);
unsigned int bcLoadShader(const char *code, unsigned int shaderType);
bool bcLinkShaderProgram(unsigned int programId);

// Image
BCImage * bcCreateImage(int width, int height, int format);
BCImage * bcCreateImageFromFile(const char *filename);
BCImage * bcCreateImageFromMemory(void *buffer, int size);
void bcDestroyImage(BCImage *image);

// Texture
BCTexture * bcCreateTextureFromFile(const char *filename, int flags);
BCTexture * bcCreateTextureFromImage(BCImage *image, int flags);
void bcDestroyTexture(BCTexture *texture);
void bcBindTexture(BCTexture *texture);

// View State
void bcClear();
void bcSetBlend(bool enabled);
void bcSetDepthTest(bool enabled);
void bcSetWireframe(bool enabled);
void bcSetLighting(bool enabled);
void bcLightPosition(float x, float y, float z);
void bcSetMaterial(BCMaterial material);
void bcResetMaterial();
void bcSetObjectColor(BCColor color);
void bcSetObjectColorf(float r, float g, float b, float a);
void bcSetProjectionMatrix(float *m);
void bcSetModelViewMatrix(float *m);
float * bcGetProjectionMatrix();
float * bcGetModelViewMatrix();

// Mesh
BCMesh * bcCreateMesh(int num_vertices, int num_indices, int flags);
BCMesh * bcUploadMesh(BCMesh *mesh, enum BCVboStatus status);
BCMesh * bcCopyMesh(BCMesh *mesh);
void bcDestroyMesh(BCMesh *mesh);
void bcDrawMesh(BCMesh *mesh);
void bcBindMesh(BCMesh *mesh);
void bcDrawMeshPart(BCMeshPart part);
void bcDrawMeshRange(BCMesh *mesh, int start, int count);
BCMeshPart bcPartFromMesh(BCMesh *mesh);
BCMeshPart bcAttachMesh(BCMesh *mesh, BCMesh *src, bool destroy_src);

// IM
bool bcBegin(enum BCDrawMode mode);
void bcEnd();
bool bcBeginMesh(BCMesh *mesh, enum BCDrawMode mode);
void bcEndMesh(BCMesh *mesh);
int bcVertex3f(float x, float y, float z);
int bcVertex2f(float x, float y);
void bcIndexi(int i);
void bcTexCoord2f(float u, float v);
void bcNormal3f(float x, float y, float z);
void bcColor4f(float r, float g, float b, float a);
void bcColor3f(float r, float g, float b);

// Matrix Stack
void bcSetPerspective(float fovy, float aspect, float znear, float zfar);
void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar);
void bcPushMatrix();
void bcPopMatrix();
void bcIdentity();
void bcTranslatef(float x, float y, float z);
void bcRotatef(float deg, float x, float y, float z);
void bcScalef(float x, float y, float z);
void bcMultMatrixf(float *m);
bool bcScreenToWorldCoords(int winX, int winY, float out[3]);
bool bcWorldToScreenCoords(float x, float y, float z, float out[2]);

// Camera
void bcPrepareScene3D(float fovy);
void bcPrepareScene2D(float height, bool center);
void bcPrepareSceneGUI();

// Draw 2D
void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh);
void bcDrawRect2D(float x, float y, float w, float h, bool fill);
void bcDrawCircle2D(float x, float y, float r, int segments, bool fill);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z);
void bcDrawGrid(int size_x, int size_y);
void bcDrawPlane(int size_x, int size_y);

// Font
BCFont * bcCreateFontTTF(const char *filename, float height);
BCFont * bcCreateFontFNT(const char *filename);
BCFont * bcCreateFontBMP(const char *filename, int char_first, int char_count, int cols);
void bcDestroyFont(BCFont *font);
void bcDrawText(BCFont *font, float x, float y, const char *text);
void bcGetTextSize(BCFont *font, const char *text, float *px, float *py);

// Geometry
BCMesh * bcCreateMeshFromShape(void *par_shape);
BCMesh * bcCreateMeshCube();
BCMesh * bcCreateMeshBox(float x1, float y1, float z1, float x2, float y2, float z2);
BCMesh * bcCreateCylinder(float radius, float height, int slices);
BCMesh * bcCreateMeshSphere(float radius, int slices, int stacks);
void bcTransformMesh(BCMesh *mesh, float *m);
void bcDumpMesh(BCMesh *mesh, FILE *stream);

// Model
BCModel * bcCreateModel(BCMesh *mesh, BCMaterial material, int parts);
BCModel * bcCreateModelFromFile(const char *filename);
void bcDestroyModel(BCModel *model);
void bcDrawModel(BCModel *model);
void bcBeginModelDraw(BCModel *model);
void bcEndModelDraw(BCModel *model);
void bcDrawModelPart(BCModel *model, int part);
int bcGetModelPartByName(BCModel *model, const char *name);

#ifdef __cplusplus
}
#endif