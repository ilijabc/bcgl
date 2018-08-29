#ifdef SUPPORT_GLAD
#include "glad/src/glad.c"
#endif

#ifdef SUPPORT_PAR_SHAPES
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define PAR_SHAPES_IMPLEMENTATION
#include "par/par_shapes.h"
#endif
