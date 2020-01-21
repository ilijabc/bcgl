#include "bcgl_internal.h"


char * __strdup(const char *str)
{
    int len = strlen(str);
    char *result = malloc(len + 1);
    memcpy(result, str, len);
    result[len] = 0;
    return result;
}
