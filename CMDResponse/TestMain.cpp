#include <stdio.h>
#include <tchar.h>
#include <windows.h>

//
// Defines
//
#define BUFFER_SIZE (4 * 1024 * 1024) // 4MB
#define COMMAND_TIMEOUT (100) //dwMilliseconds

DWORD CMDResponse(PCTSTR* cmdline, PSTR buf, DWORD size, DWORD timeout);

//
// Test Main
//
INT _tmain
(
    INT argc, _TCHAR* argv[]
)
{
    HANDLE hBuffer;
    PSTR buffer;
    TCHAR cmdline[] = _T("tasklist");
    BOOL result;

    printf("Start...\n");

    hBuffer = HeapCreate(NULL, 0, 0);
    if (hBuffer == NULL) {
        fprintf(stderr, "Cannot create handle\n");
        return(1);
    }

    buffer = (PSTR)HeapAlloc(
        hBuffer,
        HEAP_ZERO_MEMORY,
        BUFFER_SIZE
    );

    if (buffer == NULL) {
        fprintf(stderr, "Cannot allocate buffer, size=%d\n", BUFFER_SIZE);
        return(1);
    }

    result = CMDResponse(
        (PCTSTR*)cmdline,
        buffer,
        BUFFER_SIZE,
        COMMAND_TIMEOUT
    );

    if (result == 0) {
        fprintf(stderr, "CMDResponse error\n");
        return(1);
    }

    printf("CMDResponse:<<%s>> length:%d\n", buffer, result);

#if _OUTPUT_DEBUG_
    for (INT i = 0; i < 32; i++) {
        printf(" %02X", (BYTE)buffer[i]);
    }
    printf("\n");
#endif

    //
    // Output to file
    //
    DWORD writtenSize = 0;

    HANDLE h = CreateFile(
        L"C:\\Windows\\Temp\\OutFile.txt",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (h == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "CreateFile error!\n");
        return(1);
    }


    if (WriteFile(h, buffer, result, &writtenSize, NULL) == 0) {
        fprintf(stderr, "WriteFile error!\n");
        return(1);
    }
    CloseHandle(h);

    printf("End.\n");

    return(0);
}
