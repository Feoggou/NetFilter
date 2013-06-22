#include "SendPacketsInfo.h"
#include "Pipes.h"

ULONG process_buffers(PNET_BUFFER_LIST NetBufferLists)
{
	ULONG total_size = 0;

	NET_BUFFER* buffer = NET_BUFFER_LIST_FIRST_NB(NetBufferLists);

	while (buffer) {
		//process buffer
		//I'll assume there are is no total size of buffer size >= 2^32 bytes
		total_size += NET_BUFFER_DATA_LENGTH(buffer);

		buffer = NET_BUFFER_NEXT_NB(buffer);
	}

	return total_size;
}

ULONG process_buffer_list(PNET_BUFFER_LIST NetBufferLists, ULONG& total_size)
{
	NET_BUFFER_LIST* buffer_list = NetBufferLists;
	int count = 0;
	total_size = 0;

	while (buffer_list) {
		//operations
		total_size += process_buffers(buffer_list);

		buffer_list = NET_BUFFER_LIST_NEXT_NBL(NetBufferLists);

		++count;
	}

	return count;
}

void push_buffers_info_lists_inbound(PNET_BUFFER_LIST NetBufferLists)
{
	ULONG total_size = 0;
	ULONG count = process_buffer_list(NetBufferLists, total_size);

	pipe_server_write(count, total_size);
}

void push_buffers_info_lists_outbound(PNET_BUFFER_LIST NetBufferLists)
{
	ULONG total_size = 0;
	ULONG count = process_buffer_list(NetBufferLists, total_size);

	UNREFERENCED_PARAMETER(count);
	//pipe_server_write(count, total_size);
}