#include <string.h>

#include <bcgl.h>

struct Example
{
    BCCallbacks *callbacks;
    const char *name;
};

BCCallbacks example_game;
BCCallbacks example_01_box;
BCCallbacks example_02_mesh;
BCCallbacks example_03_model;
BCCallbacks example_04_performance;
BCCallbacks example_05_lighting;

struct Example s_Examples[] = {
    { &example_game, "game" },
    { &example_01_box, "box" },
    { &example_02_mesh, "mesh" },
    { &example_03_model, "model" },
    { &example_04_performance, "performance" },
    { &example_05_lighting, "lighting" },
    { NULL, NULL }
};

void printInfo()
{
    printf("BCGL Demos:\n");
    for (int i = 0; s_Examples[i].callbacks; i++)
    {
        printf(" %02d   %s\n", i + 1, s_Examples[i].name);
    }
}

int main(int argc, char **argv)
{
    int num_examples = sizeof(s_Examples) / sizeof(struct Example) - 1;
    int example = 1;
    if (argc > 1)
    {
        example = atoi(argv[1]);
        if (example < 0 || example >= num_examples)
        {
            for (int i = 0; i < num_examples; i++)
            {
                if (strcmp(s_Examples[i].name, argv[1]) == 0)
                {
                    example = i;
                    break;
                }
            }
        }
    }

    while (1)
    {
        printf("Running example_%02d_%s\n", example, s_Examples[example].name);
        bcInit(*s_Examples[example].callbacks);
        int result = bcRunMain();
        printf("Example result: %d\n", result);
        if (result == 1)
        {
            example++;
            if (example >= num_examples)
                example = 0;
        }
        else
        {
            break;
        }
    }
    printf("Quit demo!\n");

    return 0;
}
