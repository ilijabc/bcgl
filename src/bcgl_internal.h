#pragma once

#include "bcgl.h"

#if defined(__MINGW32__) || defined(__CYGWIN__) || defined(_MSC_VER)
#ifdef SUPPORT_GLAD
#include <glad/glad.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#elif defined(__ANDROID__)
#include <GLES2/gl2.h>
#endif

void bcInitGfx();
void bcTermGfx();

void bcInitGfxDraw();
void bcTermGfxDraw();
