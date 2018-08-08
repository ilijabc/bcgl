#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>

// Texture params
#define TEXTURE_PARAM_LINEAR    0x1
#define TEXTURE_PARAM_NEAREST   0x2
#define TEXTURE_PARAM_MIPMAP    0x4
#define TEXTURE_PARAM_REPEAT    0x8
#define TEXTURE_PARAM_CLAMP     0x10

typedef struct
{
    const char *title;
    int width;
    int height;
    int mode;
    int vsync;
    int msaa;
} BCConfig;

typedef struct
{
    int width;
    int height;
    void *nativeWindow;
} BCWindow;

typedef struct
{
    int type;
    int x;
    int y;
} BCEvent;

typedef struct
{
    float r, g, b, a;
} BCColor;

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

typedef int BCFont;
typedef int BCShader;
typedef int BCMesh;

#define NEW_OBJECT(T) (T*)calloc(1, sizeof(T));
#define DELETE_OBJECT(obj) free(obj);

#ifdef __ANDROID__
#define bcLog(format, ...) __android_log_print(ANDROID_LOG_INFO, "UCPlayer", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#else
#define bcLog(format, ...) { printf("[%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  fflush(stdout); }
#endif

//
// !!! OVERRIDE THIS IN APP !!!
//
#ifdef BC_MAIN_LOOP
void BC_onConfig(BCConfig *config);
void BC_onStart();
void BC_onStop();
void BC_onEvent(int event, int x, int y);
void BC_onUpdate(float dt);
#endif

//
// Core module
//

// Window
BCWindow * bcCreateWindow(BCConfig *config);
void bcDestroyWindow(BCWindow *window);
void bcUpdateWindow(BCWindow *window);
void bcCloseWindow(BCWindow *window);
bool bcIsWindowOpened(BCWindow *window);
BCWindow * bcGetWindow();

// App
bool bcInit();
void bcTerm();
void bcQuit(int code);
BCEvent * bcSendEvent(int type, int x, int y);
int bcPullEvents();
BCEvent * bcGetEvent(int index);
float bcGetTime();

//
// GL module
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
void bcDrawTexture(BCTexture *texture);

// Font
BCFont * bcCreateFontFromFile(const char *filename);
BCFont * bcCreateFontFromMemory(void *buffer);
void bcDestroyFont(BCFont *font);

// Shader
BCShader * bcCreateShaderFromFile(const char *filename);
BCShader * bcCreateShaderFromCode(const char *code);
void bcDestroyShader(BCShader *shader);

// View State
void bcInitGL();
void bcClear();
void bcSetColor(BCColor color);
void bcSetBlend(bool enable);
void bcSetDepthTest(bool enable);

// Matrix Stack
void bcSetPerspective(float fovy, float aspect, float znear, float zfar);
void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar);
void bcPushMatrix();
void bcPopMatrix();
void bcIdentity();
void bcTranslatef(float x, float y, float z);
void bcRotatef(float angle, float x, float y, float z);
void bcScalef(float x, float y, float z);

// Camera
void bcPrepareScene3D();
void bcEndScene3D();
void bcPrepareScene2D();
void bcEndScene2D();
void bcPrepareSceneGUI();
void bcEndSceneGUI();

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


//
// Enums
//

// Event Types
enum BCEventType
{
    BC_EVENT_KEYPRESS,
    BC_EVENT_KEYRELEASE,
    BC_EVENT_MOUSEPRESS,
    BC_EVENT_MOUSERELEASE,
    BC_EVENT_MOUSEMOVE,
    BC_EVENT_MOUSEWHEEL,
    BC_EVENT_WINDOWSIZE,
    BC_EVENT_WINDOWFOCUS,
    BC_EVENT_WINDOWICONIFY,
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
