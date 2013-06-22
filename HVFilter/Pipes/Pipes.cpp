#include "Pipes.h"

#include <Windows.h>

const char* PIPE_SERVER_NAME = "\\\\.\\pipe\\HV_NDIS_UniquePipeName";

namespace
{
	HANDLE hServerPipe = INVALID_HANDLE_VALUE;
}

void create_pipe_server()
{
	hServerPipe = CreateNamedPipeA(PIPE_SERVER_NAME, PIPE_ACCESS_OUTBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE, 
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 
		/*max inst*/1, /*out buf size*/ 100, /*in buf size*/ 100, /*def timeout*/ 100, /*sec attrs*/ NULL);

	if (hServerPipe == INVALID_HANDLE_VALUE) {
		//TODO: log error or smth
		return;
	}
}

void uninit_pipe_server()
{
	if (hServerPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(hServerPipe);
		hServerPipe = INVALID_HANDLE_VALUE;
	}
}

void pipe_server_write(ULONG count, ULONG size)
{
	ULONGLONG value = (ULONGLONG(count) << 32ULL) + size;
	BOOL result = WriteFile(hServerPipe, &value, sizeof(value), NULL, NULL);
	if (!result) {
		//todo: error.
		//DWORD dwError = GetLastError();
	}
}