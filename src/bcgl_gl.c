#include "bcgl.h"
#include "bcgl_opengl.h"

#include <stb/stb_image.h>
#include <mathc/mathc.h>

#define MATRIX_STACK_SIZE 32

static float s_BackgroundColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
static float s_ProjectionMatrix[16];
static float s_MatrixStack[MATRIX_STACK_SIZE][16];
static int s_CurrentMatrix = 0;
static BCMesh *s_Mesh = NULL;
#ifdef SUPPORT_GLSL
static BCShader *s_Shader = NULL;
#endif

#ifdef PLATFORM_ANDROID
#define GLSL_CODE_HEADER \
    "#version 100\n" \
    "precision mediump float;\n"
#else
#define GLSL_CODE_HEADER \
    "#version 120\n"
#endif

#define STRINGIFY(src) #src

static const char s_VertexShaderCode[] =
STRINGIFY(
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;
attribute vec4 a_color;
uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform vec4 u_color;
uniform bool a_color_enabled;
varying vec2 v_texCoord;
varying vec4 v_color;
void main()
{
    gl_Position = u_projection * u_modelView * vec4(a_position, 1);
    v_texCoord = a_texCoord;
    // if (a_color_enabled) {
        v_color = a_color;
    // } else {
        // v_color = vec3(1,1,1);
    // }
}
);

static const char s_FragmentShaderCode[] =
STRINGIFY(
varying vec2 v_texCoord;
varying vec4 v_color;
uniform sampler2D u_texture;
uniform bool u_useTexture;
uniform bool u_alphaTest;
void main()
{
    vec4 tex = vec4(1, 1, 1, 1);
    if (u_useTexture) {
        tex = texture2D(u_texture, v_texCoord);
        if (u_alphaTest && tex.a < 0.1)
            discard;
    }
    gl_FragColor = tex * v_color;
}
);

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
    int x, y, comp;
    unsigned char *data = stbi_load(filename, &x, &y, &comp, 0);
    if (data == NULL)
    {
        bcLog("Image file '%s' not found!", filename);
        return NULL;
    }
    bcLog("Image file '%s' loaded OK", filename);
    BCImage *image = NEW_OBJECT(BCImage);
    image->width = x;
    image->height = y;
    image->comps = comp;
    image->data = data;
    return image;
}

BCImage * bcCreateImageFromMemory(void *buffer, int size)
{
    int x, y, comp;
    unsigned char *data = stbi_load_from_memory(buffer, size, &x, &y, &comp, 0);
    if (data == NULL)
    {
        bcLog("Image data not valid!");
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
    BCTexture *texture = bcCreateTextureFromImage(image, flags);
    bcDestroyImage(image);
    return texture;
}

static int get_glFormat(int comps)
{
    switch (comps)
    {
    case 1: return GL_ALPHA;
    case 3: return GL_RGB;
    case 4: return GL_RGBA;
    }
    return -1;
}

BCTexture * bcCreateTextureFromImage(BCImage *image, int flags)
{
    BCTexture *texture = NEW_OBJECT(BCTexture);
    texture->width = image->width;
    texture->height = image->height;
    int internalFormat;
    switch (image->comps)
    {
    case 1:
        texture->format = GL_ALPHA;
        internalFormat = GL_ALPHA;
        break;
    case 3:
        texture->format = GL_RGB;
        internalFormat = GL_RGB;
        break;
    case 4:
        texture->format = GL_RGBA;
        internalFormat = GL_BGRA;
        break;
    }
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &(texture->id));
    glBindTexture(GL_TEXTURE_2D, (texture->id));
    // filter flags
    if (flags & TEXTURE_PARAM_LINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (flags & TEXTURE_PARAM_NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (flags & TEXTURE_PARAM_MIPMAP)
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
    if (flags & TEXTURE_PARAM_REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if (flags & TEXTURE_PARAM_CLAMP)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        texture->width,
        texture->height,
        0,
        texture->format,
        GL_UNSIGNED_BYTE,
        image->data);
    // if (mMipmaps) {
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void bcDestroyTexture(BCTexture *texture)
{
    glDeleteTextures(1, &(texture->id));
    free(texture);
}

void bcBindTexture(BCTexture *texture)
{
    if (texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture->id);
    }
#ifdef SUPPORT_GLSL
    glUniform1i(s_Shader->loc[SHADER_LOC_U_USETEXTURE], texture ? 1 : 0);
    glUniform1i(s_Shader->loc[SHADER_LOC_U_TEXTURE], 0);
#else
    glActiveTexture(GL_TEXTURE0);
    if (texture)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
#endif
}

void bcDrawTexture(BCTexture *texture)
{
    float w = texture->width;
    float h = texture->height;

    bcBindTexture(texture);
    // TODO: bcBegin(GL_TRIANGLES);
    bcBegin(0);
    bcTexCoord2f(0, 0);
    bcVertex2f(0, 0);
    bcTexCoord2f(1, 0);
    bcVertex2f(w, 0);
    bcTexCoord2f(1, 1);
    bcVertex2f(w, h);
    bcTexCoord2f(1, 1);
    bcVertex2f(w, h);
    bcTexCoord2f(0, 1);
    bcVertex2f(0, h);
    bcTexCoord2f(0, 0);
    bcVertex2f(0, 0);
    bcEnd();
}

// Shader

static void applyProjectionMatrix()
{
#ifdef SUPPORT_GLSL
    glUniformMatrix4fv(s_Shader->loc[SHADER_LOC_U_PROJECTION], 1, GL_FALSE, s_ProjectionMatrix);
#else
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(s_ProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
#endif
}

static void applyCurrentMatrix()
{
#ifdef SUPPORT_GLSL
    glUniformMatrix4fv(s_Shader->loc[SHADER_LOC_U_MODELVIEW], 1, GL_FALSE, s_MatrixStack[s_CurrentMatrix]);
#else
    glLoadMatrixf(s_MatrixStack[s_CurrentMatrix]);
#endif
}

static void bindShaderVariables(BCShader *shader)
{
    // attributes
    // glBindAttribLocation(shader->programId, 0, "a_position");
    // glBindAttribLocation(shader->programId, 1, "a_normal");
    // glBindAttribLocation(shader->programId, 2, "a_texCoord");
    // glBindAttribLocation(shader->programId, 3, "a_color");
    shader->loc[SHADER_LOC_A_POSITION] = glGetAttribLocation(shader->programId, "a_position");
    shader->loc[SHADER_LOC_A_NORMAL] = glGetAttribLocation(shader->programId, "a_normal");
    shader->loc[SHADER_LOC_A_TEXCOORD] = glGetAttribLocation(shader->programId, "a_texCoord");
    shader->loc[SHADER_LOC_A_COLOR] = glGetAttribLocation(shader->programId, "a_color");
    // uniforms
    shader->loc[SHADER_LOC_U_PROJECTION] = glGetUniformLocation(shader->programId, "u_projection");
    shader->loc[SHADER_LOC_U_MODELVIEW] = glGetUniformLocation(shader->programId, "u_modelView");
    shader->loc[SHADER_LOC_U_TEXTURE] = glGetUniformLocation(shader->programId, "u_texture");
    shader->loc[SHADER_LOC_U_COLOR] = glGetUniformLocation(shader->programId, "u_color");
    shader->loc[SHADER_LOC_U_USETEXTURE] = glGetUniformLocation(shader->programId, "u_useTexture");
    shader->loc[SHADER_LOC_U_ALPHATEST] = glGetUniformLocation(shader->programId, "u_alphaTest");
    // get attribs
    // char tmp[64];
    // for (int i = 0; i < MAX_SHADER_VERTEX_ATTRIB; i++) {
    //     mVertexAttrib[i].location = glGetAttribLocation(programId, ATT_NAME_TAGS[i]);
    //     sprintf(tmp, "%s_enabled", ATT_NAME_TAGS[i]);
    //     mVertexAttrib[i].enabledUniform = glGetUniformLocation(programId, tmp);
    // }
    static const char * names[] = {
        "SHADER_LOC_A_POSITION",
        "SHADER_LOC_A_NORMAL",
        "SHADER_LOC_A_TEXCOORD",
        "SHADER_LOC_A_COLOR",
        "SHADER_LOC_U_PROJECTION",
        "SHADER_LOC_U_MODELVIEW",
        "SHADER_LOC_U_TEXTURE",
        "SHADER_LOC_U_COLOR",
        "SHADER_LOC_U_USETEXTURE",
        "SHADER_LOC_U_ALPHATEST",
    };
    for (int i = 0; i < SHADER_LOC_MAX; i++)
    {
        bcLog("loc[%s]=%d ... %s", names[i], shader->loc[i], shader->loc[i] == -1 ? "FAIL" : "OK");
    }
}

static GLuint loadShader(const char *code, GLenum shaderType)
{
    // init shader source
    const char *strings[2] = { GLSL_CODE_HEADER, code };
    int lengths[2] = { (int) strlen(GLSL_CODE_HEADER), (int) strlen(code) };
    // create and compile
    GLuint shaderId = glCreateShader(shaderType);
    if (shaderId == 0)
    {
        bcLog("glCreateShader failed!");
        return 0;
    }
    glShaderSource(shaderId, 2, strings, lengths);
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
            bcLog("[ERROR] Compile shader:\n%s", errorLog);
        }
        else
        {
            bcLog("[WARNING] Compile shader:\n%s", errorLog);
        }
    }
    if (compileError == GL_FALSE)
    {
        glDeleteShader(shaderId);
        bcLog("Shader not created!");
        return 0;
    }
    return shaderId;
}

BCShader * bcCreateShaderFromFile(const char *filename)
{
}

BCShader * bcCreateShaderFromCode(const char *vsCode, const char *fsCode)
{
    BCShader *shader = NEW_OBJECT(BCShader);
    // create program
    shader->programId = glCreateProgram();
    if (shader->programId == 0)
    {
        bcLog("Error creating shader program!");
        goto shader_create_error;
    }
    // vertex shaders
    shader->vertexShader = loadShader(vsCode, GL_VERTEX_SHADER);
    if (shader->vertexShader == 0)
        goto shader_create_error;
    glAttachShader(shader->programId, shader->vertexShader);
    // fragment shader
    shader->fragmentShader = loadShader(fsCode, GL_FRAGMENT_SHADER);
    if (shader->fragmentShader == 0)
        goto shader_create_error;
    glAttachShader(shader->programId, shader->fragmentShader);
    // link program
    glLinkProgram(shader->programId);
    GLint linkError = 0;
    glGetProgramiv(shader->programId, GL_LINK_STATUS, &linkError);
    if (linkError == GL_FALSE) {
        char errorLog[2000];
        GLsizei errorLen = 0;
        glGetProgramInfoLog(shader->programId, 2000, &errorLen, errorLog);
        bcLog("[ERROR] Link program:\n%s", errorLog);
        goto shader_create_error;
    }
    // validate program
    glValidateProgram(shader->programId);
    GLint validateError = 0;
    glGetProgramiv(shader->programId, GL_VALIDATE_STATUS, &validateError);
    if (validateError == GL_FALSE) {
        char errorLog[2000];
        GLsizei errorLen = 0;
        glGetProgramInfoLog(shader->programId, 2000, &errorLen, errorLog);
        bcLog("[ERROR] Validate program:\n%s", errorLog);
        goto shader_create_error;
    }
    // variables
    bindShaderVariables(shader);
    return shader;

shader_create_error:
    bcDestroyShader(shader);
    return NULL;
}

void bcDestroyShader(BCShader *shader)
{
    glDeleteShader(shader->vertexShader);
    glDeleteShader(shader->fragmentShader);
    glDeleteProgram(shader->programId);
    free(shader);
}

void bcBindShader(BCShader *shader)
{
    glUseProgram(shader->programId);
    applyProjectionMatrix();
    applyCurrentMatrix();
    s_Shader = shader;
}

// View State

void bcLoadGL()
{
    // matrix stack
    mat4_identity(s_ProjectionMatrix);
    mat4_identity(s_MatrixStack[0]);
#ifdef SUPPORT_GLSL
    s_Shader = bcCreateShaderFromCode(s_VertexShaderCode, s_FragmentShaderCode);
    bcBindShader(s_Shader);
#else
    glAlphaFunc(GL_GREATER, 0.1f);
    glDisable(GL_LIGHTING);
#endif
    s_Mesh = bcCreateMesh(512, 0);
    // default state
    glClearColor(s_BackgroundColor[0], s_BackgroundColor[1], s_BackgroundColor[2], s_BackgroundColor[3]);
    // gl default
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void bcFreeGL()
{
#ifdef SUPPORT_GLSL
    bcDestroyShader(s_Shader);
#endif
    bcDestroyMesh(s_Mesh);
}

void bcClear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void bcSetColor(BCColor color)
{
#ifdef SUPPORT_GLSL
    glUniform4fv(s_Shader->loc[SHADER_LOC_U_COLOR], 1, &(color.r));
#else
    glColor4f(color.r, color.g, color.b, color.a);
#endif
}

void bcSetBlend(bool enable)
{
    if (enable)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void bcSetDepthTest(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

// Matrix Stack

void bcSetPerspective(float fovy, float aspect, float znear, float zfar)
{
    mat4_perspective(s_ProjectionMatrix, fovy, aspect, znear, zfar);
    applyProjectionMatrix();
}

void bcSetOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
    mat4_ortho(s_ProjectionMatrix, left, right, bottom, top, znear, zfar);
    applyProjectionMatrix();
}

void bcPushMatrix()
{
    if (s_CurrentMatrix == MATRIX_STACK_SIZE - 1)
    {
        bcLog("Max matrix stack reached!");
        return;
    }
    s_CurrentMatrix++;
    mat4_assign(s_MatrixStack[s_CurrentMatrix], s_MatrixStack[s_CurrentMatrix - 1]);
    applyCurrentMatrix();
}

void bcPopMatrix()
{
    if (s_CurrentMatrix == 0)
    {
        bcLog("Min matrix stack reached!");
        return;
    }
    s_CurrentMatrix--;
    applyCurrentMatrix();
}

void bcIdentity()
{
    mat4_identity(s_MatrixStack[s_CurrentMatrix]);
    applyCurrentMatrix();
}

void bcTranslatef(float x, float y, float z)
{
    float offset[3] = { x, y, z };
    float m0[16], m1[16];
    mat4_assign(m0, s_MatrixStack[s_CurrentMatrix]);
    mat4_identity(m1);
    mat4_translation(m1, m1, offset);
    mat4_multiply(s_MatrixStack[s_CurrentMatrix], m0, m1);
    applyCurrentMatrix();
}

void bcRotatef(float angle, float x, float y, float z)
{
    float axis[3] = { x, y, z };
    float m0[16], m1[16];
    mat4_assign(m0, s_MatrixStack[s_CurrentMatrix]);
    mat4_identity(m1);
    mat4_rotation_axis(m1, axis, to_radians(angle));
    mat4_multiply(s_MatrixStack[s_CurrentMatrix], m0, m1);
    applyCurrentMatrix();
}

void bcScalef(float x, float y, float z)
{
    float offset[3] = { x, y, z };
    float m0[16], m1[16];
    mat4_assign(m0, s_MatrixStack[s_CurrentMatrix]);
    mat4_identity(m1);
    mat4_scale(m1, m1, offset);
    mat4_multiply(s_MatrixStack[s_CurrentMatrix], m0, m1);
    applyCurrentMatrix();
}

// Mesh

BCMesh * bcCreateMesh(int size, int flags)
{
    BCMesh *mesh = NEW_OBJECT(BCMesh);
    mesh->count = 0;
    mesh->total = size;
    mesh->buffers[MESH_BUFFER_POSITIONS].comps = 3;
    mesh->buffers[MESH_BUFFER_TEXCOORDS].comps = 2;
    mesh->buffers[MESH_BUFFER_NORMALS].comps = 0; //3;
    mesh->buffers[MESH_BUFFER_COLORS].comps = 4;

    // TODO:
    bool isDynamic = true;
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(mesh->buffers[i]);
        if (buff->comps > 0)
        {
            int buffSize = size * sizeof(float) * buff->comps;
            buff->vertices = (float *) malloc(buffSize);
            // bind vertex data
            glGenBuffers(1, &(buff->vboId));
            glBindBuffer(GL_ARRAY_BUFFER, buff->vboId);
            glBufferData(GL_ARRAY_BUFFER, buffSize, buff->vertices, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    // // bind index data
    // if (mIndicesCount) {
    //     glGenBuffers(1, &mIndexVbo);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVbo);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicesCount * sizeof(unsigned short), mIndexData,
    //                  isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // }
    // mIsDynamicVbo = isDynamic;
    return mesh;
}

BCMesh * bcCreateMeshFromFile(const char *filename);
BCMesh * bcCreateMeshFromMemory(void *buffer);

void bcDestroyMesh(BCMesh *mesh)
{
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(mesh->buffers[i]);
        free(buff->vertices);
        glDeleteBuffers(1, &(buff->vboId));
    }
    free(mesh);
}

void bcUpdateMesh(BCMesh *mesh)
{
    if (mesh->count == 0)
        return;
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(mesh->buffers[i]);
        if (buff->comps > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buff->vboId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->count * sizeof(float) * buff->comps, buff->vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}

void bcDrawMesh(BCMesh *mesh)
{
#ifdef SUPPORT_GLSL
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(mesh->buffers[i]);
        if (buff->comps > 0)
        {
            glEnableVertexAttribArray(s_Shader->loc[i]);
            glVertexAttribPointer(s_Shader->loc[i], buff->comps, GL_FLOAT, GL_FALSE, 0, buff->vertices);
        }
    }
    glDrawArrays(GL_TRIANGLES, 0, mesh->count);
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(mesh->buffers[i]);
        if (buff->comps > 0)
        {
            glDisableVertexAttribArray(s_Shader->loc[i]);
        }
    }
#else
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(mesh->buffers[i]);
        if (buff->comps > 0)
        {
            switch (i)
            {
            case MESH_BUFFER_POSITIONS:
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(buff->comps, GL_FLOAT, 0, buff->vertices);
                break;
            case MESH_BUFFER_NORMALS:
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, 0, buff->vertices);
                break;
            case MESH_BUFFER_TEXCOORDS:
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(buff->comps, GL_FLOAT, 0, buff->vertices);
                break;
            case MESH_BUFFER_COLORS:
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(buff->comps, GL_FLOAT, 0, buff->vertices);
                break;
            }
        }
    }
    glDrawArrays(GL_TRIANGLES, 0, mesh->count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
#endif
}

// IM

static BCMesh * s_TempMesh = NULL;
static float s_TempVertexData[MESH_BUFFER_MAX][4];

bool bcBegin(int type)
{
    return bcBeginMesh(s_Mesh);
}

void bcEnd()
{
    bcEndMesh(s_TempMesh);
}

bool bcBeginMesh(BCMesh *mesh)
{
    if (s_TempMesh != NULL)
    {
        bcLog("Mesh already locked!");
        return false;
    }
    if (mesh == NULL)
    {
        bcLog("Mesh not provided!");
        return false;
    }
    s_TempMesh = mesh;
    s_TempMesh->count = 0;
    vec4(s_TempVertexData[MESH_BUFFER_POSITIONS], 0, 0, 0, 0);
    vec4(s_TempVertexData[MESH_BUFFER_NORMALS], 0, 0, 1, 0);
    vec4(s_TempVertexData[MESH_BUFFER_TEXCOORDS], 0, 0, 0, 0);
    vec4(s_TempVertexData[MESH_BUFFER_COLORS], 1, 1, 1, 1);
    return true;
}

void bcEndMesh()
{
    bcUpdateMesh(s_TempMesh);
    bcDrawMesh(s_TempMesh);
    s_TempMesh = NULL;
}

void bcVertex3f(float x, float y, float z)
{
    if (s_TempMesh == NULL)
    {
        bcLog("Mesh not locked!");
        return;
    }
    if (s_TempMesh->count >= s_TempMesh->total)
    {
        bcLog("Mesh limit reached!");
        return;
    }
    vec4(s_TempVertexData[MESH_BUFFER_POSITIONS], x, y, z, 0);
    for (int i = 0; i < MESH_BUFFER_MAX; i++)
    {
        struct BCMeshBuffer *buff = &(s_TempMesh->buffers[i]);
        float * ptr = &(buff->vertices[buff->comps * s_TempMesh->count]);
        memcpy(ptr, s_TempVertexData[i], buff->comps * sizeof(float));
    }
    s_TempMesh->count++;
}

void bcVertex2f(float x, float y)
{
    bcVertex3f(x, y, 0);
}

void bcTexCoord2f(float u, float v)
{
    vec2(s_TempVertexData[MESH_BUFFER_TEXCOORDS], u, v);
}

void bcNormalf(float x, float y, float z)
{
    vec3(s_TempVertexData[MESH_BUFFER_NORMALS], x, y, z);
}

void bcColor4f(float r, float g, float b, float a)
{
    vec4(s_TempVertexData[MESH_BUFFER_COLORS], r, g, b, a);
}

void bcColor3f(float r, float g, float b)
{
    bcColor4f(r, g, b, 1.0f);
}

void bcColorHex(unsigned int argb)
{
}

// Camera

void bcPrepareScene3D()
{
    BCWindow *win = bcGetWindow();
    float fovy = 60.0f;
    float aspect = (float) win->width / (float) win->height;
    float znear = 0.1f;
    float zfar = 10000.0f;
    bcSetPerspective(fovy, aspect, znear, zfar);
    bcSetBlend(false);
    bcSetDepthTest(true);
    bcIdentity();
}

void bcPrepareScene2D()
{
}

void bcPrepareSceneGUI()
{
    BCWindow *win = bcGetWindow();
    bcSetOrtho(0, win->width, win->height, 0, -1, 1);
    bcSetBlend(true);
    bcSetDepthTest(false);
    bcIdentity();
}

// Draw 2D
void bcDrawText2D(BCFont *font, float x, float y);
void bcDrawTexture2D(BCTexture *texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh);
void bcDrawRect2D(float x, float y, float w, float h);
void bcDrawLines2D(int count, float vertices[]);

// Draw 3D
void bcDrawCube(float x, float y, float z, float size_x, float size_y, float size_z);

// Font
BCFont * bcCreateFontFromFile(const char *filename);
BCFont * bcCreateFontFromMemory(void *buffer);
void bcDestroyFont(BCFont *font);
