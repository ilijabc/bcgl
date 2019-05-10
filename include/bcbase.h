#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

// Version
#define __BC_MAJOR                  0
#define __BC_MINOR                  1
#define __BC_PATCH                  0
#define __BC_VERSION                (__BC_MAJOR * 10000 + __BC_MINOR * 100 + __BC_PATCH)

// Assets
#define ASSETS_DIR                  "assets/"

// Memory utils
#define NEW_OBJECT(T)               (T*)calloc(1, sizeof(T))
#define NEW_ARRAY(N,T)              (T*)calloc(N, sizeof(T))
#define EXTEND_ARRAY(P,N,T)         (T*)realloc(P, (N) * sizeof(T))

// Logging
#ifdef __ANDROID__
#define bcLog(format, ...)          __android_log_print(ANDROID_LOG_INFO, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#define bcLogDebug(format, ...)     __android_log_print(ANDROID_LOG_DEBUG, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#define bcLogWarning(format, ...)   __android_log_print(ANDROID_LOG_WARN, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#define bcLogError(format, ...)     __android_log_print(ANDROID_LOG_ERROR, "BCGL", "%s: " format, __FUNCTION__, ##__VA_ARGS__)
#else
#define bcLog(format, ...)          { printf("[INFO] " format "\n", ##__VA_ARGS__); }
#define bcLogDebug(format, ...)     { printf("[DEBUG] [%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
#define bcLogWarning(format, ...)   { printf("[WARNING] [%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
#define bcLogError(format, ...)     { printf("[ERROR] [%s:%d] %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
#endif
