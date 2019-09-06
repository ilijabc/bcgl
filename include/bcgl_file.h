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

#ifdef __cplusplus
extern "C" {
#endif

// File
BCFile * bcOpenFile(const char *filename, BCFileMode mode);
void bcCloseFile(BCFile *file);
size_t bcReadFile(BCFile *file, void* buf, size_t count);
size_t bcWriteFile(BCFile *file, void* buf, size_t count);
size_t bcSeekFile(BCFile *file, size_t offset);
size_t bcGetFilePosition(BCFile *file);
const char * bcReadFileLine(BCFile *file);
bool bcFileExists(const char *filename);

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
