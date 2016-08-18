#include <stdio.h>
#include <windows.h>
#include <tchar.h>

static TCHAR *convertToShortName(TCHAR *filename);
static TCHAR *getCurrentWorkingDirectory();

int _tmain(int argc, TCHAR *argv[])
{
    TCHAR *cwd;
    int i;

    if (argc == 1) {
        cwd = getCurrentWorkingDirectory();
        puts(convertToShortName(cwd));
        free(cwd);
    }
    else {
        for (i = 1; i < argc; i++) {
            puts(convertToShortName(argv[i]));
        }
    }
    return EXIT_SUCCESS;
}

TCHAR *getCurrentWorkingDirectory()
{
    const DWORD GET_CWD_FAILURE = 0;
    DWORD requiredSize;
    TCHAR *cwd;
    DWORD result;

    if ((requiredSize = GetCurrentDirectory(0, NULL)) == GET_CWD_FAILURE) {
        fprintf(stderr, "Failed: %d\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    else {
        cwd = (TCHAR *) malloc(requiredSize * sizeof(TCHAR));
        result = GetCurrentDirectory(requiredSize, cwd);
        if (result == GET_CWD_FAILURE) {
            fprintf(stderr, "GetCurrentDirectory failed: %d\n", GetLastError());
            exit(EXIT_FAILURE);
        }
        else if (result > requiredSize) {
            fprintf(stderr, "This is fucked up.\n");
            exit(EXIT_FAILURE);
        }
    }
    return cwd;
}

TCHAR *convertToShortName(TCHAR *filename)
{
    static TCHAR shortpath[MAX_PATH+1];
    TCHAR fullpath[MAX_PATH+1];
    DWORD bufsize = MAX_PATH + 1;
    TCHAR **last = {NULL};
    DWORD shortpathlength;
    DWORD fullpathlength;

    fullpathlength = GetFullPathName(filename, bufsize, fullpath, last);

    if (fullpathlength == 0) {
        printf("Error with GetFullPathName: %d\n", GetLastError());
    }
    else if (fullpathlength > bufsize) {
        printf("Buffer size is too small for: %s", filename);
    }
    else {
        shortpathlength = GetShortPathName(fullpath, shortpath, bufsize);

        if (shortpathlength == 0) {
            printf("Error with GetShortPathName: %d\n", GetLastError());
        }
        else if (shortpathlength > bufsize) {
            printf("Buffer size is too small for: %s", fullpath);
        }
    }
    return shortpath;
}

