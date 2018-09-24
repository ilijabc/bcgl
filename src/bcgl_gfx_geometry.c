#include "bcgl_internal.h"


#ifdef SUPPORT_PAR_SHAPES
// this is a fix for already defined ARRAYSIZE in winnt.h
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define PAR_SHAPES_IMPLEMENTATION
#include "par/par_shapes.h"
#endif

BCMesh * bcCreateMeshFromShape(void *par_shape)
{
    par_shapes_mesh *shape = (par_shapes_mesh *) par_shape;
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

BCMesh * bcCreateMeshCube()
{
    BCMesh *mesh = bcCreateMesh(24, 360, MESH_FLAGS_POS3 | MESH_FLAGS_NORM | MESH_FLAGS_TEX2);
    if (bcBeginMesh(mesh, BC_QUADS))
    {
        // TODO: generate tex coords
        bcNormal3f(0, 1, 0);
        bcVertex3f( 1.0f, 1.0f, -1.0f);
        bcVertex3f(-1.0f, 1.0f, -1.0f);
        bcVertex3f(-1.0f, 1.0f,  1.0f);
        bcVertex3f( 1.0f, 1.0f,  1.0f);
        bcNormal3f(0, -1, 0);
        bcVertex3f( 1.0f, -1.0f,  1.0f);
        bcVertex3f(-1.0f, -1.0f,  1.0f);
        bcVertex3f(-1.0f, -1.0f, -1.0f);
        bcVertex3f( 1.0f, -1.0f, -1.0f);
        bcNormal3f(0, 0, 1);
        bcVertex3f( 1.0f,  1.0f, 1.0f);
        bcVertex3f(-1.0f,  1.0f, 1.0f);
        bcVertex3f(-1.0f, -1.0f, 1.0f);
        bcVertex3f( 1.0f, -1.0f, 1.0f);
        bcNormal3f(0, 0, -1);
        bcVertex3f( 1.0f, -1.0f, -1.0f);
        bcVertex3f(-1.0f, -1.0f, -1.0f);
        bcVertex3f(-1.0f,  1.0f, -1.0f);
        bcVertex3f( 1.0f,  1.0f, -1.0f);
        bcNormal3f(-1, 0, 0);
        bcVertex3f(-1.0f,  1.0f,  1.0f);
        bcVertex3f(-1.0f,  1.0f, -1.0f);
        bcVertex3f(-1.0f, -1.0f, -1.0f);
        bcVertex3f(-1.0f, -1.0f,  1.0f);
        bcNormal3f(1, 0, 0);
        bcVertex3f(1.0f,  1.0f, -1.0f);
        bcVertex3f(1.0f,  1.0f,  1.0f);
        bcVertex3f(1.0f, -1.0f,  1.0f);
        bcVertex3f(1.0f, -1.0f, -1.0f);
        bcEndMesh(mesh);
    }
    return mesh;
}

BCMesh * bcCreateMeshSphere(int slices, int stacks)
{
    par_shapes_mesh *shape = par_shapes_create_parametric_sphere(slices, stacks);
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
        vec4_t v = vec4(vert_ptr[0], vert_ptr[1], vert_ptr[2], 0);
        v = vec4_multiply_mat4(tm, v);
        vert_ptr[0] = v.x;
        vert_ptr[1] = v.y;
        vert_ptr[2] = v.z;
        if (mesh->comps[VERTEX_ATTR_NORMALS] == 3)
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
        bcLog("Invalid mesh!");
        return;
    }
    fprintf(stream, "o Dump\n");
    char line[100];
    int vp_size = mesh->comps[VERTEX_ATTR_POSITIONS];
    int vt_size = mesh->comps[VERTEX_ATTR_TEXCOORDS];
    int vn_size = mesh->comps[VERTEX_ATTR_NORMALS];
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
            bcLog("Invalid number of indicies!");
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
