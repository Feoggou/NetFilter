#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

extern const char* PIPE_SERVER_NAME;

extern "C" {
	void create_pipe_server();
	void uninit_pipe_server();
	void pipe_server_write(ULONG count, ULONG size);
}