#pragma once

#if defined(__ANDROID__)
    #include <GLES2/gl2.h>
#else
    #ifdef SUPPORT_GLAD
        #include <glad/glad.h>
    #else
        #include <GL/gl.h>
        #include <GL/glext.h>
    #endif
#endif
