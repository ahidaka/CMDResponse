#include <stdio.h>
#include <tchar.h>
#include <windows.h>

//
// CMDResponse
//

DWORD CMDResponse(PCTSTR* Command, PSTR Buffer, DWORD BufferLength, DWORD Timeout)
{
	HANDLE				read;
	HANDLE				write;
	SECURITY_ATTRIBUTES	sa;
	PROCESS_INFORMATION	pi;
	STARTUPINFO 		si;
	BOOL finished = false;
	DWORD dataLength = 0;
	DWORD point = 0;

	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&read, &write, &sa, 0)) {
		fprintf(stderr, "PIPE error!\n");
		return 0;
	}

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = write;
	si.hStdError = write;

	if (CreateProcess(NULL, (TCHAR*)Command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0) {
		fprintf(stderr, "CreateProcess error\n");
		return 0;
	}

	CloseHandle(pi.hThread);

	do {
		DWORD status;
		DWORD len;
		DWORD totalLen = 0;

		status = WaitForSingleObject(pi.hProcess, Timeout);
		if (status == WAIT_OBJECT_0) {
			fprintf(stderr, "Wait end\n");
			finished = TRUE;
		}
		else {
			fprintf(stderr, "Wait error=0x%02X\n", status);
		}

		if (PeekNamedPipe(read, NULL, 0, NULL, &totalLen, NULL) == 0) {

			fprintf(stderr, "PeekNamedPipe error!\n");
			break;
		}

		if (totalLen > 0) {
			if (ReadFile(read, &Buffer[point], BufferLength - point, &len, NULL) == 0) {
				fprintf(stderr, "ReadFile error!\n");
				return 0;
			}

			printf("Read end total=%d len=%d\n", totalLen, len);

			point += len;

			if (totalLen > point) {
				fprintf(stderr, "Read pending total=%d len=%d\n", totalLen, len);
			}
		}
	} while (!finished);

	if (CloseHandle(write) == 0) {
		fprintf(stderr, "CloseHandle(writePipe)\n");
		return 0;
	}
	if (CloseHandle(read) == 0) {
		fprintf(stderr, "CloseHandle(writePipe)\n");
		return 0;
	}
	CloseHandle(pi.hProcess);

	dataLength = point;

	return dataLength;
}
