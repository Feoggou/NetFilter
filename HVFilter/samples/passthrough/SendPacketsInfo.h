#pragma once

#include "precomp.h"

#ifdef __cplusplus
extern "C" {
#endif

void push_buffers_info_lists_inbound(PNET_BUFFER_LIST NetBufferLists);
void push_buffers_info_lists_outbound(PNET_BUFFER_LIST NetBufferLists);

#ifdef __cplusplus
}
#endif