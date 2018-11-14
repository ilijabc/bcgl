#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

#define ASSETS_DIR "assets/"

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

enum BCFileMode
{
    FILE_READ,
    FILE_WRITE,
    FILE_APPEND
};

enum BCVboStatus
{
    VBO_EMPTY,
    VBO_STATIC,
    VBO_DYNAMIC
};

typedef struct
{
    void *handle;
    char *name;
    bool isDir;
    bool isAsset;
    size_t length;
    void *aux;
} BCFile;

typedef struct
{
    const char *title;
    int width;
    int height;
    int format;
    int mode;
    bool vsync;
    int msaa;
    int orientation;
    void *surface;
} BCConfig;

typedef struct
{
    int type;
    int id;
    int x;
    int y;
} BCEvent;

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
    unsigned int vertexShader;
    unsigned int fragmentShader;
    int loc_attributes[VERTEX_ATTR_MAX];
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

#define NEW_OBJECT(T)       (T*)calloc(1, sizeof(T))
#define NEW_ARRAY(N,T)      (T*)calloc(N, sizeof(T))
#define EXTEND_ARRAY(P,N,T) (T*)realloc(P, (N) * sizeof(T))

#ifdef __ANDROID__
#include <android/log.h>
#define bcLog(format, ...) __android_log_print(ANDROID_LOG_INFO, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#define bcLogWarning(format, ...) __android_log_print(ANDROID_LOG_WARN, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#define bcLogError(format, ...) __android_log_print(ANDROID_LOG_ERROR, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#else
#define bcLog(format, ...) { printf("[%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
#define bcLogWarning(format, ...) { printf("[WARNING] [%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
#define bcLogError(format, ...) { printf("[ERROR] [%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
#endif

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

//
// !!! OVERRIDE THIS IN APP !!!
//
typedef struct
{
    void (*onConfig)(BCConfig *config);
    void (*onCreate)();
    void (*onDestroy)();
    void (*onStart)();
    void (*onStop)();
    void (*onUpdate)(float dt);
    void (*onEvent)(BCEvent event);
} BCCallbacks;

//
// bcgl_app module
//

void bcInit(BCCallbacks callbacks);
int bcRunMain();
void bcQuit(int code);
float bcGetTime();
void bcShowKeyboard(bool show);
int bcGetDisplayWidth();
int bcGetDisplayHeight();
float bcGetDisplayAspectRatio();
float bcGetDisplayDensity();

//
// bcgl_app_common
//

void bcResetStates();
void bcSetMousePosition(int x, int y);
bool bcIsKeyDown(int key);
int bcGetMouseX();
int bcGetMouseY();
bool bcIsMouseDown(int button);
float bcGetMouseWheel();
float bcGetMouseDeltaX();
float bcGetMouseDeltaY();
bool bcIsTouchDown(int index);
float bcGetTouchX(int index);
float bcGetTouchY(int index);

//
// bcgl_file module
//

// File
BCFile * bcOpenFile(const char *filename, enum BCFileMode mode);
void bcCloseFile(BCFile *file);
size_t bcReadFile(BCFile *file, void* buf, size_t count);
size_t bcWriteFile(BCFile *file, void* buf, size_t count);
size_t bcSeekFile(BCFile *file, size_t offset);
size_t bcGetFilePosition(BCFile *file);
const char * bcReadFileLine(BCFile *file);

// Dir
BCFile * bcOpenDir(const char *filename);
void bcCloseDir(BCFile *file);
void bcRewindDir(BCFile *file);
const char * bcGetNextFileName(BCFile *file);

// Data
char * bcLoadTextFile(const char *filename, int *out_size);
unsigned char * bcLoadDataFile(const char *filename, int *out_size);

//
// bcgl_gfx module
//

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

// Shader
BCShader * bcCreateShaderFromFile(const char *filename);
BCShader * bcCreateShaderFromCode(const char *vsCode, const char *fsCode);
void bcDestroyShader(BCShader *shader);
void bcBindShader(BCShader *shader);

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
float * bcGetModelViewProjectionMatrix();

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

//
// bcgl_gfx_draw module
//

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
void bcPrepareScene3D(float fov);
void bcPrepareScene2D();
void bcPrepareSceneGUI();

// Draw 2D
void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh);
void bcDrawRect2D(float x, float y, float w, float h, bool fill);
void bcDrawCircle2D(float x, float y, float r, int segments, bool fill);
void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z);
void bcDrawGrid(int size_x, int size_y);
void bcDrawPlane(int size_x, int size_y);

//
// bcgl_gfx_font module
//

BCFont * bcCreateFontTTF(const char *filename, float height);
BCFont * bcCreateFontFNT(const char *filename);
BCFont * bcCreateFontBMP(const char *filename, int char_first, int char_count, int cols);
void bcDestroyFont(BCFont *font);
void bcDrawText(BCFont *font, float x, float y, const char *text);
void bcGetTextSize(BCFont *font, const char *text, float *px, float *py);

//
// bcgl_gfx_geometry module
//

BCMesh * bcCreateMeshFromShape(void *par_shape);
BCMesh * bcCreateMeshCube();
BCMesh * bcCreateMeshBox(float x1, float y1, float z1, float x2, float y2, float z2);
BCMesh * bcCreateCylinder(float radius, float height, int slices);
BCMesh * bcCreateMeshSphere(float radius, int slices, int stacks);
void bcTransformMesh(BCMesh *mesh, float *m);
void bcDumpMesh(BCMesh *mesh, FILE *stream);

//
// bcgl_gfx_model module
//

BCModel * bcCreateModel(BCMesh *mesh, BCMaterial material, int parts);
BCModel * bcCreateModelFromFile(const char *filename);
void bcDestroyModel(BCModel *model);
void bcDrawModel(BCModel *model);
void bcBeginModelDraw(BCModel *model);
void bcEndModelDraw(BCModel *model);
void bcDrawModelPart(BCModel *model, int part);
int bcGetModelPartByName(BCModel *model, const char *name);

//
// Enums
//

// Event Types
enum BCEventType
{
    BC_EVENT_KEYPRESS,
    BC_EVENT_KEYRELEASE,
    BC_EVENT_KEYREPEAT,
    BC_EVENT_MOUSEPRESS,
    BC_EVENT_MOUSERELEASE,
    BC_EVENT_MOUSEMOVE,
    BC_EVENT_MOUSEWHEEL,
    BC_EVENT_WINDOWSIZE,
    BC_EVENT_WINDOWFOCUS,
    BC_EVENT_WINDOWICONIFY,
    BC_EVENT_TOUCH_DOWN,
    BC_EVENT_TOUCH_UP,
    BC_EVENT_TOUCH_MOVE,
};

// Key Codes
enum BCKeyCode
{
    BC_KEY_SPACE,
    BC_KEY_APOSTROPHE,
    BC_KEY_COMMA,
    BC_KEY_MINUS,
    BC_KEY_PERIOD,
    BC_KEY_SLASH,
    BC_KEY_0,
    BC_KEY_1,
    BC_KEY_2,
    BC_KEY_3,
    BC_KEY_4,
    BC_KEY_5,
    BC_KEY_6,
    BC_KEY_7,
    BC_KEY_8,
    BC_KEY_9,
    BC_KEY_SEMICOLON,
    BC_KEY_EQUAL,
    BC_KEY_A,
    BC_KEY_B,
    BC_KEY_C,
    BC_KEY_D,
    BC_KEY_E,
    BC_KEY_F,
    BC_KEY_G,
    BC_KEY_H,
    BC_KEY_I,
    BC_KEY_J,
    BC_KEY_K,
    BC_KEY_L,
    BC_KEY_M,
    BC_KEY_N,
    BC_KEY_O,
    BC_KEY_P,
    BC_KEY_Q,
    BC_KEY_R,
    BC_KEY_S,
    BC_KEY_T,
    BC_KEY_U,
    BC_KEY_V,
    BC_KEY_W,
    BC_KEY_X,
    BC_KEY_Y,
    BC_KEY_Z,
    BC_KEY_LEFT_BRACKET,
    BC_KEY_BACKSLASH,
    BC_KEY_RIGHT_BRACKET,
    BC_KEY_GRAVE_ACCENT,
    BC_KEY_WORLD_1,
    BC_KEY_WORLD_2,
    BC_KEY_ESCAPE,
    BC_KEY_ENTER,
    BC_KEY_TAB,
    BC_KEY_BACKSPACE,
    BC_KEY_INSERT,
    BC_KEY_DELETE,
    BC_KEY_RIGHT,
    BC_KEY_LEFT,
    BC_KEY_DOWN,
    BC_KEY_UP,
    BC_KEY_PAGE_UP,
    BC_KEY_PAGE_DOWN,
    BC_KEY_HOME,
    BC_KEY_END,
    BC_KEY_CAPS_LOCK,
    BC_KEY_SCROLL_LOCK,
    BC_KEY_NUM_LOCK,
    BC_KEY_PRINT_SCREEN,
    BC_KEY_PAUSE,
    BC_KEY_F1,
    BC_KEY_F2,
    BC_KEY_F3,
    BC_KEY_F4,
    BC_KEY_F5,
    BC_KEY_F6,
    BC_KEY_F7,
    BC_KEY_F8,
    BC_KEY_F9,
    BC_KEY_F10,
    BC_KEY_F11,
    BC_KEY_F12,
    BC_KEY_F13,
    BC_KEY_F14,
    BC_KEY_F15,
    BC_KEY_F16,
    BC_KEY_F17,
    BC_KEY_F18,
    BC_KEY_F19,
    BC_KEY_F20,
    BC_KEY_F21,
    BC_KEY_F22,
    BC_KEY_F23,
    BC_KEY_F24,
    BC_KEY_F25,
    BC_KEY_KP_0,
    BC_KEY_KP_1,
    BC_KEY_KP_2,
    BC_KEY_KP_3,
    BC_KEY_KP_4,
    BC_KEY_KP_5,
    BC_KEY_KP_6,
    BC_KEY_KP_7,
    BC_KEY_KP_8,
    BC_KEY_KP_9,
    BC_KEY_KP_DECIMAL,
    BC_KEY_KP_DIVIDE,
    BC_KEY_KP_MULTIPLY,
    BC_KEY_KP_SUBTRACT,
    BC_KEY_KP_ADD,
    BC_KEY_KP_ENTER,
    BC_KEY_KP_EQUAL,
    BC_KEY_LEFT_SHIFT,
    BC_KEY_LEFT_CONTROL,
    BC_KEY_LEFT_ALT,
    BC_KEY_LEFT_SUPER,
    BC_KEY_RIGHT_SHIFT,
    BC_KEY_RIGHT_CONTROL,
    BC_KEY_RIGHT_ALT,
    BC_KEY_RIGHT_SUPER,
    BC_KEY_MENU,
    BC_KEY_COUNT,
};

#ifdef __cplusplus
}
#endif
