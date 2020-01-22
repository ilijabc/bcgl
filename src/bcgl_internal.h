#pragma once

#include <bcgl.h>
#include <bcgl_opengl.h>

#define BC_MATRIX_STACK_SIZE 32

typedef struct
{
    int width;
    int height;
    void *nativeWindow;
} BCWindow;

//
// bcgl_app module
//

// App
void bcAppCreate();
void bcAppDestroy();
void bcAppConfig(BCConfig *config);
bool bcAppStart(BCConfig *config);
void bcAppStop();
void bcAppUpdate();
void bcAppMainLoop();

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
BCEvent * bcDequeueEvent();
void bcQueueEvent(BCEvent *event);
void bcSendEvent(int type, int id, int x, int y, void *data);
int bcPullEvents();
BCEvent * bcGetEvent(int index);
void bcFlushEvents();
void bcInputStateReset();

//
// bcgl_file module
//

void bcInitFiles(void *ctx);
void bcTermFiles();

//
// bcgl_gfx module
//

void bcCreateGfx();
void bcDestroyGfx();
void bcStartGfx();
void bcStopGfx();

//
// bcutils
//

char * __strdup(const char *str);

typedef struct clist_node
{
    void *data;
    struct clist_node *next;
} clist_node_t;

typedef struct clist
{
    clist_node_t *head;
    clist_node_t *tail;
} clist_t;

clist_node_t * clist_new_node(clist_t *list);
clist_node_t * clist_add_node(clist_t *list, void *data);
void clist_delete_node(clist_t *list, void *data);
void clist_clear(clist_t *list);
