#pragma once

#include "bcbase.h"

typedef enum
{
    BC_FILE_READ_TEXT = 0,
    BC_FILE_WRITE_TEXT,
    BC_FILE_APPEND_TEXT,
    BC_FILE_READ_DATA,
    BC_FILE_WRITE_DATA,
    BC_FILE_APPEND_DATA,
} BCFileMode;

typedef struct
{
    void *handle;
    char *name;
    bool isDir;
    bool isAsset;
    size_t length;
    void *aux;
} BCFile;

typedef enum
{
    BC_PATH_TYPE_ASSETS,
    BC_PATH_TYPE_LOCAL,
    BC_PATH_TYPE_EXTERNAL,
    BC_PATH_TYPE_OTHER,
} BCPathType;

#ifdef __cplusplus
extern "C" {
#endif

// FS
void bcSetPathForType(BCPathType type, const char *path);
BCPathType bcGetTypeOfPath(const char *path);
BCPathType bcConvertPath(const char *path_in, char *path_out);
bool bcFileExists(const char *filename);
bool bcCreateDir(const char *filename);
bool bcRemoveFile(const char *filename);
bool bcRemoveDir(const char *filename);

// File
BCFile * bcOpenFile(const char *filename, BCFileMode mode);
void bcCloseFile(BCFile *file);
size_t bcReadFile(BCFile *file, void* buf, size_t count);
size_t bcWriteFile(BCFile *file, void* buf, size_t count);
off_t bcSeekFile(BCFile *file, off_t offset, int origin);
size_t bcGetFilePosition(BCFile *file);
const char * bcReadFileLine(BCFile *file);

#define bcPrintFile(file, format, ...) { fprintf((FILE*)(file->handle), format, ##__VA_ARGS__); }

// Dir
BCFile * bcOpenDir(const char *filename);
void bcCloseDir(BCFile *file);
void bcRewindDir(BCFile *file);
const char * bcGetNextFileName(BCFile *file);

// Data
char * bcLoadTextFile(const char *filename, int *out_size);
unsigned char * bcLoadDataFile(const char *filename, int *out_size);

#ifdef __cplusplus
}
#endif
