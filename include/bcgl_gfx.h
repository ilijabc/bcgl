#pragma once

#include "bcbase.h"

//
// enums
//

typedef enum
{
    BC_TEXTURE_LINEAR   = 0x1,
    BC_TEXTURE_NEAREST  = 0x2,
    BC_TEXTURE_MIPMAP   = 0x4,
    BC_TEXTURE_REPEAT   = 0x8,
    BC_TEXTURE_CLAMP    = 0x10,
    BC_TEXTURE_DETACHED = 0x20,
} BCTextureFlags;

typedef enum
{
    BC_MESH_POS2        = 0x1,
    BC_MESH_POS3        = 0x2,
    BC_MESH_POS4        = 0x4,
    BC_MESH_NORM        = 0x8,
    BC_MESH_TEX2        = 0x10,
    BC_MESH_TEX3        = 0x20,
    BC_MESH_COL1        = 0x40,
    BC_MESH_COL3        = 0x80,
    BC_MESH_COL4        = 0x100,
    BC_MESH_DEFAULT     = (BC_MESH_POS3 | BC_MESH_NORM | BC_MESH_TEX2 | BC_MESH_COL4),
} BCMeshFlags;

typedef enum
{
    BC_MESH_STATIC,
    BC_MESH_DYNAMIC,
    BC_MESH_OPTIMIZED,
    BC_MESH_NO_VBO,
} BCMeshType;

typedef enum
{
    BC_LINES,
    BC_LINE_LOOP,
    BC_LINE_STRIP,
    BC_TRIANGLES,
    BC_TRIANGLE_STRIP,
    BC_TRIANGLE_FAN,
    BC_QUADS // works only on mashes with indices
} BCDrawMode;

typedef enum
{
    BC_VERTEX_ATTR_POSITIONS,
    BC_VERTEX_ATTR_NORMALS,
    BC_VERTEX_ATTR_TEXCOORDS,
    BC_VERTEX_ATTR_COLORS,
    BC_VERTEX_ATTR_MAX
} BCVertexAttributes;

typedef enum
{
    BC_SHADER_UNIFORM_PROJECTION,
    BC_SHADER_UNIFORM_MODELVIEW,
    BC_SHADER_UNIFORM_TEXTURE,
    BC_SHADER_UNIFORM_USETEXTURE,
    BC_SHADER_UNIFORM_ALPHAONLYTEXTURE,
    BC_SHADER_UNIFORM_ALPHATEST,
    BC_SHADER_UNIFORM_VERTEX_COLOR_ENABLED,
    BC_SHADER_UNIFORM_COLOR_ARRAY,
    BC_SHADER_UNIFORM_LIGHT_ENABLED,
    BC_SHADER_UNIFORM_LIGHT_POSITION,
    BC_SHADER_UNIFORM_LIGHT_COLOR,
    BC_SHADER_UNIFORM_MAX
} BCShaderUniforms;

typedef enum
{
    BC_FONT_TRUETYPE,
    BC_FONT_ANGELCODE,
    BC_FONT_BITMAP
} BCFontType;

typedef enum
{
    BC_COLOR_TYPE_PRIMARY = 0,
    BC_COLOR_TYPE_SECONDARY,
    BC_COLOR_TYPE_DIFFUSE,
    BC_COLOR_TYPE_AMBIENT,
    BC_COLOR_TYPE_SPECULAR,
    BC_COLOR_TYPE_EMISSION,
    BC_COLOR_TYPE_CUSTOM_1,
    BC_COLOR_TYPE_CUSTOM_2,
    BC_COLOR_TYPE_CUSTOM_3,
    BC_COLOR_TYPE_CUSTOM_4,
    BC_COLOR_TYPE_MAX
} BCColorType;

//
// structs
//

typedef struct
{
    int width;
    int height;
    int comps;
    unsigned char *data;
} BCImage;

typedef struct
{
    uint8_t RM_type;
    unsigned int id;
    int width;
    int height;
    int format;
    int flags;
    BCImage *image;
} BCTexture;

typedef struct
{
    uint8_t RM_type;
    unsigned int programId;
    unsigned int vs_id;
    unsigned int fs_id;
    int loc_uniforms[BC_SHADER_UNIFORM_MAX];
    char *vs_code;
    char *fs_code;
} BCShader;

typedef struct
{
    const char *type;
    const char *name;
    int size;
} BCShaderVar;

typedef struct
{
    uint8_t RM_type;
    int num_vertices;
    int num_indices;
    int format;
    int comps[BC_VERTEX_ATTR_MAX];
    int total_comps;
    float *vertices;
    uint16_t *indices;
    int draw_mode;
    int draw_count;
    unsigned int vbo_vertices;
    unsigned int vbo_indices;
    BCMeshType type;
} BCMesh;

typedef struct
{
    BCFontType type;
    int char_first;
    int char_count;
    void *cdata;
    BCTexture *texture;
    float height;
} BCFont;

typedef struct
{
    float r, g, b, a;
} BCColor;

typedef struct
{
    BCMesh *mesh;
    int start;
    int count;
} BCMeshPart;

//
// macros
//

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
#define COLOR_TO_ARRAY(C) { C.r, C.g, C.b, C.a }

static const BCColor BC_COLOR_TRANSPARENT   = {0,0,0,0};
static const BCColor BC_COLOR_BLACK         = {0,0,0,1};
static const BCColor BC_COLOR_WHITE         = {1,1,1,1};
static const BCColor BC_COLOR_RED           = {1,0,0,1};
static const BCColor BC_COLOR_GREEN         = {0,1,0,1};
static const BCColor BC_COLOR_BLUE          = {0,0,1,1};
static const BCColor BC_COLOR_YELLOW        = {1,1,0,1};
static const BCColor BC_COLOR_CYAN          = {0,1,1,1};
static const BCColor BC_COLOR_MAGENTA       = {1,0,1,1};
static const BCColor BC_COLOR_GRAY          = {0.3f,0.3f,0.3f,1.0f};

//
// functions
//

#ifdef __cplusplus
extern "C" {
#endif

// Shader
BCShader * bcCreateShader(const char *vs_code, const char *fs_code);
BCShader * bcCreateShaderFromSingleFile(const char *filename);
BCShader * bcCreateShaderFromFile(const char *vsFilename, const char *fsFilename);
bool bcUpdateShader(BCShader *shader);
void bcReleaseShader(BCShader *shader);
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
BCTexture * bcCreateTextureFromFile(const char *filename, BCTextureFlags flags);
BCTexture * bcCreateTextureFromImage(BCImage *image, BCTextureFlags flags);
void bcUpdateTexture(BCTexture *texture);
void bcReleaseTexture(BCTexture *texture);
void bcDestroyTexture(BCTexture *texture);
void bcBindTexture(BCTexture *texture);

// View State
void bcClear(BCColor color);
void bcViewport(int x, int y, int width, int height);
void bcSetBlend(bool enabled);
void bcSetDepthTest(bool enabled);
void bcSetCulling(bool enabled);
void bcSetWireframe(bool enabled);
void bcSetLighting(bool enabled);
void bcLightPosition(float x, float y, float z);
void bcSetProjectionMatrix(float *m);
void bcSetModelViewMatrix(float *m);
float * bcGetProjectionMatrix();
float * bcGetModelViewMatrix();
void bcSetScissor(bool enabled);
void bcScissorRect(int x, int y, int w, int h);
void bcSetColor(BCColor color, BCColorType type);

// Mesh
BCMesh * bcCreateMesh(int format, const float *vert_data, int vert_num, const uint16_t *indx_data, int indx_num, BCMeshType type);
BCMesh * bcCopyMesh(BCMesh *mesh);
void bcUpdateMesh(BCMesh *mesh);
void bcReleaseMesh(BCMesh *mesh);
void bcDestroyMesh(BCMesh *mesh);
void bcDrawMesh(BCMesh *mesh);
void bcBindMesh(BCMesh *mesh);
void bcDrawMeshPart(BCMeshPart part);
void bcDrawMeshRange(BCMesh *mesh, int start, int count);
BCMeshPart bcPartFromMesh(BCMesh *mesh);
BCMeshPart bcAttachMesh(BCMesh *mesh, BCMesh *src, bool destroy_src);
BCMesh * bcCreateMeshFromFile(const char *filename);
bool bcSaveMeshToFile(BCMesh *mesh, const char *filename);

// IM
bool bcBegin(BCDrawMode mode);
void bcEnd();
bool bcBeginMesh(BCMesh *mesh, BCDrawMode mode);
void bcEndMesh(BCMesh *mesh);
int bcVertex3f(float x, float y, float z);
int bcVertex2f(float x, float y);
void bcIndexi(int i);
void bcTexCoord2f(float u, float v);
void bcNormal3f(float x, float y, float z);
void bcColor4f(float r, float g, float b, float a);
void bcColor3f(float r, float g, float b);
void bcColor(BCColor c);

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
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z, bool solid);
void bcDrawGrid(int size_x, int size_y);
void bcDrawPlane(int size_x, int size_y);

// Font
BCFont * bcCreateFontTTF(const char *filename, float height);
BCFont * bcCreateFontFNT(const char *filename);
BCFont * bcCreateFontBMP(const char *filename, int char_first, int char_count, int cols);
void bcUpdateFont(BCFont *font);
void bcReleaseFont(BCFont *font);
void bcDestroyFont(BCFont *font);
void bcDrawText(BCFont *font, float x, float y, const char *text);
void bcGetTextSize(BCFont *font, const char *text, float *px, float *py);

// Geometry
BCMesh * bcCreateMeshFromShape(void *par_shape);
BCMesh * bcCreateMeshCube();
BCMesh * bcCreateMeshBox(float x1, float y1, float z1, float x2, float y2, float z2);
BCMesh * bcCreateCylinder(float radius, float height, int slices);
BCMesh * bcCreateMeshSphere(float radius, int slices, int stacks);
BCMesh * bcCreateMeshWireBox(float x1, float y1, float z1, float x2, float y2, float z2);
void bcTransformMesh(BCMesh *mesh, float *m);
void bcDumpMesh(BCMesh *mesh, FILE *stream);
bool bcGetMeshAABB(BCMesh *mesh, float *minv, float *maxv);

#ifdef __cplusplus
}
#endif
