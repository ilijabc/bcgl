#pragma once

#include <bcgl.h>
#include <bcgl_opengl.h>
#include <bcmath.h>

#define MATRIX_STACK_SIZE 32

typedef struct
{
    int width;
    int height;
    void *nativeWindow;
} BCWindow;

typedef struct
{
    BCColor BackgroundColor;
    BCMaterial DefaultMaterial;
    mat4_t ProjectionMatrix;
    mat4_t ModelViewMatrix;
    mat4_t ModelViewProjectionMatrix;
    BCMesh *CurrentMesh;
#ifdef SUPPORT_GLSL
    BCShader *DefaultShader;
    BCShader *CurrentShader;
#endif
    // draw
    BCMesh *TempMesh;
    vec4_t TempVertexData[VERTEX_ATTR_MAX];
    int VertexCounter;
    int IndexCounter;
    enum BCDrawMode DrawMode;
    mat4_t MatrixStack[MATRIX_STACK_SIZE];
    int MatrixCounter;
    BCMesh *ReusableSolidMesh;
    BCMesh *ReusableCubeMesh;
    BCModel *CurrentModel;
} BCContext;

// Global GFX context
extern BCContext *g_Context;

//
// bcgl_app module
//

BCCallbacks bcGetCallbacks();

// App
bool bcAppWrapperStart(BCConfig *config);
int bcAppWrapperStop();
bool bcAppWrapperIsRunning();
void bcAppWrapperUpdate();
int bcAppWrapperRun(BCConfig *config);

// Window
BCWindow * bcCreateWindow(BCConfig *config);
void bcDestroyWindow(BCWindow *window);
void bcUpdateWindow(BCWindow *window);
void bcCloseWindow(BCWindow *window);
bool bcIsWindowOpened(BCWindow *window);
void bcPullWindowEvents(BCWindow *window);
BCWindow * bcGetWindow();
void bcSetWindow(BCWindow *window);

// Events
BCEvent * bcSendEvent(int type, int id, int x, int y);
int bcPullEvents();
BCEvent * bcGetEvent(int index);
void bcFlushEvents();

//
// bcgl_files module
//

void bcInitFiles(void *ctx);
void bcTermFiles();

//
// bcgl_gfx module
//

void bcInitGfx();
void bcTermGfx();
