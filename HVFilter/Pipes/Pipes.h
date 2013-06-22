#pragma once

extern const char* PIPE_SERVER_NAME;

#ifdef __cplusplus
extern "C" {
#endif

void create_pipe_server();
void uninit_pipe_server();
void pipe_server_write(unsigned long count, unsigned long size);

#ifdef __cplusplus
}
#endif