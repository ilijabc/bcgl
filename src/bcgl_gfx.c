#include "bcgl_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#ifdef SUPPORT_PAR_SHAPES
// this is a fix for already defined ARRAYSIZE in winnt.h
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define PAR_SHAPES_IMPLEMENTATION
#include "par/par_shapes.h"
#endif

#define DEBUG_SHADER 0

// RM
#define RM_TYPE_SHADER      0
#define RM_TYPE_TEXTURE     1
#define RM_TYPE_MESH        2

//
// Context
//

typedef struct
{
    bool Started;
    mat4_t ProjectionMatrix;
    mat4_t ModelViewMatrix;
    mat3_t TextureMatrix;
    BCColor ColorArray[BC_COLOR_TYPE_MAX];
    bool ColorNeedUpdate;
    BCMesh *CurrentMesh;
#ifdef SUPPORT_GLSL
    BCShader *DefaultShader;
    BCShader *CurrentShader;
#endif
    bool LightingEnabled;
    // draw
    BCMesh *TempMesh;
    vec4_t TempVertexData[BC_VERTEX_ATTR_MAX];
    int VertexCounter;
    int IndexCounter;
    BCDrawMode DrawMode;
    mat4_t MatrixStack[BC_MATRIX_STACK_SIZE];
    int MatrixCounter;
    BCMesh *ReusableSolidMesh;
    BCMesh *ReusableCubeMesh;
    BCMesh *ReusableWireCubeMesh;
    BCMesh *ReusablePlaneMesh;
    clist_t *RM_list;
} BCContext;

static BCContext *g_Context = NULL;

//
// Shader
//

#ifdef SUPPORT_GLSL

#if defined(__EMSCRIPTEN__)
    #define GLSL_VERSION \
    "#version 100\n" \
    "precision highp float;\n"
#elif defined(__ANDROID__)
    #define GLSL_VERSION \
    "#version 100\n" \
    "precision highp float;\n"
#else
    #define GLSL_VERSION \
    "#version 120\n"
#endif

static struct { const char * name; int value; } s_DefaultShaderConstInts[] =
{
    { "COLOR_PRIMARY", BC_COLOR_TYPE_PRIMARY },
    { "COLOR_SECONDARY", BC_COLOR_TYPE_SECONDARY },
    { "COLOR_DIFFUSE", BC_COLOR_TYPE_DIFFUSE },
    { "COLOR_AMBIENT", BC_COLOR_TYPE_AMBIENT },
    { "COLOR_SPECULAR", BC_COLOR_TYPE_SPECULAR },
    { "COLOR_EMISSION", BC_COLOR_TYPE_EMISSION },
    { "COLOR_CUSTOM_1", BC_COLOR_TYPE_CUSTOM_1 },
    { "COLOR_CUSTOM_2", BC_COLOR_TYPE_CUSTOM_2 },
    { "COLOR_CUSTOM_3", BC_COLOR_TYPE_CUSTOM_3 },
    { "COLOR_CUSTOM_4", BC_COLOR_TYPE_CUSTOM_4 },
    { "COLOR_MAX", BC_COLOR_TYPE_MAX },
    { NULL, 0 }
};

// This must be alligned with @BCVertexAttributes
static BCShaderVar s_DefaultShaderAttributes[] =
{
    { "vec3", "a_Position", 1 },
    { "vec3", "a_Normal", 1 },
    { "vec2", "a_TexCoord", 1 },
    { "vec4", "a_Color", 1 },
    { NULL, NULL }
};

// This must be alligned with @BCShaderUniforms
static BCShaderVar s_DefaultShaderUniforms[] =
{
    { "mat4", "u_ProjectionMatrix", 1 },
    { "mat4", "u_ModelViewMatrix", 1 },
    { "mat3", "u_TextureMatrix", 1 },
    { "sampler2D", "u_Texture", 1 },
    { "bool", "u_UseTexture", 1 },
    { "bool", "u_AlphaOnlyTexture", 1 },
    { "bool", "u_AlphaTest", 1 },
    { "bool", "u_VertexColorEnabled", 1 },
    { "vec4", "u_ColorArray", BC_COLOR_TYPE_MAX },
    { "bool", "u_LightEnabled", 1 },
    { "vec3", "u_LightPosition", 1 },
    { "vec4", "u_LightColor", 1 },
    { NULL, NULL }
};

static BCShaderVar s_DefaultShaderVars[] =
{
    { "vec3", "v_position", 1 },
    { "vec3", "v_normal", 1 },
    { "vec2", "v_texCoord", 1 },
    { "vec4", "v_color", 1 },
    { NULL, NULL }
};

#define STRINGIFY(s) #s

// vertex code string
static const char s_DefaultShaderVertexCode[] = STRINGIFY(
void main()
{
    v_position = (u_ModelViewMatrix * vec4(a_Position, 1)).xyz;
    v_normal = (u_ModelViewMatrix * vec4(a_Normal, 0)).xyz;
    v_texCoord = (u_TextureMatrix * vec3(a_TexCoord, 1)).xy;
    v_color = a_Color;
    gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4(a_Position, 1);
}
);

// fragment code string
static const char s_DefaultShaderFragmentCode[] = STRINGIFY(
void main()
{
    gl_FragColor = u_VertexColorEnabled ? v_color : u_ColorArray[COLOR_PRIMARY];
    if (u_UseTexture)
    {
        vec4 tex = texture2D(u_Texture, v_texCoord);
        if (u_AlphaTest && tex.a < 0.1)
            discard;
        if (u_AlphaOnlyTexture)
            tex = vec4(1, 1, 1, tex.a);
        gl_FragColor *= tex;
    }
    if (u_LightEnabled)
    {
        vec3 norm = normalize(v_normal);
        vec3 lightDir = normalize(u_LightPosition - v_position);
        float diff = max(dot(norm, lightDir), 0.0);
        vec4 diffuse = diff * u_ColorArray[COLOR_DIFFUSE];
        gl_FragColor *= (u_ColorArray[COLOR_AMBIENT] + diffuse);
    }
}
);

#else // SUPPORT_GLSL

// This must be alligned with @BCVertexAttributes
static struct
{
    BCVertexAttributes index;
    int type;
} const s_ClientStateType[] =
{
    { BC_VERTEX_ATTR_POSITIONS, GL_VERTEX_ARRAY },
    { BC_VERTEX_ATTR_NORMALS, GL_NORMAL_ARRAY },
    { BC_VERTEX_ATTR_TEXCOORDS, GL_TEXTURE_COORD_ARRAY },
    { BC_VERTEX_ATTR_COLORS, GL_COLOR_ARRAY },
    { BC_VERTEX_ATTR_MAX, -1 }
};

#endif // SUPPORT_GLSL

typedef struct
{
    uint8_t signature[4];
    uint32_t version;
    uint32_t size;
    uint32_t reserved;
    uint32_t format;
    uint32_t type;
    uint32_t total_comps;
    uint32_t num_vertices;
    uint32_t num_indices;
} BCMeshFileHeader;

static const char s_MeshFileSignature[4] = { 'B', 'C', 'M', 'D' };
static const uint32_t s_MeshFileVersion = 1;

//
// Init
//

void bcCreateGfx()
{
    g_Context = NEW_OBJECT(BCContext);
    g_Context->RM_list = NEW_OBJECT(clist_t);
}

void bcDestroyGfx()
{
    if (g_Context->ReusableSolidMesh)
    {
        bcDestroyMesh(g_Context->ReusableSolidMesh);
        g_Context->ReusableSolidMesh = NULL;
    }
    if (g_Context->ReusableCubeMesh)
    {
        bcDestroyMesh(g_Context->ReusableCubeMesh);
        g_Context->ReusableCubeMesh = NULL;
    }
    if (g_Context->ReusableWireCubeMesh)
    {
        bcDestroyMesh(g_Context->ReusableWireCubeMesh);
        g_Context->ReusableWireCubeMesh = NULL;
    }
    if (g_Context->ReusablePlaneMesh)
    {
        bcDestroyMesh(g_Context->ReusablePlaneMesh);
        g_Context->ReusablePlaneMesh = NULL;
    }
#ifdef SUPPORT_GLSL
    bcDestroyShader(g_Context->DefaultShader);
    g_Context->DefaultShader = NULL;
#endif
    free(g_Context->RM_list);
    free(g_Context);
    g_Context = NULL;
}

void bcStartGfx()
{
    bcLog("OpenGL: %s", glGetString(GL_VERSION));
    bcLog("Device: %s", glGetString(GL_RENDERER));
    bcLog("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    // init context
    g_Context->ColorArray[BC_COLOR_TYPE_PRIMARY] = SET_COLOR(1, 1, 1, 1);
    g_Context->ColorArray[BC_COLOR_TYPE_SECONDARY] = SET_COLOR(1, 1, 1, 1);
    g_Context->ColorArray[BC_COLOR_TYPE_DIFFUSE] = SET_COLOR(0.8f, 0.8f, 0.8f, 1);
    g_Context->ColorArray[BC_COLOR_TYPE_AMBIENT] = SET_COLOR(0.2f, 0.2f, 0.2f, 1);
    g_Context->ColorArray[BC_COLOR_TYPE_SPECULAR] = SET_COLOR(0, 0, 0, 1);
    g_Context->ColorArray[BC_COLOR_TYPE_EMISSION] = SET_COLOR(0, 0, 0, 1);
    g_Context->ColorNeedUpdate = true;
    g_Context->VertexCounter = -1;
    g_Context->IndexCounter = -1;
#ifdef SUPPORT_GLSL
    if (!g_Context->DefaultShader)
    {
        g_Context->DefaultShader = bcCreateShader(s_DefaultShaderVertexCode, s_DefaultShaderFragmentCode);
    }
#else
    glAlphaFunc(GL_GREATER, 0.1f);
    glDisable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
#endif
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    // gl default
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    // RM
    for (clist_node_t *node = g_Context->RM_list->head; node; node = node->next)
    {
        uint8_t *p_type = (uint8_t*) node->data;
        switch (*p_type)
        {
        case RM_TYPE_SHADER:
            bcUpdateShader((BCShader*) node->data);
            break;
        case RM_TYPE_TEXTURE:
            bcUpdateTexture((BCTexture*) node->data);
            break;
        case RM_TYPE_MESH:
            bcUpdateMesh((BCMesh*) node->data);
            break;
        }
    }
    g_Context->Started = true;
    bcBindShader(NULL);
}

void bcStopGfx()
{
    for (clist_node_t *node = g_Context->RM_list->head; node; node = node->next)
    {
        uint8_t *p_type = (uint8_t*) node->data;
        switch (*p_type)
        {
        case RM_TYPE_SHADER:
            bcReleaseShader((BCShader*) node->data);
            break;
        case RM_TYPE_TEXTURE:
            bcReleaseTexture((BCTexture*) node->data);
            break;
        case RM_TYPE_MESH:
            bcReleaseMesh((BCMesh*) node->data);
            break;
        }
    }
    g_Context->Started = false;
}

//
// Shader
//

#ifdef SUPPORT_GLSL

BCShader * bcCreateShaderFromSingleFile(const char *filename)
{
    char *code = bcLoadTextFile(filename, NULL);
    if (code == NULL)
        return NULL;
    BCShader *shader = bcCreateShader(code, code);
    free(code);
    return shader;
}

BCShader * bcCreateShaderFromFile(const char *vsFilename, const char *fsFilename)
{
    char *vsCode = bcLoadTextFile(vsFilename, NULL);
    if (vsCode == NULL)
    {
        return NULL;
    }
    char *fsCode = bcLoadTextFile(fsFilename, NULL);
    if (fsCode == NULL)
    {
        free(vsCode);
        return NULL;
    }
    BCShader *shader = bcCreateShader(vsCode, fsCode);
    free(vsCode);
    free(fsCode);
    return shader;
}

BCShader * bcCreateShader(const char *vs_code, const char *fs_code)
{
    BCShader *shader = NEW_OBJECT(BCShader);
    shader->RM_type = RM_TYPE_SHADER;
    shader->vs_code = cstr_strdup(vs_code);
    shader->fs_code = cstr_strdup(fs_code);
    if (g_Context->Started && !bcUpdateShader(shader))
    {
        bcDestroyShader(shader);
        free(shader);
        shader = NULL;
    }
    clist_add_node(g_Context->RM_list, shader);
    return shader;
}

bool bcUpdateShader(BCShader *shader)
{
    // vertex shaders
    shader->vs_id = bcLoadShader(shader->vs_code, GL_VERTEX_SHADER);
    if (shader->vs_id == 0)
    {
        return false;
    }
    // fragment shader
    shader->fs_id = bcLoadShader(shader->fs_code, GL_FRAGMENT_SHADER);
    if (shader->fs_id == 0)
    {
        return false;
    }
    // create program
    shader->programId = glCreateProgram();
    if (shader->programId == 0)
    {
        bcLogError("Error creating shader program!");
        return false;
    }
    glAttachShader(shader->programId, shader->vs_id);
    glAttachShader(shader->programId, shader->fs_id);
    // bind attributes
    for (int i = 0; i < BC_VERTEX_ATTR_MAX; i++)
    {
        glBindAttribLocation(shader->programId, i, s_DefaultShaderAttributes[i].name);
    }
    if (!bcLinkShaderProgram(shader->programId))
    {
        return false;
    }
    // get uniforms
    for (int i = 0; i < BC_SHADER_UNIFORM_MAX; i++)
    {
        shader->loc_uniforms[i] = glGetUniformLocation(shader->programId, s_DefaultShaderUniforms[i].name);
        if (shader->loc_uniforms[i] == -1)
            bcLogWarning("Shader uniform '%s' not found!", s_DefaultShaderUniforms[i].name);
    }
    return shader;
}


void bcReleaseShader(BCShader *shader)
{
    if (!shader)
    {
        bcLogError("Invalid shader!");
        return;
    }
    if (shader->vs_id)
        glDeleteShader(shader->vs_id);
    if (shader->fs_id)
        glDeleteShader(shader->fs_id);
    glDeleteProgram(shader->programId);
}

void bcDestroyShader(BCShader *shader)
{
    if (!shader)
    {
        bcLogError("Invalid shader!");
        return;
    }
    bcReleaseShader(shader);
    free(shader->vs_code);
    free(shader->fs_code);
    free(shader);
    clist_delete_node(g_Context->RM_list, shader);
}

void bcBindShader(BCShader *shader)
{
    if (shader == NULL)
        shader = g_Context->DefaultShader;
    g_Context->CurrentShader = shader;
    glUseProgram(shader->programId);
    // applyProjectionMatrix();
    // applyCurrentMatrix();
}

unsigned int bcLoadShader(const char *code, unsigned int shaderType)
{
    const char *type_str = (shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
    // generated code
    char generated_code[2000];
    // defines
    sprintf(generated_code, "#define %s\n", type_str);
    for (int i = 0; s_DefaultShaderConstInts[i].name; i++)
    {
        sprintf(generated_code, "%s#define %s %d\n", generated_code, s_DefaultShaderConstInts[i].name, s_DefaultShaderConstInts[i].value);
    }
    // attributes
    if (shaderType == GL_VERTEX_SHADER)
    {
        for (int i = 0; s_DefaultShaderAttributes[i].name; i++)
        {
            sprintf(generated_code, "%sattribute %s %s;\n", generated_code, s_DefaultShaderAttributes[i].type, s_DefaultShaderAttributes[i].name);
        }
    }
    // uniforms
    for (int i = 0; s_DefaultShaderUniforms[i].name; i++)
    {
        if (s_DefaultShaderUniforms[i].size > 1)
            sprintf(generated_code, "%suniform %s %s[%d];\n", generated_code, s_DefaultShaderUniforms[i].type, s_DefaultShaderUniforms[i].name, s_DefaultShaderUniforms[i].size);
        else
            sprintf(generated_code, "%suniform %s %s;\n", generated_code, s_DefaultShaderUniforms[i].type, s_DefaultShaderUniforms[i].name);
    }
    // varyings
    for (int i = 0; s_DefaultShaderVars[i].name; i++)
    {
        sprintf(generated_code, "%svarying %s %s;\n", generated_code, s_DefaultShaderVars[i].type, s_DefaultShaderVars[i].name);
    }
    // init shader source
    const char *strings[3] = { GLSL_VERSION,  generated_code, code };
    int lengths[3] = { (int) strlen(GLSL_VERSION), (int) strlen(generated_code), (int) strlen(code) };
#if DEBUG_SHADER
    // print shader code
    printf("\n>>> SHADER type=%d <<<\n%s%s%s\n>>> END <<<\n", strings[0], strings[1], strings[2]);
#endif
    // create and compile
    GLuint shaderId = glCreateShader(shaderType);
    if (shaderId == 0)
    {
        bcLogError("glCreateShader failed!");
        return 0;
    }
    glShaderSource(shaderId, 3, strings, lengths);
    glCompileShader(shaderId);
    GLint compileError = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileError);
    // parse error log
    char errorLog[2000];
    GLsizei errorLen = 0;
    glGetShaderInfoLog(shaderId, 2000, &errorLen, errorLog);
    if (errorLen)
    {
        if (compileError == GL_FALSE)
        {
            bcLogError("Compile %s shader:\n%s", type_str, errorLog);
        }
        else
        {
            bcLogWarning("Compile %s shader:\n%s", type_str, errorLog);
        }
    }
    if (compileError == GL_FALSE)
    {
        glDeleteShader(shaderId);
        bcLogError("Shader not created!");
        return 0;
    }
    return shaderId;
}

bool bcLinkShaderProgram(unsigned int programId)
{
    // link program
    glLinkProgram(programId);
    GLint linkError = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkError);
    if (linkError == GL_FALSE) {
        char errorLog[2000];
        GLsizei errorLen = 0;
        glGetProgramInfoLog(programId, 2000, &errorLen, errorLog);
        bcLogError("Link program:\n%s", errorLog);
        return false;
    }
    // validate program
    glValidateProgram(programId);
    GLint validateError = 0;
    glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateError);
    if (validateError == GL_FALSE) {
        char errorLog[2000];
        GLsizei errorLen = 0;
        glGetProgramInfoLog(programId, 2000, &errorLen, errorLog);
        bcLogError("Validate program:\n%s", errorLog);
        return false;
    }
    return true;
}

#else // SUPPORT_GLSL

BCShader * bcCreateShader(const char *vs_code, const char *fs_code) { return NULL; }
BCShader * bcCreateShaderFromSingleFile(const char *filename) { return NULL; }
BCShader * bcCreateShaderFromFile(const char *vsFilename, const char *fsFilename) { return NULL; }
void bcUpdateShader(BCShader *shader) { }
void bcReleaseShader(BCShader *shader) { }
void bcDestroyShader(BCShader *shader) { }
void bcBindShader(BCShader *shader) { }
unsigned int bcLoadShader(const char *code, unsigned int shaderType) { return 0; }
bool bcLinkShaderProgram(unsigned int programId) { return false; }

#endif // SUPPORT_GLSL

//
// Image
//

BCImage * bcCreateImage(int width, int height, int comps)
{
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = width;
    image->height = height;
    image->comps = comps;
    image->data = calloc(1, width * height * comps);
    return image;
}

BCImage * bcCreateImageFromFile(const char *filename)
{
#if 0
    int x, y, comp;
    unsigned char *data = stbi_load(filename, &x, &y, &comp, 0);
    if (data == NULL)
    {
        bcLogWarning("Image file '%s' not found!", filename);
        return NULL;
    }
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = x;
    image->height = y;
    image->comps = comp;
    image->data = data;
    return image;
#else
    int size = 0;
    unsigned char *data = bcLoadDataFile(filename, &size);
    if (data == NULL)
    {
        bcLogWarning("Image file '%s' not found!", filename);
        return NULL;
    }
    if (size <= 0)
    {
        bcLogWarning("Image file '%s' not valid!", filename);
        return NULL;
    }
    return bcCreateImageFromMemory(data, size);
#endif
}

BCImage * bcCreateImageFromMemory(void *buffer, int size)
{
    int x, y, comp;
    unsigned char *data = stbi_load_from_memory(buffer, size, &x, &y, &comp, 0);
    if (data == NULL)
    {
        bcLogWarning("Image data not valid!");
        return NULL;
    }
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = x;
    image->height = y;
    image->comps = comp;
    image->data = data;
    return image;
}

void bcDestroyImage(BCImage *image)
{
    free(image->data);
    free(image);
}

//
// Texture
//

BCTexture * bcCreateTextureFromFile(const char *filename, int flags)
{
    BCImage *image = bcCreateImageFromFile(filename);
    if (image == NULL)
        return NULL;
    return bcCreateTextureFromImage(image, flags);
}

BCTexture * bcCreateTextureFromImage(BCImage *image, int flags)
{
    BCTexture *texture = NEW_OBJECT(BCTexture);
    texture->RM_type = RM_TYPE_TEXTURE;
    texture->width = image->width;
    texture->height = image->height;
    texture->image = image;
    texture->flags = flags;
    if (g_Context->Started)
    {
        bcUpdateTexture(texture);
    }
    if (flags & BC_TEXTURE_DETACHED)
    {
        bcDestroyImage(image);
        texture->image = NULL;
    }
    clist_add_node(g_Context->RM_list, texture);
    return texture;
}

void bcUpdateTexture(BCTexture *texture)
{
    int internalFormat;
    switch (texture->image->comps)
    {
    case 1:
        internalFormat = GL_ALPHA;
        texture->format = GL_ALPHA;
        break;
    case 3:
        internalFormat = GL_RGB;
        texture->format = GL_RGB;
        break;
    case 4:
        internalFormat = GL_RGBA;
        texture->format = GL_RGBA;
        break;
    }
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &(texture->id));
    glBindTexture(GL_TEXTURE_2D, (texture->id));
    // filter flags
    if (texture->flags & BC_TEXTURE_LINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (texture->flags & BC_TEXTURE_NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (texture->flags & BC_TEXTURE_MIPMAP)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        // mMipmaps = true;
    }
    else
    {
        // linear filter by default
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // wrap flags
    if (texture->flags & BC_TEXTURE_REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if (texture->flags & BC_TEXTURE_CLAMP)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        texture->image->width,
        texture->image->height,
        0,
        texture->format,
        GL_UNSIGNED_BYTE,
        texture->image->data);
    // if (mMipmaps) {
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void bcReleaseTexture(BCTexture *texture)
{
    if (!texture)
    {
        bcLogError("Invalid texture!");
        return;
    }
    glDeleteTextures(1, &(texture->id));
    texture->id = 0;
}

void bcDestroyTexture(BCTexture *texture)
{
    if (!texture)
    {
        bcLogError("Invalid texture!");
        return;
    }
    if (texture->image)
        bcDestroyImage(texture->image);
    bcReleaseTexture(texture);
    free(texture);
    clist_delete_node(g_Context->RM_list, texture);
}

void bcBindTexture(BCTexture *texture)
{
    glActiveTexture(GL_TEXTURE0);
    if (texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture->id);
    }
#ifdef SUPPORT_GLSL
    glUniform1i(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_USETEXTURE], texture ? 1 : 0);
    if (texture)
    {
        glUniform1i(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_TEXTURE], 0);
        glUniform1i(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_ALPHAONLYTEXTURE], texture->format == GL_ALPHA ? 1 : 0);
    }
#else
    if (texture)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
#endif
}

//
// View State
//

void bcClear(BCColor color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, bcGetDisplayWidth(), bcGetDisplayHeight());
}

void bcViewport(int x, int y, int width, int height)
{
    if (width == 0)
        width = bcGetDisplayWidth();
    if (height == 0)
        height = bcGetDisplayHeight();
    glViewport(x, bcGetDisplayHeight() - height, width, height);
}

void bcSetBlend(bool enabled)
{
    if (enabled)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void bcSetDepthTest(bool enabled)
{
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void bcSetAlphaTest(bool enabled)
{
#ifdef SUPPORT_GLSL
    glUniform1i(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_ALPHATEST], enabled);
#else
    if (enabled)
        glEnable(GL_ALPHA_TEST);
    else
        glDisable(GL_ALPHA_TEST);
#endif
}

void bcSetCulling(bool enabled)
{
    if (enabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

void bcSetWireframe(bool enabled)
{
#ifndef SUPPORT_GLES
    if (enabled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

void bcSetLighting(bool enabled)
{
#ifdef SUPPORT_GLSL
    glUniform1i(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_LIGHT_ENABLED], enabled);
    if (enabled)
    {
        glUniform4f(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_LIGHT_COLOR], 1, 1, 1, 1);
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
#else
    if (enabled)
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }
#endif
    g_Context->LightingEnabled = enabled;
}

void bcLightPosition(float x, float y, float z)
{
#ifdef SUPPORT_GLSL
    glUniform3f(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_LIGHT_POSITION], x, y, z);
#else
    float lightPos[] = { x, y, z, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
#endif
}

void bcSetProjectionMatrix(float *m)
{
    g_Context->ProjectionMatrix = mat4_from_array(m);
#ifdef SUPPORT_GLSL
    glUniformMatrix4fv(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_PROJECTIONMATRIX], 1, GL_FALSE, m);
#else
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m);
    glMatrixMode(GL_MODELVIEW);
#endif
}

void bcSetModelViewMatrix(float *m)
{
    g_Context->ModelViewMatrix = mat4_from_array(m);
#ifdef SUPPORT_GLSL
    glUniformMatrix4fv(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_MODELVIEWMATRIX], 1, GL_FALSE, m);
#else
    glLoadMatrixf(m);
#endif
}

void bcSetTextureMatrix(float *m)
{
    g_Context->TextureMatrix = mat3_from_array(m);
#ifdef SUPPORT_GLSL
    glUniformMatrix3fv(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_TEXTUREMATRIX], 1, GL_FALSE, m);
#else
    // TODO: not implemented for OpenGL legacy mode!
#endif
}

float * bcGetProjectionMatrix()
{
    return g_Context->ProjectionMatrix.v;
}

float * bcGetModelViewMatrix()
{
    return g_Context->ModelViewMatrix.v;
}

float * bcGetTextureMatrix()
{
    return g_Context->TextureMatrix.v;
}

void bcSetScissor(bool enabled)
{
    if (enabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

void bcScissorRect(int x, int y, int w, int h)
{
    y = bcGetDisplayHeight() - y - h;
    glScissor(x, y, w, h);
}

void bcSetColor(BCColor color, BCColorType type)
{
    g_Context->ColorArray[type] = color;
    g_Context->ColorNeedUpdate = true;
}

//
// Mesh
//

BCMesh * bcCreateMesh(int format, const float *vert_data, int vert_num, const uint16_t *indx_data, int indx_num, BCMeshType type)
{
    if (type == BC_MESH_OPTIMIZED && !vert_data)
    {
        bcLogError("BC_MESH_OPTIMIZED must have data!");
        return NULL;
    }
    BCMesh *mesh = NEW_OBJECT(BCMesh);
    mesh->RM_type = RM_TYPE_MESH;
    mesh->num_vertices = vert_num;
    mesh->num_indices = indx_num;
    mesh->format = format;
    mesh->type = type;
    // calculate components
    mesh->comps[BC_VERTEX_ATTR_POSITIONS] =
        (format & BC_MESH_POS2) ? 2 :
        (format & BC_MESH_POS3) ? 3 :
        (format & BC_MESH_POS4) ? 4 :
        0;
    mesh->comps[BC_VERTEX_ATTR_TEXCOORDS] =
        (format & BC_MESH_TEX2) ? 2 :
        (format & BC_MESH_TEX3) ? 3 :
        0;
    mesh->comps[BC_VERTEX_ATTR_NORMALS] =
        (format & BC_MESH_NORM) ? 3 :
        0;
    mesh->comps[BC_VERTEX_ATTR_COLORS] =
        (format & BC_MESH_COL1) ? 1 :
        (format & BC_MESH_COL3) ? 3 :
        (format & BC_MESH_COL4) ? 4 :
        0;
    for (int i = 0; i < BC_VERTEX_ATTR_MAX; i++)
    {
        mesh->total_comps += mesh->comps[i];
    }
    if (mesh->type == BC_MESH_OPTIMIZED)
    {
        // init VBOs
        if (mesh->num_vertices)
        {
            glGenBuffers(1, &(mesh->vbo_vertices));
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
            glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * mesh->total_comps * sizeof(float), vert_data, GL_STATIC_DRAW);
        }
        // init vbo_indices
        if (mesh->num_indices)
        {
            glGenBuffers(1, &(mesh->vbo_indices));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(uint16_t), indx_data, GL_STATIC_DRAW);
        }
    }
    else
    {
        // allocate vertex and index data holders
        if (mesh->num_vertices)
        {
            mesh->vertices = NEW_ARRAY(mesh->num_vertices * mesh->total_comps, float);
            if (vert_data)
                memcpy(mesh->vertices, vert_data, mesh->num_vertices * mesh->total_comps * sizeof(float));
        }
        if (mesh->num_indices)
        {
            mesh->indices = NEW_ARRAY(mesh->num_indices, uint16_t);
            if (indx_data)
                memcpy(mesh->indices, indx_data, mesh->num_indices * sizeof(uint16_t));
        }
        // update VBOs
        if (g_Context->Started && (vert_data || indx_data))
        {
            bcUpdateMesh(mesh);
        }
    }
    // draw params
    mesh->draw_mode = GL_TRIANGLES;
    mesh->draw_count = (mesh->num_indices > 0) ? mesh->num_indices : mesh->num_vertices;
    clist_add_node(g_Context->RM_list, mesh);
    return mesh;
}

BCMesh * bcCopyMesh(BCMesh *src)
{
    if (src == NULL)
    {
        bcLogError("Invalid mesh!");
        return NULL;
    }
    BCMesh *mesh = bcCreateMesh(src->format, src->vertices, src->num_vertices, src->indices, src->num_indices, src->type);
    if (src->num_vertices)
    {
        memcpy(mesh->vertices, src->vertices, src->num_vertices * src->total_comps * sizeof(float));
    }
    if (src->num_indices)
    {
        memcpy(mesh->indices, src->indices, src->num_indices * sizeof(uint16_t));
    }
    return mesh;
}

void bcUpdateMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    if (mesh->type == BC_MESH_OPTIMIZED)
    {
        bcLogWarning("Can't update BC_MESH_OPTIMIZED");
        return;
    }
    if (mesh->type == BC_MESH_NO_VBO)
    {
        bcLogWarning("Can't update BC_MESH_NO_VBO");
        return;
    }
    if (mesh->type == BC_MESH_STATIC)
    {
        if (mesh->vbo_vertices)
        {
            glDeleteBuffers(1, &(mesh->vbo_vertices));
            mesh->vbo_vertices = 0;
        }
        if (mesh->vbo_indices)
        {
            glDeleteBuffers(1, &(mesh->vbo_indices));
            mesh->vbo_indices = 0;
        }
    }
    if (mesh->num_vertices)
    {
        if (mesh->vbo_vertices)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
            glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->num_vertices * mesh->total_comps * sizeof(float), mesh->vertices);
        }
        else
        {
            glGenBuffers(1, &(mesh->vbo_vertices));
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
            glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * mesh->total_comps * sizeof(float),
                mesh->vertices,
                (mesh->type == BC_MESH_STATIC) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        }
    }
    if (mesh->num_indices)
    {
        if (mesh->vbo_indices)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mesh->num_indices * sizeof(uint16_t), mesh->indices);
        }
        else
        {
            glGenBuffers(1, &(mesh->vbo_indices));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(uint16_t),
                mesh->indices,
                (mesh->type == BC_MESH_STATIC) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        }
    }
    // reset state machine
    if (g_Context->CurrentMesh)
    {
        bcBindMesh(NULL);
    }
}

void bcReleaseMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    if (mesh->vbo_vertices)
    {
        glDeleteBuffers(1, &(mesh->vbo_vertices));
        mesh->vbo_vertices = 0;
    }
    if (mesh->vbo_indices)
    {
        glDeleteBuffers(1, &(mesh->vbo_indices));
        mesh->vbo_indices = 0;
    }
}

void bcDestroyMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    bcReleaseMesh(mesh);
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh);
    clist_delete_node(g_Context->RM_list, mesh);
}

void bcDrawMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    bcDrawMeshRange(mesh, 0, mesh->draw_count);
}

void bcBindMesh(BCMesh *mesh)
{
    if (g_Context->CurrentMesh == mesh)
    {
        bcLogWarning("Mesh already assigned!");
        return;
    }
    if (mesh == NULL)
    {
        // unbind mesh
#ifdef SUPPORT_GLSL
        for (int i = 0; i < BC_VERTEX_ATTR_MAX; i++)
        {
            glDisableVertexAttribArray(i);
        }
#else
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
#endif
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        // bind mesh
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
        float *vert_ptr = (mesh->vbo_vertices ? (float *) 0 : mesh->vertices);
#ifdef SUPPORT_GLSL
        for (int i = 0; i < BC_VERTEX_ATTR_MAX; i++)
        {
            if (mesh->comps[i] > 0)
            {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, mesh->comps[i], GL_FLOAT, GL_FALSE, mesh->total_comps * sizeof(float), vert_ptr);
                vert_ptr += mesh->comps[i];
            }
            else
            {
                glDisableVertexAttribArray(i);
            }
        }
        glUniform1i(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_VERTEX_COLOR_ENABLED], mesh->comps[BC_VERTEX_ATTR_COLORS]);
#else
        for (int i = 0; i < BC_VERTEX_ATTR_MAX; i++)
        {
            if (mesh->comps[i] > 0)
            {
                glEnableClientState(s_ClientStateType[i].type);
                switch (i)
                {
                case BC_VERTEX_ATTR_POSITIONS:
                    glVertexPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                case BC_VERTEX_ATTR_NORMALS:
                    glNormalPointer(GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                case BC_VERTEX_ATTR_TEXCOORDS:
                    glTexCoordPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                case BC_VERTEX_ATTR_COLORS:
                    glColorPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                }
                vert_ptr += mesh->comps[i];
            }
            else
            {
                glDisableClientState(s_ClientStateType[i].type);
            }
        }
#endif
    }
    g_Context->CurrentMesh = mesh;
}

void bcDrawMeshPart(BCMeshPart part)
{
    bcDrawMeshRange(part.mesh, part.start, part.count);
}

void bcDrawMeshRange(BCMesh *mesh, int start, int count)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    if (g_Context->CurrentMesh != mesh)
    {
        bcBindMesh(mesh);
    }
    if (g_Context->ColorNeedUpdate)
    {
#ifdef SUPPORT_GLSL
        glUniform4fv(g_Context->CurrentShader->loc_uniforms[BC_SHADER_UNIFORM_COLOR_ARRAY], BC_COLOR_TYPE_MAX, (float *) g_Context->ColorArray);
#else
        glColor4fv((float *) &(g_Context->ColorArray[BC_COLOR_TYPE_PRIMARY]));
        glMaterialfv(GL_FRONT, GL_DIFFUSE, (float *) &(g_Context->ColorArray[BC_COLOR_TYPE_DIFFUSE]));
        glMaterialfv(GL_FRONT, GL_AMBIENT, (float *) &(g_Context->ColorArray[BC_COLOR_TYPE_AMBIENT]));
        glMaterialfv(GL_FRONT, GL_SPECULAR, (float *) &(g_Context->ColorArray[BC_COLOR_TYPE_SPECULAR]));
        glMaterialfv(GL_FRONT, GL_EMISSION, (float *) &(g_Context->ColorArray[BC_COLOR_TYPE_EMISSION]));
#endif
        g_Context->ColorNeedUpdate = false;
    }
    if (mesh->num_indices)
    {
        uint16_t *elem_start = (mesh->vbo_indices ? (uint16_t *) 0 : mesh->indices) + start;
        glDrawElements(mesh->draw_mode, count, GL_UNSIGNED_SHORT, elem_start);
    }
    else
    {
        glDrawArrays(mesh->draw_mode, start, count);
    }
}

BCMeshPart bcPartFromMesh(BCMesh *mesh)
{
    BCMeshPart part = { mesh, 0, mesh->draw_count };
    return part;
}

BCMeshPart bcAttachMesh(BCMesh *mesh, BCMesh *src, bool destroy_src)
{
    BCMeshPart part = bcPartFromMesh(mesh);
    if (mesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return part;
    }
    if (src == NULL)
    {
        bcLogError("Invalid meshe!");
        return part;
    }
    // compare formats
    if (src->format != mesh->format)
    {
        bcLogError("Meshes format does not match!");
        return part;
    }
    // fill vertex data
    mesh->vertices = EXTEND_ARRAY(mesh->vertices, (mesh->num_vertices + src->num_vertices) * mesh->total_comps, float);
    float *vert_ptr = &(mesh->vertices[mesh->num_vertices * mesh->total_comps]);
    memcpy(vert_ptr, src->vertices, src->num_vertices * src->total_comps * sizeof(float));
    part.start = mesh->num_vertices;
    // fill index data
    if (src->num_indices > 0)
    {
        mesh->indices = EXTEND_ARRAY(mesh->indices, mesh->num_indices + src->num_indices, uint16_t);
        uint16_t *indx_ptr = &(mesh->indices[mesh->num_indices]);
        for (int i = 0; i < src->num_indices; i++)
        {
            indx_ptr[i] = src->indices[i] + mesh->num_vertices;
        }
        part.start = mesh->num_indices;
        mesh->num_indices += src->num_indices;
    }
    mesh->num_vertices += src->num_vertices;
    mesh->draw_count += src->draw_count;
    part.count = src->draw_count;
    if (destroy_src)
    {
        bcDestroyMesh(src);
    }
    return part;
}

BCMesh * bcCreateMeshFromFile(const char *filename)
{
    BCFile *file = bcOpenFile(filename, BC_FILE_READ_DATA);
    if (!file)
    {
        bcLogError("Can't open file: %s", filename);
        return NULL;
    }
    BCMeshFileHeader header;
    bcReadFile(file, &header, sizeof(header));
    if (header.signature[0] != s_MeshFileSignature[0] ||
        header.signature[1] != s_MeshFileSignature[1] ||
        header.signature[2] != s_MeshFileSignature[2] ||
        header.signature[3] != s_MeshFileSignature[3] ||
        header.version != s_MeshFileVersion ||
        header.size != sizeof(header))
    {
        bcLogError("Invalid mesh file!");
        bcCloseFile(file);
        return NULL;
    }
    float *vert_data = NEW_ARRAY(header.num_vertices * header.total_comps, float);
    bcReadFile(file, vert_data, header.num_vertices * header.total_comps * sizeof(float));
    uint16_t *indx_data = NEW_ARRAY(header.num_indices, uint16_t);
    bcReadFile(file, indx_data, header.num_indices * sizeof(uint16_t));
    bcCloseFile(file);
    BCMesh *mesh = bcCreateMesh(header.format, vert_data, header.num_vertices, indx_data, header.num_indices, (BCMeshType) header.type);
    free(vert_data);
    free(indx_data);
    return mesh;
}

bool bcSaveMeshToFile(BCMesh *mesh, const char *filename)
{
    BCFile *file = bcOpenFile(filename, BC_FILE_WRITE_DATA);
    if (!file)
    {
        bcLogError("Can't write to file: %s", filename);
        return false;
    }
    BCMeshFileHeader header;
    header.signature[0] = s_MeshFileSignature[0];
    header.signature[1] = s_MeshFileSignature[1];
    header.signature[2] = s_MeshFileSignature[2];
    header.signature[3] = s_MeshFileSignature[3];
    header.version = s_MeshFileVersion;
    header.size = sizeof(header);
    header.reserved = 0;
    header.format = mesh->format;
    header.type = mesh->type;
    header.total_comps = mesh->total_comps;
    header.num_vertices = mesh->num_vertices;
    header.num_indices = mesh->num_indices;
    bcWriteFile(file, &header, sizeof(header));
    bcWriteFile(file, mesh->vertices, mesh->num_vertices * mesh->total_comps * sizeof(float));
    bcWriteFile(file, mesh->indices, mesh->num_indices * sizeof(uint16_t));
    bcCloseFile(file);
    return true;
}

//
// IM
//

static struct
{
    BCDrawMode mode;
    int type;
} const s_DrawModeMap[] =
{
    { BC_LINES, GL_LINES },
    { BC_LINE_LOOP, GL_LINE_LOOP },
    { BC_LINE_STRIP, GL_LINE_STRIP },
    { BC_TRIANGLES, GL_TRIANGLES },
    { BC_TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
    { BC_TRIANGLE_FAN, GL_TRIANGLE_FAN },
    { BC_QUADS, GL_TRIANGLES },
};

bool bcBegin(BCDrawMode mode)
{
    if (g_Context->ReusableSolidMesh == NULL)
    {
        g_Context->ReusableSolidMesh = bcCreateMesh(BC_MESH_POS3 | BC_MESH_NORM | BC_MESH_TEX2 | BC_MESH_COL4, NULL, 1024, NULL, 1024, BC_MESH_DYNAMIC);
    }
    int ret = bcBeginMesh(g_Context->ReusableSolidMesh, mode);
    if (ret)
    {
        bcColor(g_Context->ColorArray[BC_COLOR_TYPE_PRIMARY]);
    }
    return ret;
}

void bcEnd()
{
    bcEndMesh(g_Context->ReusableSolidMesh);
    bcDrawMesh(g_Context->ReusableSolidMesh);
}

bool bcBeginMesh(BCMesh *mesh, BCDrawMode mode)
{
    if (g_Context->TempMesh != NULL)
    {
        bcLogWarning("Mesh already locked!");
        return false;
    }
    if (mesh == NULL)
    {
        bcLogWarning("Mesh not provided!");
        return false;
    }
    g_Context->TempMesh = mesh;
    g_Context->TempMesh->draw_count = 0;
    g_Context->TempMesh->draw_mode = s_DrawModeMap[mode].type;
    g_Context->TempVertexData[BC_VERTEX_ATTR_POSITIONS] = vec4(0, 0, 0, 0);
    g_Context->TempVertexData[BC_VERTEX_ATTR_NORMALS] = vec4(0, 0, 1, 0);
    g_Context->TempVertexData[BC_VERTEX_ATTR_TEXCOORDS] = vec4(0, 0, 0, 0);
    g_Context->TempVertexData[BC_VERTEX_ATTR_COLORS] = vec4(1, 1, 1, 1);
    g_Context->VertexCounter = 0;
    g_Context->IndexCounter = 0;
    g_Context->DrawMode = mode;
    return true;
}

void bcEndMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    if (mesh != g_Context->TempMesh)
    {
        bcLogError("Wrong mesh!");
        return;
    }
    // generate indices
    if (g_Context->TempMesh->num_indices > 0 && g_Context->IndexCounter == 0)
    {
        for (int i = 0; i < g_Context->VertexCounter; i++)
        {
            if (g_Context->DrawMode == BC_QUADS && i % 4 == 3)
            {
                bcIndexi(i - 3);
                bcIndexi(i - 1);
                bcIndexi(i);
            }
            else
            {
                bcIndexi(i);
            }
        }
    }
    // assign counter
    g_Context->TempMesh->draw_count = (g_Context->TempMesh->num_indices > 0) ? g_Context->IndexCounter : g_Context->VertexCounter;
    g_Context->TempMesh = NULL;
    // finish mesh
    if (mesh->type != BC_MESH_NO_VBO)
    {
        bcUpdateMesh(mesh);
    }
}

static void pushTempVertex()
{
    float *vert_ptr = &(g_Context->TempMesh->vertices[g_Context->VertexCounter * g_Context->TempMesh->total_comps]);
    for (int i = 0; i < BC_VERTEX_ATTR_MAX; i++)
    {
        if (g_Context->TempMesh->comps[i] > 0)
        {
            memcpy(vert_ptr, g_Context->TempVertexData[i].v, g_Context->TempMesh->comps[i] * sizeof(float));
            vert_ptr += g_Context->TempMesh->comps[i];
        }
    }
    g_Context->VertexCounter++;
}

int bcVertex3f(float x, float y, float z)
{
    if (g_Context->TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return -1;
    }
    if (g_Context->VertexCounter == g_Context->TempMesh->num_vertices)
    {
        bcLogWarning("Mesh limit reached!");
        return -1;
    }
    g_Context->TempVertexData[BC_VERTEX_ATTR_POSITIONS] = vec4(x, y, z, 0);
    pushTempVertex();
    return g_Context->VertexCounter - 1;
}

int bcVertex2f(float x, float y)
{
    return bcVertex3f(x, y, 0);
}

void bcIndexi(int i)
{
    if (g_Context->TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return;
    }
    if (g_Context->IndexCounter == g_Context->TempMesh->num_indices)
    {
        bcLogWarning("Mesh limit reached!");
        return;
    }
    g_Context->TempMesh->indices[g_Context->IndexCounter++] = i;
}

void bcTexCoord2f(float u, float v)
{
    if (g_Context->TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return;
    }
    g_Context->TempVertexData[BC_VERTEX_ATTR_TEXCOORDS] = vec4(u, v, 0, 0);
}

void bcNormal3f(float x, float y, float z)
{
    if (g_Context->TempMesh == NULL)
    {
        bcLogWarning("Mesh not locked!");
        return;
    }
    g_Context->TempVertexData[BC_VERTEX_ATTR_NORMALS] = vec4(x, y, z, 0);
}

void bcColor4f(float r, float g, float b, float a)
{
    if (g_Context->TempMesh == NULL)
    {
        bcSetColor((BCColor){r, g, b, a}, BC_COLOR_TYPE_PRIMARY);
    }
    else
    {
        g_Context->TempVertexData[BC_VERTEX_ATTR_COLORS] = vec4(r, g, b, a);
    }
}

void bcColor3f(float r, float g, float b)
{
    bcColor4f(r, g, b, 1.0f);
}

void bcColor(BCColor c)
{
    bcColor4f(c.r, c.g, c.b, c.a);
}

//
// Matrix Stack
//

void bcSetPerspective(float fovy, float aspect, float znear, float zfar)
{
    bcSetProjectionMatrix(mat4_perspective(fovy, aspect, znear, zfar).v);
}

void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
    bcSetProjectionMatrix(mat4_ortho(left, right, bottom, top, znear, zfar).v);
}

static mat4_t getCurrentMatrix()
{
    return mat4_from_array(bcGetModelViewMatrix());
}

void bcPushMatrix()
{
    if (g_Context->MatrixCounter == BC_MATRIX_STACK_SIZE - 1)
    {
        bcLogWarning("Max matrix stack reached!");
        return;
    }
    g_Context->MatrixStack[g_Context->MatrixCounter] = getCurrentMatrix();
    g_Context->MatrixCounter++;
}

void bcPopMatrix()
{
    if (g_Context->MatrixCounter == 0)
    {
        bcLogWarning("Min matrix stack reached!");
        return;
    }
    g_Context->MatrixCounter--;
    bcSetModelViewMatrix(g_Context->MatrixStack[g_Context->MatrixCounter].v);
}

void bcIdentity()
{
    bcSetModelViewMatrix(mat4_identity().v);
    bcSetTextureMatrix(mat3_identity().v);
}

void bcTranslatef(float x, float y, float z)
{
    bcSetModelViewMatrix(mat4_translate(getCurrentMatrix(), x, y, z).v);
}

void bcRotatef(float deg, float x, float y, float z)
{
    bcSetModelViewMatrix(mat4_rotate_axis(getCurrentMatrix(), to_radians(deg), x, y, z).v);
}

void bcScalef(float x, float y, float z)
{
    bcSetModelViewMatrix(mat4_scale(getCurrentMatrix(), x, y, z).v);
}

void bcMultMatrixf(float *m)
{
    bcSetModelViewMatrix(mat4_multiply(getCurrentMatrix(), mat4_from_array(m)).v);
}

bool bcScreenToWorldCoords(int winX, int winY, float out[3])
{
    int viewport[4] = { 0, 0, bcGetDisplayWidth(), bcGetDisplayHeight() };
    winY = bcGetDisplayHeight() - winY;
    float winZ = 0;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    mat4_t mvp = mat4_multiply(g_Context->ProjectionMatrix, g_Context->ModelViewMatrix);
    vec4_t v = mat4_unproject(mvp, winX, winY, winZ, viewport);
    if (out)
    {
        out[0] = v.x;
        out[1] = v.y;
        out[2] = v.z;
    }
    return true;
}

bool bcWorldToScreenCoords(float x, float y, float z, float out[2])
{
    int viewport[4] = { 0, 0, bcGetDisplayWidth(), bcGetDisplayHeight() };
    mat4_t mvp = mat4_multiply(g_Context->ProjectionMatrix, g_Context->ModelViewMatrix);
    vec4_t v = mat4_project(mvp, x, y, z, viewport);
    if (out)
    {
        out[0] = v.x;
        out[1] = bcGetDisplayHeight() - v.y;
    }
    return true;
}

//
// Camera
//

void bcPrepareScene3D(float fovy)
{
    BCWindow *win = bcGetWindow();
    float aspect = (float) win->height / (float) win->width;
    float znear = 0.1f;
    float zfar = 10000.0f;
    bcSetPerspective(to_radians(fovy), aspect, znear, zfar);
    bcIdentity();
    bcSetBlend(false);
    bcSetDepthTest(true);
    bcSetLighting(true);
}

void bcPrepareScene2D(float height, bool center)
{
    float width = bcGetDisplayAspectRatio() * height;
    if (center)
        bcSetOrtho(-width / 2, width / 2, -height / 2, height / 2, -1, 1);
    else
        bcSetOrtho(0, width, 0, height, -1, 1);
    bcIdentity();
    bcSetBlend(true);
    bcSetDepthTest(false);
    bcSetLighting(false);
}

void bcPrepareSceneGUI()
{
    BCWindow *win = bcGetWindow();
    bcSetOrtho(0, win->width, win->height, 0, -1, 1);
    bcIdentity();
    bcSetBlend(true);
    bcSetDepthTest(false);
    bcSetLighting(false);
}

//
// Draw 2D
//

void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh)
{
    bcBindTexture(texture);
    bcBegin(BC_TRIANGLES);
    bcTexCoord2f(sx, sy);
    bcVertex2f(x, y);
    bcTexCoord2f(sx + sw, sy);
    bcVertex2f(x + w, y);
    bcTexCoord2f(sx + sw, sy + sh);
    bcVertex2f(x + w, y + h);
    bcTexCoord2f(sx + sw, sy + sh);
    bcVertex2f(x + w, y + h);
    bcTexCoord2f(sx, sy + sh);
    bcVertex2f(x, y + h);
    bcTexCoord2f(sx, sy);
    bcVertex2f(x, y);
    bcEnd();
}

void bcDrawRect2D(float x, float y, float w, float h, bool fill)
{
    bcBegin(fill ? BC_QUADS : BC_LINE_LOOP);
    bcTexCoord2f(0, 0);
    bcVertex2f(x, y);
    bcTexCoord2f(1, 0);
    bcVertex2f(x + w, y);
    bcTexCoord2f(1, 1);
    bcVertex2f(x + w, y + h);
    bcTexCoord2f(0, 1);
    bcVertex2f(x, y + h);
    bcEnd();
}

void bcDrawCircle2D(float x, float y, float r, int segments, bool fill)
{
    bcBegin(fill ? BC_TRIANGLE_FAN : BC_LINE_LOOP);
    int n = segments;
    if (fill)
    {
        bcVertex2f(x, y);
        n++;
    }
    for (int i = 0; i < n; i++)
    {
        float t = (float) i / (float) segments * M_PI * 2;
        bcVertex2f(x + cosf(t) * r, y + sinf(t) * r);
    }
    bcEnd();
}

//
// Draw 3D
//

void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z, bool solid)
{
    BCMesh * mesh = NULL;
    if (solid)
    {
        if (g_Context->ReusableCubeMesh == NULL)
        {
            g_Context->ReusableCubeMesh = bcCreateMeshCube();
            bcUpdateMesh(g_Context->ReusableCubeMesh);
        }
        mesh = g_Context->ReusableCubeMesh;
    }
    else
    {
        if (g_Context->ReusableWireCubeMesh == NULL)
        {
            g_Context->ReusableWireCubeMesh = bcCreateMeshWireBox(0, 0, 0, 1, 1, 1);
            bcUpdateMesh(g_Context->ReusableWireCubeMesh);
        }
        mesh = g_Context->ReusableWireCubeMesh;
    }
    bcPushMatrix();
    bcTranslatef(x, y, z);
    bcScalef(size_x, size_y, size_z);
    bcDrawMesh(mesh);
    bcPopMatrix();
}

void bcDrawGrid(int size_x, int size_y)
{
    bcBegin(BC_LINES);
    for (int x = 0; x < size_x + 1; x++)
    {
        bcVertex2f(x, 0);
        bcVertex2f(x, size_y);
    }
    for (int y = 0; y < size_y + 1; y++)
    {
        bcVertex2f(0, y);
        bcVertex2f(size_x, y);
    }
    bcEnd();
}

void bcDrawPlane(float x, float y, float z, float size_x, float size_y)
{
    if (!g_Context->ReusablePlaneMesh)
    {
        BCMesh *mesh = bcCreateMesh(BC_MESH_POS3 | BC_MESH_NORM | BC_MESH_TEX2, NULL, 4, NULL, 6, BC_MESH_STATIC);
        if (bcBeginMesh(mesh, BC_QUADS))
        {
            bcTexCoord2f(0, 1);
            bcVertex2f(0, 0);
            bcTexCoord2f(1, 1);
            bcVertex2f(size_x, 0);
            bcTexCoord2f(1, 0);
            bcVertex2f(size_x, size_y);
            bcTexCoord2f(0, 0);
            bcVertex2f(0, size_y);
            bcEndMesh(mesh);
            g_Context->ReusablePlaneMesh = mesh;
        }
        else
        {
            bcDestroyMesh(mesh);
            return;
        }
    }
    bcPushMatrix();
    bcTranslatef(x, y, z);
    bcDrawMesh(g_Context->ReusablePlaneMesh);
    bcPopMatrix();
}


BCMesh * bcCreateMeshWireBox(float x1, float y1, float z1, float x2, float y2, float z2)
{
    BCMesh *mesh = bcCreateMesh(BC_MESH_POS3 | BC_MESH_NORM, NULL, 48, NULL, 0, BC_MESH_STATIC);
    if (bcBeginMesh(mesh, BC_LINES))
    {
        bcNormal3f(0, 0, -1);
        bcVertex3f(x2, y1, z1);
        bcVertex3f(x1, y1, z1);
        bcVertex3f(x1, y1, z1);
        bcVertex3f(x1, y2, z1);
        bcVertex3f(x1, y2, z1);
        bcVertex3f(x2, y2, z1);
        bcVertex3f(x2, y2, z1);
        bcVertex3f(x2, y1, z1);
        bcNormal3f(0, 1, 0);
        bcVertex3f(x2, y2, z1);
        bcVertex3f(x1, y2, z1);
        bcVertex3f(x1, y2, z1);
        bcVertex3f(x1, y2, z2);
        bcVertex3f(x1, y2, z2);
        bcVertex3f(x2, y2, z2);
        bcVertex3f(x2, y2, z2);
        bcVertex3f(x2, y2, z1);
        bcNormal3f(0, -1, 0);
        bcVertex3f(x2, y1, z2);
        bcVertex3f(x1, y1, z2);
        bcVertex3f(x1, y1, z2);
        bcVertex3f(x1, y1, z1);
        bcVertex3f(x1, y1, z1);
        bcVertex3f(x2, y1, z1);
        bcVertex3f(x2, y1, z1);
        bcVertex3f(x2, y1, z2);
        bcNormal3f(-1, 0, 0);
        bcVertex3f(x1, y2, z2);
        bcVertex3f(x1, y2, z1);
        bcVertex3f(x1, y2, z1);
        bcVertex3f(x1, y1, z1);
        bcVertex3f(x1, y1, z1);
        bcVertex3f(x1, y1, z2);
        bcVertex3f(x1, y1, z2);
        bcVertex3f(x1, y2, z2);
        bcNormal3f(1, 0, 0);
        bcVertex3f(x2, y2, z1);
        bcVertex3f(x2, y2, z2);
        bcVertex3f(x2, y2, z2);
        bcVertex3f(x2, y1, z2);
        bcVertex3f(x2, y1, z2);
        bcVertex3f(x2, y1, z1);
        bcVertex3f(x2, y1, z1);
        bcVertex3f(x2, y2, z1);
        bcNormal3f(0, 0, 1);
        bcVertex3f(x2, y2, z2);
        bcVertex3f(x1, y2, z2);
        bcVertex3f(x1, y2, z2);
        bcVertex3f(x1, y1, z2);
        bcVertex3f(x1, y1, z2);
        bcVertex3f(x2, y1, z2);
        bcVertex3f(x2, y1, z2);
        bcVertex3f(x2, y2, z2);
        bcEndMesh(mesh);
    }
    return mesh;
}

//
// Font
//

#define BAKE_BITMAP_WIDTH   512
#define BAKE_BITMAP_HEIGHT  512
#define BAKE_CHAR_FIRST     32
#define BAKE_CHAR_COUNT     96

struct font_data_bm
{
    int cols;
    int rows;
    int char_width;
    int char_height;
};

static bool getFontQuad(BCFont *font, char ch, float *px, float *py, stbtt_aligned_quad *pq)
{
    if (ch < font->char_first || ch >= font->char_first + font->char_count)
        return false;
    if (font->type == BC_FONT_TRUETYPE)
    {
        stbtt_GetBakedQuad((stbtt_bakedchar *) font->cdata,
                           BAKE_BITMAP_WIDTH, BAKE_BITMAP_HEIGHT,
                           ch - font->char_first, px, py, pq, 1); // 1=opengl & d3d10+, 0=d3d9
    }
    else if (font->type == BC_FONT_ANGELCODE)
    {
    }
    else if (font->type == BC_FONT_BITMAP)
    {
        struct font_data_bm *bm = (struct font_data_bm *) font->cdata;
        pq->x0 = *px;
        pq->y0 = *py;
        pq->x1 = pq->x0 + bm->char_width;
        pq->y1 = pq->y0 + bm->char_height;
        pq->s0 = (ch % bm->cols) / (float) (bm->cols);
        pq->t0 = (ch / bm->cols) / (float) (bm->cols);
        pq->s1 = pq->s0 + 1.0f / bm->cols;
        pq->t1 = pq->t0 + 1.0f / bm->cols;
        *px += bm->char_width;
        if (ch == '\n')
            *py += bm->char_height;
    }
    return true;
}

BCFont * bcCreateFont(const char *filename, BCFontParams params)
{
    switch (params.type)
    {
    case BC_FONT_TRUETYPE:
        return bcCreateFont_TTF(filename, params.ttf.height);
    case BC_FONT_ANGELCODE:
        return bcCreateFont_FNT(filename);
    case BC_FONT_BITMAP:
        return bcCreateFont_BMP(filename, params.bmp.char_first, params.bmp.char_count, params.bmp.cols);
    }
    return NULL;
}

BCFont * bcCreateFont_TTF(const char *filename, float height)
{
    int size = 0;
    unsigned char *ttf_buffer = bcLoadDataFile(filename, &size);
    if (ttf_buffer == NULL)
    {
        bcLogError("Failed loading font '%s'!", filename);
        return NULL;
    }
    BCFont *font = NEW_OBJECT(BCFont);
    font->type = BC_FONT_TRUETYPE;
    font->char_first = BAKE_CHAR_FIRST;
    font->char_count = BAKE_CHAR_COUNT;
    font->cdata = NEW_ARRAY(BAKE_CHAR_COUNT, stbtt_bakedchar);
    font->height = height;
    BCImage *image = bcCreateImage(BAKE_BITMAP_WIDTH, BAKE_BITMAP_HEIGHT, 1);
    stbtt_BakeFontBitmap(ttf_buffer, 0, height, image->data,
                         BAKE_BITMAP_WIDTH, BAKE_BITMAP_HEIGHT,
                         BAKE_CHAR_FIRST, BAKE_CHAR_COUNT, (stbtt_bakedchar *) font->cdata); // no guarantee this fits!
    font->texture = bcCreateTextureFromImage(image, 0);
    free(ttf_buffer);
    return font;
}

BCFont * bcCreateFont_FNT(const char *filename)
{
    return NULL;
}

BCFont * bcCreateFont_BMP(const char *filename, int char_first, int char_count, int cols)
{
    BCImage *image = bcCreateImageFromFile(filename);
    if (image == NULL)
        return NULL;
    struct font_data_bm *bm = NEW_OBJECT(struct font_data_bm);
    bm->cols = cols;
    bm->rows = char_count / cols;
    bm->char_width = image->width / cols;
    bm->char_height = image->height / bm->rows;
    BCFont *font = NEW_OBJECT(BCFont);
    font->type = BC_FONT_BITMAP;
    font->char_first = char_first;
    font->char_count = char_count;
    font->cdata = bm;
    font->texture = bcCreateTextureFromImage(image, 0);
    font->height = bm->char_height;
    return font;
}

void bcUpdateFont(BCFont *font)
{
    if (!font)
    {
        bcLogError("Invalid font!");
        return;
    }
    bcUpdateTexture(font->texture);
}

void bcReleaseFont(BCFont *font)
{
    if (!font)
    {
        bcLogError("Invalid font!");
        return;
    }
    bcReleaseTexture(font->texture);
}

void bcDestroyFont(BCFont *font)
{
    if (!font)
    {
        bcLogError("Invalid font!");
        return;
    }
    bcReleaseFont(font);
    free(font->cdata);
    free(font);
}

void bcDrawText(BCFont *font, float x, float y, const char *text)
{
    if (font == NULL || text == NULL)
        return;
    bcBindTexture(font->texture);
    bcBegin(BC_TRIANGLES);
    float start_x = x;
    while (*text)
    {
        stbtt_aligned_quad q;
        if (getFontQuad(font, *text, &x, &y, &q))
        {
            bcTexCoord2f(q.s0, q.t0);
            bcVertex2f(q.x0, q.y0);
            bcTexCoord2f(q.s1, q.t0);
            bcVertex2f(q.x1, q.y0);
            bcTexCoord2f(q.s1, q.t1);
            bcVertex2f(q.x1, q.y1);
            bcTexCoord2f(q.s1, q.t1);
            bcVertex2f(q.x1, q.y1);
            bcTexCoord2f(q.s0, q.t1);
            bcVertex2f(q.x0, q.y1);
            bcTexCoord2f(q.s0, q.t0);
            bcVertex2f(q.x0, q.y0);
        }
        if (*text == '\n')
        {
            x = start_x;
            y += font->height; // TODO: remove this or provide valid line height!
        }
        ++text;
    }
    bcEnd();
    bcBindTexture(NULL);
}

void bcGetTextSize(BCFont *font, const char *text, float *px, float *py)
{
    if (font == NULL || text == NULL)
        return;
    float x = 0;
    float y = 0;
    float my = 0;
    while (*text)
    {
        stbtt_aligned_quad q;
        if (getFontQuad(font, *text, &x, &y, &q))
        {
            float qy = q.y1 - q.y0;
            if (my < qy) my = qy;
        }
        ++text;
    }
    y += my;
    if (px) *px = x;
    if (py) *py = y;
}

//
// Geometry
//

BCMesh * bcCreateMeshFromShape(void *par_shape)
{
    par_shapes_mesh *shape = (par_shapes_mesh *) par_shape;
    int format = BC_MESH_POS3;
    int comps = 3;
    if (shape->normals)
    {
        format |= BC_MESH_NORM;
        comps += 3;
    }
    if (shape->tcoords)
    {
        format |= BC_MESH_TEX2;
        comps += 2;
    }
    float *vert_ptr = NEW_ARRAY(shape->npoints * comps, float);
    int vp = 0;
    for (int i = 0; i < shape->npoints; i++)
    {
        vert_ptr[vp++] = shape->points[i * 3 + 0];
        vert_ptr[vp++] = shape->points[i * 3 + 1];
        vert_ptr[vp++] = shape->points[i * 3 + 2];
        if (shape->normals)
        {
            vert_ptr[vp++] = shape->normals[i * 3 + 0];
            vert_ptr[vp++] = shape->normals[i * 3 + 1];
            vert_ptr[vp++] = shape->normals[i * 3 + 2];
        }
        if (shape->tcoords)
        {
            vert_ptr[vp++] = shape->tcoords[i * 2 + 0];
            vert_ptr[vp++] = shape->tcoords[i * 2 + 1];
        }
    }
    BCMesh *mesh = bcCreateMesh(format, vert_ptr, shape->npoints, shape->triangles, shape->ntriangles * 3, BC_MESH_STATIC);
    free(vert_ptr);
    return mesh;
}

BCMesh * bcCreateMeshCube()
{
    return bcCreateMeshBox(0, 0, 0, 1, 1, 1);
}

BCMesh * bcCreateMeshBox(float x1, float y1, float z1, float x2, float y2, float z2)
{
    BCMesh *mesh = bcCreateMesh(BC_MESH_POS3 | BC_MESH_NORM | BC_MESH_TEX2, NULL, 24, NULL, 36, BC_MESH_STATIC);
    if (bcBeginMesh(mesh, BC_QUADS))
    {
        // bottom
        bcNormal3f(0, 0, -1);
        bcTexCoord2f(1, 0);
        bcVertex3f(x2, y1, z1);
        bcTexCoord2f(0, 0);
        bcVertex3f(x1, y1, z1);
        bcTexCoord2f(0, 1);
        bcVertex3f(x1, y2, z1);
        bcTexCoord2f(1, 1);
        bcVertex3f(x2, y2, z1);
        // fornt
        bcNormal3f(0, 1, 0);
        bcTexCoord2f(0, 1);
        bcVertex3f(x2, y2, z1);
        bcTexCoord2f(1, 1);
        bcVertex3f(x1, y2, z1);
        bcTexCoord2f(1, 0);
        bcVertex3f(x1, y2, z2);
        bcTexCoord2f(0, 0);
        bcVertex3f(x2, y2, z2);
        // back
        bcNormal3f(0, -1, 0);
        bcTexCoord2f(1, 0);
        bcVertex3f(x2, y1, z2);
        bcTexCoord2f(0, 0);
        bcVertex3f(x1, y1, z2);
        bcTexCoord2f(0, 1);
        bcVertex3f(x1, y1, z1);
        bcTexCoord2f(1, 1);
        bcVertex3f(x2, y1, z1);
        // left
        bcNormal3f(-1, 0, 0);
        bcTexCoord2f(0, 0);
        bcVertex3f(x1, y2, z2);
        bcTexCoord2f(0, 1);
        bcVertex3f(x1, y2, z1);
        bcTexCoord2f(1, 1);
        bcVertex3f(x1, y1, z1);
        bcTexCoord2f(1, 0);
        bcVertex3f(x1, y1, z2);
        // right
        bcNormal3f(1, 0, 0);
        bcTexCoord2f(1, 1);
        bcVertex3f(x2, y2, z1);
        bcTexCoord2f(1, 0);
        bcVertex3f(x2, y2, z2);
        bcTexCoord2f(0, 0);
        bcVertex3f(x2, y1, z2);
        bcTexCoord2f(0, 1);
        bcVertex3f(x2, y1, z1);
        // top
        bcNormal3f(0, 0, 1);
        bcTexCoord2f(1, 0);
        bcVertex3f(x2, y2, z2);
        bcTexCoord2f(0, 0);
        bcVertex3f(x1, y2, z2);
        bcTexCoord2f(0, 1);
        bcVertex3f(x1, y1, z2);
        bcTexCoord2f(1, 1);
        bcVertex3f(x2, y1, z2);
        bcEndMesh(mesh);
    }
    return mesh;
}

BCMesh * bcCreateCylinder(float radius, float height, int slices)
{
    BCMesh *mesh = bcCreateMesh(BC_MESH_POS3 | BC_MESH_NORM | BC_MESH_TEX2, NULL, 4 * slices + 2, NULL, 12 * slices, BC_MESH_STATIC);
    if (bcBeginMesh(mesh, BC_TRIANGLES))
    {
        bcNormal3f(0, 0, -1);
        /* 0 */ bcVertex3f(0, 0, 0);
        bcNormal3f(0, 0, 1);
        /* 1 */ bcVertex3f(0, 0, height);
        int i1, i2, j1, j2;
        int k1, k2, k3, k4;
        i2 = (slices - 1) * 4 + 2;
        j2 = (slices - 1) * 4 + 3;
        k3 = (slices - 1) * 4 + 4;
        k4 = (slices - 1) * 4 + 5;
        for (int i = 0; i < slices; i++)
        {
            float t = (float) i / slices * M_PI * 2;
            float st = sinf(t);
            float ct = cosf(t);
            float x = st * radius;
            float y = ct * radius;
            bcNormal3f(0, 0, -1);
            i1 = bcVertex3f(x, y, 0);
            bcNormal3f(0, 0, 1);
            j1 = bcVertex3f(x, y, height);
            {
                // bottom caps
                bcIndexi(0);
                bcIndexi(i2);
                bcIndexi(i1);
                // top caps
                bcIndexi(1);
                bcIndexi(j1);
                bcIndexi(j2);
                // body
                bcNormal3f(st, ct, 0);
                k1 = bcVertex3f(x, y, 0);
                k2 = bcVertex3f(x, y, height);
                bcIndexi(k1);
                bcIndexi(k3);
                bcIndexi(k2);
                bcIndexi(k2);
                bcIndexi(k3);
                bcIndexi(k4);
            }
            i2 = i1;
            j2 = j1;
            k3 = k1;
            k4 = k2;
        }
        bcEndMesh(mesh);
    }
    return mesh;
}

BCMesh * bcCreateMeshSphere(float radius, int slices, int stacks)
{
    par_shapes_mesh *shape = par_shapes_create_parametric_sphere(slices, stacks);
    par_shapes_scale(shape, radius/2, radius/2, radius/2);
    BCMesh *mesh = bcCreateMeshFromShape(shape);
    par_shapes_free_mesh(shape);
    return mesh;
}

void bcTransformMesh(BCMesh *mesh, float *m)
{
    mat4_t tm = mat4_from_array(m);
    float *vert_ptr = mesh->vertices;
    for (int i = 0; i < mesh->num_vertices; i++)
    {
        vec4_t v = vec4(vert_ptr[0], vert_ptr[1], vert_ptr[2], 1);
        v = vec4_multiply_mat4(tm, v);
        vert_ptr[0] = v.x;
        vert_ptr[1] = v.y;
        vert_ptr[2] = v.z;
        if (mesh->comps[BC_VERTEX_ATTR_NORMALS] == 3)
        {
            vec4_t vn = vec4(vert_ptr[3], vert_ptr[4], vert_ptr[5], 0);
            vn = vec4_multiply_mat4(tm, vn);
            vert_ptr[3] = vn.x;
            vert_ptr[4] = vn.y;
            vert_ptr[5] = vn.z;
        }
        vert_ptr += mesh->total_comps;
    }
}

void bcDumpMesh(BCMesh *mesh, FILE *stream)
{
    if (mesh == NULL)
    {
        bcLogWarning("Invalid mesh!");
        return;
    }
    fprintf(stream, "o Dump\n");
    char line[100];
    int vp_size = mesh->comps[BC_VERTEX_ATTR_POSITIONS];
    int vt_size = mesh->comps[BC_VERTEX_ATTR_TEXCOORDS];
    int vn_size = mesh->comps[BC_VERTEX_ATTR_NORMALS];
    // vertices
    for (int i = 0; i < mesh->num_vertices; i++)
    {
        strcpy(line, "v");
        for (int j = 0; j < vp_size; j++)
        {
            sprintf(line, "%s %f", line, mesh->vertices[i * mesh->total_comps + j]);
        }
        fprintf(stream, "%s\n", line);
    }
    // texture coordinates
    if (vt_size > 0)
    {
        for (int i = 0; i < mesh->num_vertices; i++)
        {
            strcpy(line, "vt");
            for (int j = 0; j < vt_size; j++)
            {
                sprintf(line, "%s %f", line, mesh->vertices[i * mesh->total_comps + vp_size + vn_size + j]);
            }
            fprintf(stream, "%s\n", line);
        }
    }
    // normals
    if (vn_size > 0)
    {
        for (int i = 0; i < mesh->num_vertices; i++)
        {
            strcpy(line, "vn");
            for (int j = 0; j < vn_size; j++)
            {
                sprintf(line, "%s %f", line, mesh->vertices[i * mesh->total_comps + vp_size + j]);
            }
            fprintf(stream, "%s\n", line);
        }
    }
    // faces
    if (mesh->num_indices > 0)
    {
        if (mesh->num_indices % 3)
        {
            bcLogWarning("Invalid number of indicies!");
        }
        else
        {
            for (int i = 0; i < mesh->num_indices; i += 3)
            {
                strcpy(line, "f");
                for (int j = 0; j < 3; j++)
                {
                    int ind = mesh->indices[i + j] + 1;
                    sprintf(line, "%s %d", line, ind);
                    if (vt_size > 0)
                    {
                        sprintf(line, "%s/%d", line, ind);
                    }
                    if (vn_size > 0)
                    {
                        if (vt_size == 0)
                        {
                            sprintf(line, "%s/", line);
                        }
                        sprintf(line, "%s/%d", line, ind);
                    }
                }
                fprintf(stream, "%s\n", line);
            }
        }
    }
}

bool bcGetMeshAABB(BCMesh *mesh, float *minv, float *maxv)
{
    if (!mesh || !mesh->vertices)
    {
        bcLogWarning("Invalid mesh!");
        return false;
    }
    minv[0] = mesh->vertices[0];
    minv[1] = mesh->vertices[1];
    minv[2] = mesh->vertices[2];
    maxv[0] = mesh->vertices[0];
    maxv[1] = mesh->vertices[1];
    maxv[2] = mesh->vertices[2];
    for (int i = 1; i < mesh->num_vertices; i++)
    {
        minv[0] = fminf(minv[0], mesh->vertices[i * mesh->total_comps + 0]);
        minv[1] = fminf(minv[1], mesh->vertices[i * mesh->total_comps + 1]);
        minv[2] = fminf(minv[2], mesh->vertices[i * mesh->total_comps + 2]);
        maxv[0] = fmaxf(maxv[0], mesh->vertices[i * mesh->total_comps + 0]);
        maxv[1] = fmaxf(maxv[1], mesh->vertices[i * mesh->total_comps + 1]);
        maxv[2] = fmaxf(maxv[2], mesh->vertices[i * mesh->total_comps + 2]);
    }
    return true;
}
