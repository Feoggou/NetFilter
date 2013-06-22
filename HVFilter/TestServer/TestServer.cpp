// TestServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Pipes.h"

#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	create_pipe_server();
	
	for (int i = 0; i < 1000; ++i) {
		pipe_server_write(i, i * 100);
		Sleep(100);
	}
	
	uninit_pipe_server();

	return 0;
}

