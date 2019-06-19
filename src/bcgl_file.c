#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bcgl_internal.h"

#define LINE_MAX 1024

static const char * s_ModeStr[] = { "rt", "wt", "at", "rb", "wb", "ab" };

#ifdef __ANDROID__
#include <android/asset_manager.h>
static AAssetManager *s_Manager = NULL;
#endif

static char * __strdup(const char *str)
{
    int len = strlen(str);
    char *result = malloc(len + 1);
    memcpy(result, str, len);
    result[len] = 0;
    return result;
}

static off_t __fsize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

//
// Init
//

void bcInitFiles(void *ctx)
{
#ifdef __ANDROID__
    s_Manager = ctx;
#endif
    bcLog("BCGL: %d.%d.%d (%d)", __BC_MAJOR, __BC_MINOR, __BC_PATCH, __BC_VERSION);
}

void bcTermFiles()
{
}

//
// File
//

BCFile * bcOpenFile(const char *filename, BCFileMode mode)
{
    bool isAsset = (strstr(filename, ASSETS_DIR) == filename);
    if (isAsset && mode != BC_FILE_READ_TEXT && mode != BC_FILE_READ_DATA)
    {
        bcLogError("Assets must be opened as read-only!");
        return NULL;
    }
#ifdef __ANDROID__
    if (isAsset)
    {
        AAsset *aas = AAssetManager_open(s_Manager, filename + strlen(ASSETS_DIR), 0);
        if (aas == NULL)
            return NULL;
        BCFile *file = NEW_OBJECT(BCFile);
        file->handle = aas;
        file->name = __strdup(filename);
        file->isDir = false;
        file->isAsset = true;
        file->length = AAsset_getLength(aas);
        return file;
    }
#endif
    off_t length = __fsize(filename);
    FILE *fp = fopen(filename, s_ModeStr[mode]);
    if (fp == NULL)
        return NULL;
    BCFile *file = NEW_OBJECT(BCFile);
    file->handle = fp;
    file->name = __strdup(filename);
    file->isDir = false;
    file->isAsset = isAsset;
    file->length = length;
    return file;
}

void bcCloseFile(BCFile *file)
{
    if (file == NULL)
        return;
#ifdef __ANDROID__
    if (file->isAsset)
    {
        AAsset_close(file->handle);
        file->handle = NULL;
    }
#endif
    if (file->handle)
        fclose(file->handle);
    free(file->name);
    free(file->aux);
    free(file);
}

size_t bcReadFile(BCFile *file, void* buf, size_t count)
{
    if (file == NULL)
        return 0;
#ifdef __ANDROID__
    if (file->isAsset)
        return AAsset_read(file->handle, buf, count);
#endif
    return fread(buf, 1, count, file->handle);
}

size_t bcWriteFile(BCFile *file, void* buf, size_t count)
{
    if (file == NULL)
        return 0;
    if (file->isAsset || file->isDir)
    {
        bcLogError("Can't write to asset/dir file!");
        return 0;
    }
    return fwrite(buf, 1, count, file->handle);
}

size_t bcSeekFile(BCFile *file, size_t offset)
{
    if (file == NULL)
        return 0;
#ifdef __ANDROID__
    if (file->isAsset)
        return AAsset_seek(file->handle, offset, SEEK_SET);
#endif
    return fseek(file->handle, offset, SEEK_SET);
}

size_t bcGetFilePosition(BCFile *file)
{
    if (file == NULL)
        return 0;
#ifdef __ANDROID__
    if (file->isAsset)
        return AAsset_getRemainingLength(file->handle);
#endif
    return ftell(file->handle);
}

const char * bcReadFileLine(BCFile *file)
{
    if (file->aux == NULL)
        file->aux = (char *) malloc(LINE_MAX);
    char *line = (char *) file->aux;
    int i = 0;
    char c;
    if (bcReadFile(file, &c, 1) == 0)
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
    } while (bcReadFile(file, &c, 1));
    line[i] = '\0';
    return line;
}

//
// Dir
//

BCFile * bcOpenDir(const char *filename)
{
    bool isAsset = (strstr(filename, ASSETS_DIR) == filename);
#ifdef __ANDROID__
    if (isAsset)
    {
        AAssetDir *aas = AAssetManager_openDir(s_Manager, filename + strlen(ASSETS_DIR));
        if (aas == NULL)
            return NULL;
        BCFile *file = NEW_OBJECT(BCFile);
        file->handle = aas;
        file->name = __strdup(filename);
        file->isDir = true;
        file->isAsset = true;
        file->length = 0;
        return file;
    }
#endif
    DIR *d = opendir(filename);
    if (d == NULL)
        return NULL;
    BCFile *file = NEW_OBJECT(BCFile);
    file->handle = d;
    file->name = __strdup(filename);
    file->isDir = true;
    file->isAsset = false;
    file->length = 0;
    return file;
}

void bcCloseDir(BCFile *file)
{
    if (file == NULL)
        return;
#ifdef __ANDROID__
    if (file->isAsset)
    {
        AAssetDir_close(file->handle);
        file->handle = NULL;
    }
#endif
    if (file->handle)
        closedir(file->handle);
    free(file->name);
    free(file->aux);
    free(file);
}

void bcRewindDir(BCFile *file)
{
    if (file == NULL)
        return;
#ifdef __ANDROID__
    if (file->isAsset)
    {
        AAssetDir_rewind(file->handle);
        return;
    }
#endif
    rewinddir(file->handle);
}

const char * bcGetNextFileName(BCFile *file)
{
    if (file == NULL)
        return NULL;
#ifdef __ANDROID__
    if (file->isAsset)
        return AAssetDir_getNextFileName(file->handle);;
#endif
    struct dirent *dir = readdir(file->handle);
    if (dir == NULL)
        return NULL;
    if (dir->d_name[0] == '.') // skip hidden files
        return bcGetNextFileName(file);
    return dir->d_name;
}

char * bcLoadTextFile(const char *filename, int *out_size)
{
    if (filename == NULL)
        return NULL;
    BCFile * file = bcOpenFile(filename, BC_FILE_READ_TEXT);
    if (file == NULL)
    {
        bcLogWarning("Text file '%s' not found!", filename);
        return NULL;
    }
    char *text = (char *) malloc(sizeof(char) * (file->length + 1));
    bcReadFile(file, text, file->length);
    text[file->length] = '\0';
    if (out_size)
        *out_size = file->length + 1;
    bcCloseFile(file);
    return text;
}

unsigned char * bcLoadDataFile(const char *filename, int *out_size)
{
    if (filename == NULL)
        return NULL;
    BCFile * file = bcOpenFile(filename, BC_FILE_READ_DATA);
    if (file == NULL)
    {
        bcLogWarning("Data file '%s' not found!", filename);
        return NULL;
    }
    if (file->length == 0)
    {
        bcLogWarning("Data file '%s' empty!", filename);
        bcCloseFile(file);
        return NULL;
    }
    unsigned char *out = (unsigned char *) malloc(sizeof(unsigned char) * file->length);
    bcReadFile(file, out, file->length);
    if (out_size)
        *out_size = file->length;
    bcCloseFile(file);
    return out;
}
