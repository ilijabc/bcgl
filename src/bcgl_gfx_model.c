#include "bcgl_internal.h"

#define INVALID_PART    -1
#define ALL_PARTS       -2

BCModel * bcCreateModel(BCMesh *mesh, BCMaterial material, int parts)
{
    BCModel *model = NEW_OBJECT(BCModel);
    model->mesh = mesh;
    model->material = material;
    if (parts)
    {
        model->parts_count = parts;
        model->parts_list = NEW_ARRAY(parts, BCMeshPart);
    }
    return model;
}

BCModel * bcCreateModelFromFile(const char *filename)
{
    BCModel *model = NULL;
    // TODO: load model from file (.obj, .3ds, ...)
    return model;
}

void bcDestroyModel(BCModel *model)
{
    if (model == NULL)
    {
        bcLogError("Invalid model!");
        return;
    }
    bcDestroyMesh(model->mesh);
    if (model->parts_list)
    {
        for (int i = 0; i < model->parts_count; i++)
        {
            if (model->parts_list[i].name)
                free(model->parts_list[i].name);
        }
        free(model->parts_list);
    }
    free(model);
}

void bcDrawModel(BCModel *model)
{
    if (model == NULL)
    {
        bcLogError("Invalid model!");
        return;
    }
    bcDrawModelPart(model, ALL_PARTS);
}

void bcBeginModelDraw(BCModel *model)
{
    if (model == NULL)
    {
        bcLogError("Invalid model!");
        return;
    }
    g_Context->CurrentModel = model;
    bcSetMaterial(model->material);
    bcBindMesh(model->mesh);
}

void bcEndModelDraw(BCModel *model)
{
    if (model == NULL)
    {
        bcLogError("Invalid model!");
        return;
    }
    // bcBindMesh(NULL);
    bcResetMaterial();
    g_Context->CurrentModel = NULL;
}

void bcDrawModelPart(BCModel *model, int part)
{
    if (model == NULL)
    {
        bcLogError("Invalid model!");
        return;
    }
    bool autoEnd = false;
    if (g_Context->CurrentModel == NULL)
    {
        bcBeginModelDraw(model);
        autoEnd = true;
    }
    if (part == ALL_PARTS)
        bcDrawMesh(model->mesh);
    else if (part >= 0 && part < model->parts_count)
        bcDrawMeshRange(model->mesh, model->parts_list[part].start, model->parts_list[part].count);
    if (autoEnd)
    {
        bcEndModelDraw(model);
    }
}

int bcGetModelPartByName(BCModel *model, const char *name)
{
    if (model->parts_list)
    {
        for (int i = 0; i < model->parts_count; i++)
        {
            if (model->parts_list[i].name && strcmp(model->parts_list[i].name, name) == 0)
                return i;
        }
    }
    return INVALID_PART;
}
