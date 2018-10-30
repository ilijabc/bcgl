#include <string.h>

#include <bcgl.h>
#include <bcgl_desktop.h>

BCCallbacks example_01_box;
BCCallbacks example_02_mesh;
BCCallbacks example_03_model;
BCCallbacks example_04_performance;
BCCallbacks example_05_lighting;
#define EXAMPLES_NUM    5

static struct
{
    BCCallbacks *callbacks;
    const char *name;
} s_Examples[EXAMPLES_NUM] = {
    { &example_01_box, "box" },
    { &example_02_mesh, "mesh" },
    { &example_03_model, "model" },
    { &example_04_performance, "performance" },
    { &example_05_lighting, "lighting" },
};

void printInfo()
{
    printf("BCGL Demos:\n");
    for (int i = 0; i < EXAMPLES_NUM; i++)
    {
        printf(" %02d   %s\n", i + 1, s_Examples[i].name);
    }
}

int main(int argc, char **argv)
{
    int example = 0;
    if (argc > 1)
    {
        int index = atoi(argv[1]);
        if (index < 0 || index > EXAMPLES_NUM)
        {
            for (int i = 0; i < EXAMPLES_NUM; i++)
            {
                if (strcmp(s_Examples[i].name, argv[1]) == 0)
                {
                    example = i;
                    break;
                }
            }
        }
        else
        {
            example = index - 1;
        }
    }
#if 1
    printf("Running example_%02d_%s\n", example + 1, s_Examples[example].name);
    bcInit(*s_Examples[example].callbacks);
    bcDesktopMain(argc, argv);
#else
    for (int i = 0; i < EXAMPLES_NUM; i++)
    {
        printf("Running example_%02d_%s\n", i + 1, s_Examples[i].name);
        bcInit(*s_Examples[i].callbacks);
        bcDesktopMain(argc, argv);
    }
#endif
    return 0;
}
