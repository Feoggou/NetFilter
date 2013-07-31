#pragma once

#include "precomp.h"

typedef struct _PacketInfo {
	ULONG		ulCount;
	ULONG		ulSize;
} PacketInfo;

#ifdef __cplusplus
extern "C" {
#endif

void push_buffers_info_lists_inbound(PNET_BUFFER_LIST NetBufferLists);
void push_buffers_info_lists_outbound(PNET_BUFFER_LIST NetBufferLists);

void init_io_data();
void uninit_io_data();

void add_io_data(ULONG count, ULONG size, BOOLEAN is_inbound);
//retrieves the value of count & size. Resets the value in the global variables.
void retrieve_io_data(__out PacketInfo* inbound, __out PacketInfo* outbound);

#ifdef __cplusplus
}
#endif