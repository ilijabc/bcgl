#include "bcgl.h"

char * bcLoadText(const char *filename)
{
    FILE *textFile;
    char *text = NULL;
    int count = 0;
    if (filename != NULL)
    {
        textFile = fopen(filename, "rt");
        if (textFile != NULL)
        {
            fseek(textFile, 0, SEEK_END);
            count = ftell(textFile);
            rewind(textFile);
            if (count > 0)
            {
                text = (char *)malloc(sizeof(char)*(count + 1));
                count = fread(text, sizeof(char), count, textFile);
                text[count] = '\0';
            }
            fclose(textFile);
        }
        else bcLog("[%s] Text file could not be opened", filename);
    }
    return text;
}
