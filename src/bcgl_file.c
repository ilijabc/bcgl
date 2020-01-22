#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bcgl_internal.h"

#define LINE_MAX 1024

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static const char * s_ModeStr[] = { "r", "w", "a", "rb", "wb", "ab" };

#ifdef __ANDROID__
#include <android/asset_manager.h>
static AAssetManager *s_Manager = NULL;
#endif

// Must be aligned with @BCPathType
static struct
{
    char prefix[100];
    int prefix_len;
    char base_path[PATH_MAX];
} s_PathTypeMap[BC_PATH_TYPE_OTHER + 1] = {
    { ASSETS_DIR, strlen(ASSETS_DIR), "" },
    { LOCAL_DIR, strlen(LOCAL_DIR), "" },
    { EXTERNAL_DIR, strlen(EXTERNAL_DIR), "" },
    { "", 0, "" },
};


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
    bcLog("BCGL: %d.%d.%d (%d)", __BC_MAJOR, __BC_MINOR, __BC_PATCH, __BC_VERSION);
#ifdef __ANDROID__
    s_Manager = ctx;
#else
    bcSetPathForType(BC_PATH_TYPE_ASSETS, "assets/");
    bcSetPathForType(BC_PATH_TYPE_LOCAL, "./");
    bcSetPathForType(BC_PATH_TYPE_EXTERNAL, "./");
#endif
}

void bcTermFiles()
{
}

//
// FS
//

void bcSetPathForType(BCPathType type, const char *path)
{
    strcpy(s_PathTypeMap[type].base_path, path);
    bcLog("Set path of %s to: %s", s_PathTypeMap[type].prefix, s_PathTypeMap[type].base_path);
}

BCPathType bcGetTypeOfPath(const char *path)
{
    for (int i = 0; i < BC_PATH_TYPE_OTHER; i++)
    {
        if (strstr(path, s_PathTypeMap[i].prefix) == path)
            return i;
    }
    return BC_PATH_TYPE_OTHER;
}

BCPathType bcConvertPath(const char *path_in, char *path_out)
{
    BCPathType type = bcGetTypeOfPath(path_in);
    strcpy(path_out, s_PathTypeMap[type].base_path);
    strcat(path_out, path_in + s_PathTypeMap[type].prefix_len);
    // convert backslashes to slashes
    int n = strlen(path_out);
    for (int i = 0; i < n; i++)
    {
        if (path_out[i] == '\\')
        {
            path_out[i] = '/';
        }
    }
    return type;
}

bool bcFileExists(const char *filename)
{
    char path[PATH_MAX];
    BCPathType type = bcConvertPath(filename, path);
    return (access(path, F_OK) != -1);
}

bool bcCreateDir(const char *filename)
{
    char path[PATH_MAX];
    BCPathType type = bcConvertPath(filename, path);
    if (type == BC_PATH_TYPE_ASSETS)
    {
        bcLogError("Can't create dir in assets!");
        return false;
    }
    int status;
#ifdef __MINGW32__
    status = mkdir(path);
#else
    status = mkdir(path, 0777);
#endif
    return (status == 0);
}

bool bcRemoveFile(const char *filename)
{
    char path[PATH_MAX];
    BCPathType type = bcConvertPath(filename, path);
    if (type == BC_PATH_TYPE_ASSETS)
    {
        bcLogError("Can't remove file from assets!");
        return false;
    }
    int status = unlink(path);
    return (status == 0);
}

bool bcRemoveDir(const char *filename)
{
    char path[PATH_MAX];
    BCPathType type = bcConvertPath(filename, path);
    if (type == BC_PATH_TYPE_ASSETS)
    {
        bcLogError("Can't remove dir from assets!");
        return false;
    }
    int status = rmdir(path);
    return (status == 0);
}

//
// File
//

BCFile * bcOpenFile(const char *filename, BCFileMode mode)
{
    char path[PATH_MAX];
    BCPathType type = bcConvertPath(filename, path);
    bool isAsset = (type == BC_PATH_TYPE_ASSETS);
    if (isAsset && mode != BC_FILE_READ_TEXT && mode != BC_FILE_READ_DATA)
    {
        bcLogError("Assets must be opened as read-only!");
        return NULL;
    }
#ifdef __ANDROID__
    if (isAsset)
    {
        AAsset *aas = AAssetManager_open(s_Manager, path, 0);
        if (aas == NULL)
        {
            return NULL;
        }
        BCFile *file = NEW_OBJECT(BCFile);
        file->handle = aas;
        file->name = cstr_strdup(path);
        file->isDir = false;
        file->isAsset = true;
        file->length = AAsset_getLength(aas);
        return file;
    }
#endif
    off_t length = __fsize(path);
    FILE *fp = fopen(path, s_ModeStr[mode]);
    if (fp == NULL)
    {
        return NULL;
    }
    BCFile *file = NEW_OBJECT(BCFile);
    file->handle = fp;
    file->name = cstr_strdup(path);
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
    char path[PATH_MAX];
    BCPathType type = bcConvertPath(filename, path);
#ifdef __ANDROID__
    if (type == BC_PATH_TYPE_ASSETS)
    {
        char dir_name[PATH_MAX];
        int n = strlen(path);
        if (path[strlen(path) - 1] == '/')
        {
            n--;
        }
        strncpy(dir_name, path, n);
        dir_name[n] = 0;
        AAssetDir *aas = AAssetManager_openDir(s_Manager, dir_name);
        if (aas == NULL)
        {
            return NULL;
        }
        BCFile *file = NEW_OBJECT(BCFile);
        file->handle = aas;
        file->name = cstr_strdup(path);
        file->isDir = true;
        file->isAsset = true;
        file->length = 0;
        return file;
    }
#endif
    DIR *d = opendir(path);
    if (d == NULL)
    {
        return NULL;
    }
    BCFile *file = NEW_OBJECT(BCFile);
    file->handle = d;
    file->name = cstr_strdup(path);
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
    BCFile * file = bcOpenFile(filename, BC_FILE_READ_DATA);
    if (file == NULL)
    {
        // bcLogWarning("Text file '%s' not found!", filename);
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
