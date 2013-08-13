#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "precomp.h"

//typedef struct _PacketInfo {
//	BYTE		source_ip[4];
//	BYTE		destination_ip[4];
//	ULONG		data_size;
//	//data here
//} PacketInfo;

void push_buffers_info_lists_inbound(PNET_BUFFER_LIST NetBufferLists);
void push_buffers_info_lists_outbound(PNET_BUFFER_LIST NetBufferLists);

void init_io_data();
void uninit_io_data();

extern FAST_MUTEX g_inbound_mutex;
extern FAST_MUTEX g_outbound_mutex;

extern BYTE* g_pInboundData;
extern BYTE* g_pOutboundData;

//void add_io_data(ULONG count, ULONG size, BOOLEAN is_inbound);
//retrieves the value of count & size. 
//void retrieve_io_data(__out PacketInfo* inbound, __out PacketInfo* outbound);

#ifdef __cplusplus
}
#endif