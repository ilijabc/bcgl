#include <dirent.h>

#include "bcgl_internal.h"

#define LINE_MAX 1024

static const char * s_ModeStr[] = { "r", "w", "a" };

//
// File
//

BCFile * bcOpenFile(const char *filename, enum BCFileMode mode)
{
    bool isAsset = false;
    if (strstr(filename, "assets/") == filename)
    {
        isAsset = true;
    }
    FILE *fp = fopen(filename, s_ModeStr[mode]);
    if (fp == NULL)
        return NULL;
    BCFile *file = NEW_OBJECT(BCFile);
    file->handle = fp;
    file->name = strdup(filename);
    file->isDir = false;
    file->isAsset = isAsset;
    fseek(fp, 0, SEEK_END);
    file->length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return file;
}

void bcCloseFile(BCFile *file)
{
    if (file == NULL)
        return;
    fclose(file->handle);
    free(file->name);
    free(file->aux);
    free(file);
}

size_t bcReadFile(BCFile *file, void* buf, size_t count)
{
    if (file == NULL)
        return 0;
    return fread(buf, 1, count, file->handle);
}

size_t bcWriteFile(BCFile *file, void* buf, size_t count)
{
    if (file == NULL)
        return 0;
    return fwrite(buf, 1, count, file->handle);
}

size_t bcSeekFile(BCFile *file, size_t offset)
{
    if (file == NULL)
        return 0;
    return fseek(file->handle, offset, SEEK_SET);
}

size_t bcGetFilePosition(BCFile *file)
{
    return ftell(file->handle);
}

const char * bcReadFileLine(BCFile *file)
{
    if (file->aux == NULL)
        file->aux = (char *) malloc(LINE_MAX);
    char *line = (char *) file->aux;
    int i = 0;
    int c = fgetc(file->handle);
    if (c == EOF)
        return NULL;
    do
    {
        if (c == '\n' || c == '\r')
        {
            break;
        }
        line[i++] = c;
        if (i == LINE_MAX - 1)
        {
            bcLogWarning("Wrapping read line!");
            break;
        }
    } while ((c = fgetc(file->handle)) != EOF);
    line[i] = '\0';
    return line;
}

//
// Dir
//

BCFile * bcOpenDir(const char *path)
{
    DIR *d = opendir(path);
    if (d == NULL)
        return NULL;
    BCFile *file = NEW_OBJECT(BCFile);
    file->handle = d;
    file->name = strdup(path);
    file->isDir = true;
    file->isAsset = false;
    file->length = 0;
    file->aux = NEW_OBJECT(BCFile);
    return file;
}

void bcCloseDir(BCFile *file)
{
    if (file == NULL)
        return;
    closedir(file->handle);
    free(file->name);
    free(file->aux);
    free(file);
}

void bcRewindDir(BCFile *file)
{
    rewinddir(file->handle);
}

BCFile * bcGetNextFile(BCFile *file)
{
    struct dirent *dir = readdir(file->handle);
    if (dir == NULL)
        return NULL;
    BCFile *aux = file->aux;
    aux->handle = NULL;
    aux->name = dir->d_name;
    // aux->isDir = (dir->d_type == DT_DIR);
    aux->isAsset = false;
    aux->length = 0;
    return aux;
}

char * bcLoadTextFile(const char *filename)
{
    if (filename == NULL)
        return NULL;
    BCFile * file = bcOpenFile(filename, FILE_READ);
    if (file == NULL)
    {
        bcLog("Text file '%s' not found!", filename);
        return NULL;
    }
    char *text = (char *) malloc(sizeof(char) * (file->length + 1));
    bcReadFile(file, text, file->length);
    text[file->length] = '\0';
    bcCloseFile(file);
    return text;
}

unsigned char * bcLoadDataFile(const char *filename, int * psize)
{
    if (filename == NULL)
        return NULL;
    BCFile * file = bcOpenFile(filename, FILE_READ);
    if (file == NULL)
    {
        bcLog("Data file '%s' not found!", filename);
        return NULL;
    }
    unsigned char *out = (unsigned char *) malloc(sizeof(unsigned char) * file->length);
    bcReadFile(file, out, file->length);
    bcCloseFile(file);
    if (psize)
        *psize = file->length;
    return out;
}
