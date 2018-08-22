#include "bcgl_internal.h"

#include <stb/stb_image.h>

#define MATRIX_STACK_SIZE 32

static float s_BackgroundColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
static float s_ProjectionMatrix[16];
static float s_MatrixStack[MATRIX_STACK_SIZE][16];
static int s_CurrentMatrix = 0;

#ifdef SUPPORT_GLSL
static BCShader *s_DefaultShader = NULL;
static BCShader *s_CurrentShader = NULL;
#endif

#ifdef PLATFORM_ANDROID
#define GLSL_CODE_HEADER \
"#version 100\n" \
"precision mediump float;\n"
#else
#define GLSL_CODE_HEADER \
"#version 120\n"
#endif

static struct
{
    enum BCVertexAttributes index;
    const char * type;
    const char * name;
} s_ShaderAttributes[] =
{
    { VERTEX_ATTR_POSITIONS, "vec3", "a_Position" },
    { VERTEX_ATTR_NORMALS, "vec3", "a_Normal" },
    { VERTEX_ATTR_TEXCOORDS, "vec2", "a_TexCoord" },
    { VERTEX_ATTR_COLORS, "vec4", "a_Color" },
    { VERTEX_ATTR_MAX, NULL }
};

static struct
{
    enum BCShaderUniforms index;
    const char * type;
    const char * name;
} s_ShaderUniforms[] =
{
    { SHADER_UNIFORM_PROJECTION, "mat4", "u_ProjectionMatrix" },
    { SHADER_UNIFORM_MODELVIEW, "mat4", "u_ModelViewMatrix" },
    { SHADER_UNIFORM_TEXTURE, "sampler2D", "u_Texture" },
    { SHADER_UNIFORM_USETEXTURE, "bool", "u_UseTexture" },
    { SHADER_UNIFORM_ALPHATEST, "bool", "u_AlphaTest" },
    { SHADER_UNIFORM_VERTEX_COLOR_ENABLED, "bool", "u_VertexColorEnabled" },
    { SHADER_UNIFORM_OBJECT_COLOR, "vec4", "u_ObjectColor" },
    { SHADER_UNIFORM_DIFFUSE_COLOR, "vec4", "u_DiffuseColor" },
    { SHADER_UNIFORM_AMBIENT_COLOR, "vec4", "u_AmbientColor" },
    { SHADER_UNIFORM_LIGHT_ENABLED, "bool", "u_LightEnabled" },
    { SHADER_UNIFORM_LIGHT_POSITION, "vec3", "u_LightPosition" },
    { SHADER_UNIFORM_LIGHT_COLOR, "vec4", "u_LightColor" },
    { SHADER_UNIFORM_MAX, NULL }
};

static const char s_DefaultShaderCode[] =
"varying vec3 v_position;\n" \
"varying vec3 v_normal;\n" \
"varying vec2 v_texCoord;\n" \
"varying vec4 v_color;\n" \
"#ifdef VERTEX\n" \
"void main()\n" \
"{\n" \
"    v_position = a_Position;\n" \
"    v_texCoord = a_TexCoord;\n" \
"    v_color = a_Color;\n" \
"    v_normal = a_Normal;\n" \
"    gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4(a_Position, 1);\n" \
"}\n" \
"#endif\n" \
"#ifdef FRAGMENT\n" \
"void main()\n" \
"{\n" \
"    gl_FragColor = vec4(1, 1, 1, 1);\n" \
"    if (u_LightEnabled)\n" \
"    {\n" \
"        vec3 norm = normalize(v_normal);\n" \
"        vec3 lightDir = normalize(u_LightPosition - v_position);\n" \
"        float diff = max(dot(norm, lightDir), 0.0);\n" \
"        vec4 diffuse = diff * u_LightColor * u_DiffuseColor;\n" \
"        gl_FragColor *= (u_AmbientColor + diffuse) * u_ObjectColor;\n" \
"    }\n" \
"    if (u_UseTexture)\n" \
"    {\n" \
"        vec4 tex = texture2D(u_Texture, v_texCoord);\n" \
"        if (u_AlphaTest && tex.a < 0.1)\n" \
"            discard;\n" \
"        gl_FragColor *= tex;\n" \
"    }\n" \
"    if (u_VertexColorEnabled)\n" \
"    {\n" \
"        gl_FragColor *= v_color;\n" \
"    }\n" \
"}\n" \
"#endif\n" \
;

// private

void bcInitGfx()
{
    // matrix stack
    mat4_identity(s_ProjectionMatrix);
    mat4_identity(s_MatrixStack[0]);
#ifdef SUPPORT_GLSL
    s_DefaultShader = bcCreateShaderFromCode(s_DefaultShaderCode, s_DefaultShaderCode);
    bcBindShader(NULL);
#else
    glAlphaFunc(GL_GREATER, 0.1f);
    glDisable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
#endif
    // default state
    glClearColor(s_BackgroundColor[0], s_BackgroundColor[1], s_BackgroundColor[2], s_BackgroundColor[3]);
    // gl default
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bcInitGfxDraw();
}

void bcTermGfx()
{
    bcTermGfxDraw();
#ifdef SUPPORT_GLSL
    bcDestroyShader(s_DefaultShader);
#endif
}

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
    glUniform1i(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_USETEXTURE], texture ? 1 : 0);
    glUniform1i(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_TEXTURE], 0);
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
    glUniformMatrix4fv(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_PROJECTION], 1, GL_FALSE, s_ProjectionMatrix);
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
    glUniformMatrix4fv(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_MODELVIEW], 1, GL_FALSE, s_MatrixStack[s_CurrentMatrix]);
#else
    glLoadMatrixf(s_MatrixStack[s_CurrentMatrix]);
#endif
}

static void bindShaderVariables(BCShader *shader)
{
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        shader->loc_attributes[i] = glGetAttribLocation(shader->programId, s_ShaderAttributes[i].name);
        if (shader->loc_attributes[i] == -1)
            bcLog("Shader attribute '%s' not found!", s_ShaderAttributes[i].name);
    }
    for (int i = 0; i < SHADER_UNIFORM_MAX; i++)
    {
        shader->loc_uniforms[i] = glGetUniformLocation(shader->programId, s_ShaderUniforms[i].name);
        if (shader->loc_uniforms[i] == -1)
            bcLog("Shader uniform '%s' not found!", s_ShaderUniforms[i].name);
    }
}

static GLuint loadShader(const char *code, GLenum shaderType)
{
    bcLog("loadShader...");
    // generate common shader code
    char common_code[1000] = "";
    if (shaderType == GL_VERTEX_SHADER)
    {
        strcpy(common_code, "#define VERTEX\n");
        for (int i = 0; i < VERTEX_ATTR_MAX; i++)
        {
            sprintf(common_code, "%sattribute %s %s;\n", common_code, s_ShaderAttributes[i].type, s_ShaderAttributes[i].name);
        }
    }
    else
    {
        strcpy(common_code, "#define FRAGMENT\n");
    }
    for (int i = 0; i < SHADER_UNIFORM_MAX; i++)
    {
        sprintf(common_code, "%suniform %s %s;\n", common_code, s_ShaderUniforms[i].type, s_ShaderUniforms[i].name);
    }
    // init shader source
    const char *strings[3] = { GLSL_CODE_HEADER, common_code, code };
    int lengths[3] = { (int) strlen(GLSL_CODE_HEADER), (int) strlen(common_code), (int) strlen(code) };
    // create and compile
    GLuint shaderId = glCreateShader(shaderType);
    if (shaderId == 0)
    {
        bcLog("glCreateShader failed!");
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
    char *code = bcLoadText(filename);
    if (code == NULL)
        return NULL;
    return bcCreateShaderFromCode(code, code);
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
    if (shader == NULL)
        return;
    glDeleteShader(shader->vertexShader);
    glDeleteShader(shader->fragmentShader);
    glDeleteProgram(shader->programId);
    free(shader);
}

void bcBindShader(BCShader *shader)
{
#ifdef SUPPORT_GLSL
    if (shader == NULL)
        shader = s_DefaultShader;
    s_CurrentShader = shader;
    glUseProgram(shader->programId);
    // applyProjectionMatrix();
    // applyCurrentMatrix();
    // default values
    bcSetMaterialColor(SHADER_UNIFORM_OBJECT_COLOR, 1, 1, 1, 1);
    bcSetMaterialColor(SHADER_UNIFORM_DIFFUSE_COLOR, 1, 1, 1, 1);
    bcSetMaterialColor(SHADER_UNIFORM_AMBIENT_COLOR, 0.2f, 0.2f, 0.2f, 1);
    bcSetMaterialColor(SHADER_UNIFORM_LIGHT_COLOR, 1, 1, 1, 1);
#endif
}

BCShader * bcGetShader()
{
#ifdef SUPPORT_GLSL
    return s_CurrentShader;
#else
    return NULL;
#endif
}

// View State

void bcClear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void bcSetWireframe(bool enabled)
{
    if (enabled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void bcSetLighting(bool enabled)
{
#ifdef SUPPORT_GLSL
    glUniform1i(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_LIGHT_ENABLED], enabled);
    if (enabled)
    {
        glUniform3f(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_LIGHT_COLOR], 1, 1, 1);
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
}

void bcLightPosition(float x, float y, float z)
{
#ifdef SUPPORT_GLSL
    glUniform3f(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_LIGHT_POSITION], x, y, z);
#else
    float lightPos[] = { x, y, z, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
#endif
}

void bcSetMaterialColor(int loc, float r, float g, float b, float a)
{
#ifdef SUPPORT_GLSL
    glUniform4f(s_CurrentShader->loc_uniforms[loc], r, g, b, a);
#else
    glColor4f(r, g, b, a);
#endif
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

void bcSetProjection(float *matrix)
{
    mat4_assign(s_ProjectionMatrix, matrix);
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

void bcRotatef(float deg, float x, float y, float z)
{
    float axis[3] = { x, y, z };
    float m0[16], m1[16];
    mat4_assign(m0, s_MatrixStack[s_CurrentMatrix]);
    mat4_identity(m1);
    mat4_rotation_axis(m1, axis, to_radians(deg));
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

float * bcGetMatrix()
{
    return s_MatrixStack[s_CurrentMatrix];
}

// Mesh

BCMesh * bcCreateMesh(int num_vertices, int num_indices, int flags)
{
    if (num_vertices == 0)
    {
        bcLog("Number of verticies must be a positive number!");
        return NULL;
    }
    BCMesh *mesh = NEW_OBJECT(BCMesh);
    mesh->num_vertices = num_vertices;
    mesh->num_indices = num_indices;
    // vertices
    mesh->comps[VERTEX_ATTR_POSITIONS] =
        (flags & MESH_FLAGS_POS2) ? 2 :
        (flags & MESH_FLAGS_POS3) ? 3 :
        (flags & MESH_FLAGS_POS4) ? 4 :
        0;
    mesh->comps[VERTEX_ATTR_TEXCOORDS] =
        (flags & MESH_FLAGS_TEX2) ? 2 :
        (flags & MESH_FLAGS_TEX3) ? 3 :
        0;
    mesh->comps[VERTEX_ATTR_NORMALS] =
        (flags & MESH_FLAGS_NORM) ? 3 :
        0;
    mesh->comps[VERTEX_ATTR_COLORS] =
        (flags & MESH_FLAGS_COL3) ? 3 :
        (flags & MESH_FLAGS_COL4) ? 4 :
        0;
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        mesh->total_comps += mesh->comps[i];
    }
    // vertices
    mesh->vertices = (float *) calloc(mesh->num_vertices * mesh->total_comps, sizeof(float));
    // indices
    if (num_indices)
    {
        mesh->indices = (uint16_t *) calloc(num_indices, sizeof(uint16_t));
    }
    // draw
    mesh->draw_mode = GL_TRIANGLES;
    mesh->draw_count = (num_indices > 0) ? num_indices : num_vertices;
    return mesh;
}

BCMesh * bcCompileMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLog("Invalid mesh!");
        return NULL;
    }
    // bind vertex data
    if (mesh->vert_vbo == 0)
    {
        glGenBuffers(1, &(mesh->vert_vbo));
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vert_vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * mesh->total_comps, mesh->vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
        // TODO: GL_DYNAMIC_DRAW
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vert_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->num_vertices * mesh->total_comps, mesh->vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    // bind index data
    if (mesh->num_indices > 0) {
        int indxSize = mesh->num_indices * sizeof(uint16_t);
        if (mesh->indx_vbo == 0)
        {
            glGenBuffers(1, &(mesh->indx_vbo));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indx_vbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxSize, mesh->indices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        else
        {
            // TODO: GL_DYNAMIC_DRAW
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indx_vbo);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indxSize, mesh->indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    return mesh;
}

void bcDestroyMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLog("Invalid mesh!");
        return;
    }
    free(mesh->vertices);
    free(mesh->indices);
    if (mesh->vert_vbo)
        glDeleteBuffers(1, &(mesh->vert_vbo));
    if (mesh->indx_vbo)
        glDeleteBuffers(1, &(mesh->indx_vbo));
    free(mesh);
}

void bcDrawMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLog("Invalid mesh!");
        return;
    }
#ifdef SUPPORT_GLSL
    float *vert_ptr = mesh->vertices;
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        if (mesh->comps[i] > 0)
        {
            glEnableVertexAttribArray(s_CurrentShader->loc_attributes[i]);
            glVertexAttribPointer(s_CurrentShader->loc_attributes[i], mesh->comps[i], GL_FLOAT, GL_FALSE, mesh->total_comps * sizeof(float), vert_ptr);
            vert_ptr += mesh->comps[i];
        }
    }
    glUniform1i(s_CurrentShader->loc_uniforms[SHADER_UNIFORM_VERTEX_COLOR_ENABLED], mesh->comps[VERTEX_ATTR_COLORS]);
    if (mesh->indices) {
        glDrawElements(mesh->draw_mode, mesh->draw_count, GL_UNSIGNED_SHORT, mesh->indices);
    } else {
        glDrawArrays(mesh->draw_mode, 0, mesh->draw_count);
    }
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        if (mesh->comps[i] > 0)
        {
            glDisableVertexAttribArray(s_CurrentShader->loc_attributes[i]);
        }
    }
#else
    float *vert_ptr = mesh->vertices;
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        if (mesh->comps[i] > 0)
        {
            switch (i)
            {
            case VERTEX_ATTR_POSITIONS:
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                break;
            case VERTEX_ATTR_NORMALS:
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                break;
            case VERTEX_ATTR_TEXCOORDS:
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                break;
            case VERTEX_ATTR_COLORS:
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                break;
            }
            vert_ptr += mesh->comps[i];
        }
    }
    if (mesh->indices) {
        glDrawElements(mesh->draw_mode, mesh->draw_count, GL_UNSIGNED_SHORT, mesh->indices);
    } else {
        glDrawArrays(mesh->draw_mode, 0, mesh->draw_count);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
#endif
}

void bcDumpMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLog("Invalid mesh!");
        return;
    }
    const char *bufferName[] =
    {
        "VERTEX_ATTR_POSITIONS",
        "VERTEX_ATTR_NORMALS",
        "VERTEX_ATTR_TEXCOORDS",
        "VERTEX_ATTR_COLORS",
    };
    bcLog("mesh(%p):", mesh);
    bcLog("    num_vertices=%d", mesh->num_vertices);
    bcLog("    num_indices=%d", mesh->num_indices);
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        bcLog("    comps[%s]=%d", bufferName[i], mesh->comps[i]);
    }
    bcLog("    total_comps=%d", mesh->total_comps);
#if 0
    bcLog("    vertices=%p", mesh->vertices);
    bcLog("    indices=%p", mesh->indices);
#else
    char str[10000] = "";
    for (int i = 0; i < mesh->num_vertices * mesh->total_comps; i++)
    {
        sprintf(str, "%s %.2f", str, mesh->vertices[i]);
    }
    bcLog("    vertices={%s }", str);
    strcpy(str, "");
    for (int i = 0; i < mesh->num_indices; i++)
    {
        sprintf(str, "%s %d", str, mesh->indices[i]);
    }
    bcLog("    indices={%s }", str);
    // bcLog("    indices=%p", mesh->indices);
#endif
    bcLog("    indx_vbo=%d", mesh->vert_vbo);
    bcLog("    indx_vbo=%d", mesh->indx_vbo);
    bcLog("    draw_mode=%d", mesh->draw_mode);
    bcLog("    draw_count=%d", mesh->draw_count);
}

BCMesh * bcCreateMeshFromShape(par_shapes_mesh *shape)
{
    int format = MESH_FLAGS_POS3;
    if (shape->normals)
        format |= MESH_FLAGS_NORM;
    if (shape->tcoords)
        format |= MESH_FLAGS_TEX2;
    BCMesh *mesh = bcCreateMesh(shape->npoints, shape->ntriangles * 3, format);
    float *vert_ptr = mesh->vertices;
    for (int i = 0; i < shape->npoints; i++)
    {
        int vp = 0;
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
        vert_ptr += vp;
    }
    memcpy(mesh->indices, shape->triangles, mesh->num_indices * sizeof(uint16_t));
    return mesh;
}

// Font
BCFont * bcCreateFontFromFile(const char *filename);
BCFont * bcCreateFontFromMemory(void *buffer);
void bcDestroyFont(BCFont *font);
