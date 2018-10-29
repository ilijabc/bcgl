#include <string.h>

#include <bcgl.h>
#include <bcgl_desktop.h>

BCCallbacks get_example_box();
BCCallbacks get_example_lighting();

int main(int argc, char **argv)
{
    BCCallbacks callbacks = get_example_box();
    if (argc > 1)
    {
        if (strcmp(argv[1], "box") == 0)
            callbacks = get_example_box();
        else if (strcmp(argv[1], "lighting") == 0)
            callbacks = get_example_lighting();
    }
    bcInit(callbacks);
    return bcDesktopMain(argc, argv);
}
