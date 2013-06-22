#include "Pipes.h"

const char* PIPE_SERVER_NAME = "\\\\.\\pipe\\HV_NDIS_UniquePipeName";

namespace
{
	HANDLE hServerPipe = INVALID_HANDLE_VALUE;
	HANDLE hEvent = INVALID_HANDLE_VALUE;
	OVERLAPPED event_overlapped;
}

void create_pipe_server()
{
	hServerPipe = CreateNamedPipeA(PIPE_SERVER_NAME, PIPE_ACCESS_OUTBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE /*| FILE_FLAG_OVERLAPPED*/, 
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 
		/*max inst*/1, /*out buf size*/ 100, /*in buf size*/ 100, /*def timeout*/ 100, /*sec attrs*/ NULL);

	if (hServerPipe == INVALID_HANDLE_VALUE) {
		//TODO: log error or smth
		return;
	}

	hEvent = CreateEvent(/*sec attrs*/ NULL, /*man reset*/ TRUE, /*signaled*/ TRUE, /*name*/ NULL);
	if (!hEvent) {
		//TODO: log error or smth
		uninit_pipe_server();
		return;
	}

	event_overlapped.hEvent = hEvent;

	/*BOOL connected = ConnectNamedPipe(hServerPipe, &event_overlapped);
	if (!connected) {
		DWORD dwError = GetLastError();
		if (!(dwError == ERROR_IO_PENDING || dwError == ERROR_PIPE_CONNECTED)) {
			uninit_pipe_server();
			return;
		}
	}*/
}

void uninit_pipe_server()
{
	if (hEvent != INVALID_HANDLE_VALUE) {
		CloseHandle(INVALID_HANDLE_VALUE);
		hEvent = INVALID_HANDLE_VALUE;
	}

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
		DWORD dwError = GetLastError();
		int x = 0;
		++x;
		//TODO: log or smth
	}
}