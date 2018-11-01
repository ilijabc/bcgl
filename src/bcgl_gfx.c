#include "bcgl_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

BCContext *g_Context = NULL;

#ifdef SUPPORT_GLSL

#ifdef SUPPORT_GLES
    #define GLSL_CODE_HEADER \
    "#version 100\n" \
    "precision mediump float;\n"
#else
    #define GLSL_CODE_HEADER \
    "#version 120\n"
#endif

// This must be alligned with @BCVertexAttributes
static struct
{
    enum BCVertexAttributes index;
    const char * type;
    const char * name;
} const s_ShaderAttributes[] =
{
    { VERTEX_ATTR_POSITIONS, "vec3", "a_Position" },
    { VERTEX_ATTR_NORMALS, "vec3", "a_Normal" },
    { VERTEX_ATTR_TEXCOORDS, "vec2", "a_TexCoord" },
    { VERTEX_ATTR_COLORS, "vec4", "a_Color" },
    { VERTEX_ATTR_MAX, NULL }
};

// This must be alligned with @BCShaderUniforms
static struct
{
    enum BCShaderUniforms index;
    const char * type;
    const char * name;
} const s_ShaderUniforms[] =
{
    { SHADER_UNIFORM_PROJECTION, "mat4", "u_ProjectionMatrix" },
    { SHADER_UNIFORM_MODELVIEW, "mat4", "u_ModelViewMatrix" },
    { SHADER_UNIFORM_TEXTURE, "sampler2D", "u_Texture" },
    { SHADER_UNIFORM_USETEXTURE, "bool", "u_UseTexture" },
    { SHADER_UNIFORM_ALPHAONLYTEXTURE, "bool", "u_AlphaOnlyTexture" },
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
"    v_position = (u_ModelViewMatrix * vec4(a_Position, 1)).xyz;\n" \
"    v_normal = (u_ModelViewMatrix * vec4(a_Normal, 0)).xyz;\n" \
"    v_texCoord = a_TexCoord;\n" \
"    v_color = a_Color;\n" \
"    gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4(a_Position, 1);\n" \
"}\n" \
"#endif\n" \
"#ifdef FRAGMENT\n" \
"void main()\n" \
"{\n" \
"    gl_FragColor = u_ObjectColor;\n" \
"    if (u_UseTexture)\n" \
"    {\n" \
"        vec4 tex = texture2D(u_Texture, v_texCoord);\n" \
"        if (u_AlphaTest && tex.a < 0.1)\n" \
"            discard;\n" \
"        if (u_AlphaOnlyTexture)\n" \
"            tex = vec4(1, 1, 1, tex.a);\n" \
"        gl_FragColor *= tex;\n" \
"    }\n" \
"    if (u_LightEnabled)\n" \
"    {\n" \
"        vec3 norm = normalize(v_normal);\n" \
"        vec3 lightDir = normalize(u_LightPosition - v_position);\n" \
"        float diff = max(dot(norm, lightDir), 0.0);\n" \
"        vec4 diffuse = diff * u_LightColor * u_DiffuseColor;\n" \
"        gl_FragColor *= (u_AmbientColor + diffuse);\n" \
"    }\n" \
"    if (u_VertexColorEnabled)\n" \
"    {\n" \
"        gl_FragColor *= v_color;\n" \
"    }\n" \
"}\n" \
"#endif\n" \
;

#else // SUPPORT_GLSL

// This must be alligned with @BCVertexAttributes
static struct
{
    enum BCVertexAttributes index;
    int type;
} const s_ClientStateType[] =
{
    { VERTEX_ATTR_POSITIONS, GL_VERTEX_ARRAY },
    { VERTEX_ATTR_NORMALS, GL_NORMAL_ARRAY },
    { VERTEX_ATTR_TEXCOORDS, GL_TEXTURE_COORD_ARRAY },
    { VERTEX_ATTR_COLORS, GL_COLOR_ARRAY },
    { VERTEX_ATTR_MAX, -1 }
};

#endif // SUPPORT_GLSL

// private

void bcInitGfx()
{
    g_Context = NEW_OBJECT(BCContext);
    g_Context->BackgroundColor = SET_COLOR(0.3f, 0.3f, 0.3f, 1.0f);
    g_Context->DefaultMaterial.objectColor = SET_COLOR(1, 1, 1, 1);
    g_Context->DefaultMaterial.diffuseColor = SET_COLOR(1, 1, 1, 1);
    g_Context->DefaultMaterial.ambientColor = SET_COLOR(0.2f, 0.2f, 0.2f, 1);
    g_Context->DefaultMaterial.texture = NULL;
    g_Context->VertexCounter = -1;
    g_Context->IndexCounter = -1;
#ifdef SUPPORT_GLSL
    g_Context->DefaultShader = bcCreateShaderFromCode(s_DefaultShaderCode, s_DefaultShaderCode);
    bcBindShader(NULL);
#else
    glAlphaFunc(GL_GREATER, 0.1f);
    glDisable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
#endif
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    // default state
    glClearColor(
        g_Context->BackgroundColor.r,
        g_Context->BackgroundColor.g,
        g_Context->BackgroundColor.b,
        g_Context->BackgroundColor.a);
    // gl default
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
}

void bcTermGfx()
{
    if (g_Context->ReusableSolidMesh)
        bcDestroyMesh(g_Context->ReusableSolidMesh);
    if (g_Context->ReusableCubeMesh)
        bcDestroyMesh(g_Context->ReusableCubeMesh);
#ifdef SUPPORT_GLSL
    bcDestroyShader(g_Context->DefaultShader);
#endif
    free(g_Context);
    g_Context = NULL;
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
        bcLogWarning("Image file '%s' not found!", filename);
        return NULL;
    }
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
    glUniform1i(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_USETEXTURE], texture ? 1 : 0);
    if (texture)
    {
        glUniform1i(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_TEXTURE], 0);
        glUniform1i(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_ALPHAONLYTEXTURE], texture->format == GL_ALPHA ? 1 : 0);
    }
#else
    glActiveTexture(GL_TEXTURE0);
    if (texture)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
#endif
}

// Shader

#ifdef SUPPORT_GLSL

static void bindShaderVariables(BCShader *shader)
{
    for (int i = 0; i < VERTEX_ATTR_MAX; i++)
    {
        shader->loc_attributes[i] = glGetAttribLocation(shader->programId, s_ShaderAttributes[i].name);
        if (shader->loc_attributes[i] == -1)
            bcLogWarning("Shader attribute '%s' not found!", s_ShaderAttributes[i].name);
    }
    for (int i = 0; i < SHADER_UNIFORM_MAX; i++)
    {
        shader->loc_uniforms[i] = glGetUniformLocation(shader->programId, s_ShaderUniforms[i].name);
        if (shader->loc_uniforms[i] == -1)
            bcLogWarning("Shader uniform '%s' not found!", s_ShaderUniforms[i].name);
    }
}

static GLuint loadShader(const char *code, GLenum shaderType)
{
    bcLog("type=%s", shaderType == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER");
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
            bcLogError("Compile shader:\n%s", errorLog);
        }
        else
        {
            bcLogWarning("Compile shader:\n%s", errorLog);
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

#endif // SUPPORT_GLSL

BCShader * bcCreateShaderFromFile(const char *filename)
{
    char *code = bcLoadTextFile(filename, NULL);
    if (code == NULL)
        return NULL;
    BCShader *shader = bcCreateShaderFromCode(code, code);
    free(code);
    return shader;
}

BCShader * bcCreateShaderFromCode(const char *vsCode, const char *fsCode)
{
#ifdef SUPPORT_GLSL
    BCShader *shader = NEW_OBJECT(BCShader);
    // create program
    shader->programId = glCreateProgram();
    if (shader->programId == 0)
    {
        bcLogError("Error creating shader program!");
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
        bcLogError("Link program:\n%s", errorLog);
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
        bcLogError("Validate program:\n%s", errorLog);
        goto shader_create_error;
    }
    // variables
    bindShaderVariables(shader);
    return shader;
    // error
shader_create_error:
    bcDestroyShader(shader);
#endif // SUPPORT_GLSL
    return NULL;
}

void bcDestroyShader(BCShader *shader)
{
#ifdef SUPPORT_GLSL
    if (shader == NULL)
        return;
    glDeleteShader(shader->vertexShader);
    glDeleteShader(shader->fragmentShader);
    glDeleteProgram(shader->programId);
    free(shader);
#endif
}

void bcBindShader(BCShader *shader)
{
#ifdef SUPPORT_GLSL
    if (shader == NULL)
        shader = g_Context->DefaultShader;
    g_Context->CurrentShader = shader;
    glUseProgram(shader->programId);
    // applyProjectionMatrix();
    // applyCurrentMatrix();
    bcSetMaterial(g_Context->DefaultMaterial);
#endif
}

// View State

void bcClear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, bcGetDisplayWidth(), bcGetDisplayHeight());
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
    glUniform1i(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_LIGHT_ENABLED], enabled);
    if (enabled)
    {
        glUniform4f(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_LIGHT_COLOR], 1, 1, 1, 1);
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
    glUniform3f(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_LIGHT_POSITION], x, y, z);
#else
    float lightPos[] = { x, y, z, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
#endif
}

void bcSetMaterial(BCMaterial material)
{
#ifdef SUPPORT_GLSL
    glUniform4fv(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_OBJECT_COLOR], 1, (float *) &(material.objectColor));
    glUniform4fv(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_DIFFUSE_COLOR], 1, (float *) &(material.diffuseColor));
    glUniform4fv(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_AMBIENT_COLOR], 1, (float *) &(material.ambientColor));
#else
   glColor4fv((float *) &(material.objectColor));
   glMaterialfv(GL_FRONT, GL_DIFFUSE, (float *) &(material.diffuseColor));
   glMaterialfv(GL_FRONT, GL_AMBIENT, (float *) &(material.ambientColor));
#endif
    bcBindTexture(material.texture);
}

void bcResetMaterial()
{
    bcSetMaterial(g_Context->DefaultMaterial);
}

void bcSetObjectColor(BCColor color)
{
#ifdef SUPPORT_GLSL
    glUniform4fv(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_OBJECT_COLOR], 1, (float *) &(color));
#else
    glColor4fv((float *) &(color));
#endif
}

void bcSetObjectColorf(float r, float g, float b, float a)
{
#ifdef SUPPORT_GLSL
    glUniform4f(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_OBJECT_COLOR], r, g, b, a);
#else
    glColor4f(r, g, b, a);
#endif
}

void bcSetProjectionMatrix(float *m)
{
    g_Context->ProjectionMatrix = mat4_from_array(m);
#ifdef SUPPORT_GLSL
    glUniformMatrix4fv(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_PROJECTION], 1, GL_FALSE, m);
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
    glUniformMatrix4fv(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_MODELVIEW], 1, GL_FALSE, m);
#else
    glLoadMatrixf(m);
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

float * bcGetModelViewProjectionMatrix()
{
    // TODO: if we want MVP to be available to chader we have to precalculate it on P and MV set
    g_Context->ModelViewProjectionMatrix = mat4_multiply(g_Context->ProjectionMatrix, g_Context->ModelViewMatrix);
    return g_Context->ModelViewProjectionMatrix.v;
}

// Mesh

BCMesh * bcCreateMesh(int num_vertices, int num_indices, int flags)
{
    BCMesh *mesh = NEW_OBJECT(BCMesh);
    mesh->num_vertices = num_vertices;
    mesh->num_indices = num_indices;
    mesh->format = flags;
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
    if (num_vertices)
    {
        mesh->vertices = NEW_ARRAY(mesh->num_vertices * mesh->total_comps, float);
    }
    // indices
    if (num_indices)
    {
        mesh->indices = NEW_ARRAY(num_indices, uint16_t);
    }
    // draw
    mesh->draw_mode = GL_TRIANGLES;
    mesh->draw_count = (num_indices > 0) ? num_indices : num_vertices;
    return mesh;
}

static void freeVBOs(BCMesh *mesh)
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

BCMesh * bcUploadMesh(BCMesh *mesh, enum BCVboStatus status)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return NULL;
    }
    if (mesh->vbo_status == VBO_DYNAMIC && status == VBO_DYNAMIC)
    {
        if (mesh->vbo_vertices)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
            glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->num_vertices * mesh->total_comps * sizeof(float), mesh->vertices);
            // glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        if (mesh->vbo_indices)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mesh->num_indices * sizeof(uint16_t), mesh->indices);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    else
    {
        if (mesh->vbo_status != VBO_EMPTY)
        {
            freeVBOs(mesh);
        }
        if (status != VBO_EMPTY)
        {
            if (mesh->num_vertices)
            {
                glGenBuffers(1, &(mesh->vbo_vertices));
                glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
                glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * mesh->total_comps * sizeof(float), mesh->vertices,
                        status == VBO_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
                // glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            if (mesh->num_indices)
            {
                glGenBuffers(1, &(mesh->vbo_indices));
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(uint16_t), mesh->indices,
                        status == VBO_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
                // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }
    }
    mesh->vbo_status = status;
    // reset state machine
    if (g_Context->CurrentMesh)
    {
        bcBindMesh(NULL);
    }
    return mesh;
}

BCMesh * bcCopyMesh(BCMesh *src)
{
    if (src == NULL)
    {
        bcLogError("Invalid mesh!");
        return NULL;
    }
    BCMesh *mesh = bcCreateMesh(src->num_vertices, src->num_indices, src->format);
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

void bcDestroyMesh(BCMesh *mesh)
{
    if (mesh == NULL)
    {
        bcLogError("Invalid mesh!");
        return;
    }
    freeVBOs(mesh);
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh);
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
        for (int i = 0; i < VERTEX_ATTR_MAX; i++)
        {
            glDisableVertexAttribArray(g_Context->CurrentShader->loc_attributes[i]);
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
        for (int i = 0; i < VERTEX_ATTR_MAX; i++)
        {
            if (mesh->comps[i] > 0)
            {
                glEnableVertexAttribArray(g_Context->CurrentShader->loc_attributes[i]);
                glVertexAttribPointer(g_Context->CurrentShader->loc_attributes[i], mesh->comps[i], GL_FLOAT, GL_FALSE, mesh->total_comps * sizeof(float), vert_ptr);
                vert_ptr += mesh->comps[i];
            }
            else
            {
                glDisableVertexAttribArray(g_Context->CurrentShader->loc_attributes[i]);
            }
        }
        glUniform1i(g_Context->CurrentShader->loc_uniforms[SHADER_UNIFORM_VERTEX_COLOR_ENABLED], mesh->comps[VERTEX_ATTR_COLORS]);
#else
        for (int i = 0; i < VERTEX_ATTR_MAX; i++)
        {
            if (mesh->comps[i] > 0)
            {
                glEnableClientState(s_ClientStateType[i].type);
                switch (i)
                {
                case VERTEX_ATTR_POSITIONS:
                    glVertexPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                case VERTEX_ATTR_NORMALS:
                    glNormalPointer(GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                case VERTEX_ATTR_TEXCOORDS:
                    glTexCoordPointer(mesh->comps[i], GL_FLOAT, mesh->total_comps * sizeof(float), vert_ptr);
                    break;
                case VERTEX_ATTR_COLORS:
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
    uint16_t *elem_start = (mesh->vbo_indices ? (uint16_t *) 0 : mesh->indices) + start;
    if (mesh->indices)
    {
        glDrawElements(mesh->draw_mode, count, GL_UNSIGNED_SHORT, elem_start);
    }
    else
    {
        glDrawArrays(mesh->draw_mode, start, count);
    }
}

BCMeshPart bcPartFromMesh(BCMesh *mesh)
{
    BCMeshPart part = { mesh, NULL, 0, mesh->draw_count };
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
