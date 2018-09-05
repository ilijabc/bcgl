#include "bcgl_internal.h"

char * bcLoadTextFile(const char *filename)
{
    if (filename == NULL)
        return NULL;
    FILE *fp = fopen(filename, "rt");
    if (fp == NULL)
    {
        bcLog("Text file '%s' not found!", filename);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *text = (char *) malloc(sizeof(char) * (len + 1));
    fread(text, sizeof(char), len, fp);
    text[len] = '\0';
    fclose(fp);
    return text;
}

int bcLoadDataFile(const char *filename, unsigned char **out)
{
    if (filename == NULL)
        return 0;
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        bcLog("Data file '%s' not found!", filename);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *out = (unsigned char *) malloc(sizeof(unsigned char) * len);
    fread(*out, sizeof(char), len, fp);
    fclose(fp);
    return len;
}

BCColor bcHexToColor(uint32_t rgba)
{
    BCColor color;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    color.a = (rgba & 0xff) / 255.0f;
    color.b = (rgba >> 8 & 0xff) / 255.0f;
    color.g = (rgba >> 16 & 0xff) / 255.0f;
    color.r = (rgba >> 24 & 0xff) / 255.0f;
#else
    color.r = (rgba & 0xff) / 255.0f;
    color.g = (rgba >> 8 & 0xff) / 255.0f;
    color.b = (rgba >> 16 & 0xff) / 255.0f;
    color.a = (rgba >> 24 & 0xff) / 255.0f;
#endif
    return color;
}

float bcGetRandom()
{
    return (float) rand() / (float) RAND_MAX;
}
