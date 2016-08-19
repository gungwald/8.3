#include <stdio.h>
#include <stdlib.h>     /* EXIT_SUCCESS, EXIT_FAILURE */
#include <windows.h>
#include <tchar.h>

static TCHAR *convertToShortName(TCHAR *fileName);
static TCHAR *getCurrentWorkingDirectory();
static void fail(TCHAR *message, DWORD errorCode);
static void printError(TCHAR *message, DWORD errorCode);
static TCHAR *getMessage(unsigned long errorCode);

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
    const DWORD GET_CURRENT_DIRECTORY_FAILURE = 0;
    DWORD requiredSize;
    TCHAR *cwd;
    DWORD result;

    requiredSize = GetCurrentDirectory(0, NULL);
    if (requiredSize == GET_CURRENT_DIRECTORY_FAILURE) {
        fprintf(stderr, "Failed: %d\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    else {
        cwd = (TCHAR *) malloc(requiredSize * sizeof(TCHAR));
        result = GetCurrentDirectory(requiredSize, cwd);
        if (result == GET_CURRENT_DIRECTORY_FAILURE) {
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

void fail(TCHAR *message, DWORD errorCode)
{
    printError(message, errorCode);
    exit(EXIT_FAILURE);
}

void printError(const TCHAR *offendingObject, unsigned long errorCode)
{
    TCHAR *message;

    message = getMessage(errorCode);
    _tfprintf(stderr, TEXT("which: \u201C%s\u201D: %s\n"), offendingObject, message);
    free(message);
}

/* This function was taken from Microsoft's Knowledge Base Article 149409
   and modified to fix the formatting. The returned string must be free'd
   by the caller. */
TCHAR *getMessage(unsigned long errorCode)
{
    HMODULE messageMocation;
    TCHAR *message;
    unsigned long messageLength;

    /* If dwLastError is in the network range, load the message source. */
    if (NERR_BASE <= errorCode && errorCode <= MAX_NERR) {
        messageLocation = LoadLibraryEx(TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE);
    }
    else {
        /* NULL means to look for the message in the system module. */
        messageLocaion = NULL;
    }

    /* Call FormatMessage to allow for message text to be acquired
       from the system or the supplied module handle. */
    messageLength = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_FROM_SYSTEM | /* Always consider system table */
            ((messageLocation != NULL) ? FORMAT_MESSAGE_FROM_HMODULE : 0),
            messageLocation, /* Module to get message from (NULL == system) */
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
            (TCHAR *) &message,
            0,
            NULL);

    if (messageLength == 0) {
        message = concatUInt32(TEXT("Error code "), errorCode);
    }
    /* If a message source was loaded, unload it. */
    if (messageLocation != NULL) {
        FreeLibrary(messageLocation);
    }
    return message;
}

