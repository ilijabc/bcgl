#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#ifdef SUPPORT_GLAD
#include "glad/src/glad.c"
#endif

#include "mathc/mathc.c"

#ifdef SUPPORT_PAR_SHAPES
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define PAR_SHAPES_IMPLEMENTATION
#include "par/par_shapes.h"
#endif
