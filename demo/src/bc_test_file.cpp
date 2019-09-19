#include <assert.h>
#include <string.h>

#include <bcgl_file.h>

static const char *files_list[] = {
    "lines_lin.txt",
    "lines_mac.txt",
    "lines_win.txt",
    NULL
};

void BC_test_file()
{
    printf("=== Begin test bcgl_file ===\n");


    // directory list
    {
        printf("TEST 1: directory list\n");
        BCFile *dir = bcOpenDir(ASSETS_DIR"test_dir");
        assert(dir);
        int i = 0;
        for (const char *file = bcGetNextFileName(dir); file; file = bcGetNextFileName(dir))
        {
            assert(strcmp(file, files_list[i]) == 0);
            i++;
        }
        bcCloseDir(dir);
        // void bcRewindDir(BCFile *file);
        printf(">> Success\n");
    }

    // read text
    {
        printf("TEST 2: read text\n");
        for (int i = 0; files_list[i]; i++)
        {
            char filename[256];
            int out_size;
            sprintf(filename, ASSETS_DIR"test_dir/%s", files_list[i]);
            char *text = bcLoadTextFile(filename, &out_size);
            printf("> filename=%s out_size=%d len=%d\n", filename, out_size, strlen(text));
            printf("%s<\n", text);
            free(text);
        }
        printf(">> Success\n");
    }

    printf("=== End test bcgl_file ===\n");
    exit(0);
}
