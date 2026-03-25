#pragma once

#if defined(SUPPORT_GLES)
    #include <GLES2/gl2.h>
#elif defined(SUPPORT_GLAD)
    #include <glad/glad.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif
