#include "SendPacketsInfo.h"
#include "Pipes.h"

class FastMutexLocker {
public:
	explicit FastMutexLocker(PFAST_MUTEX pMutex) : m_pMutex(pMutex) { Acquire(); }
	~FastMutexLocker() { Release(); }

private:
	void Acquire() { ExAcquireFastMutex(m_pMutex); }
	void Release() { ExAcquireFastMutex(m_pMutex); }

private:
	PFAST_MUTEX	m_pMutex;
};

namespace {
	FAST_MUTEX g_inbound_mutex;
	FAST_MUTEX g_outbound_mutex;

	PacketInfo g_inbound_data = {0};
	PacketInfo g_outbound_data = {0};
}

void init_io_data()
{
	ExInitializeFastMutex(&g_inbound_mutex);
	ExInitializeFastMutex(&g_outbound_mutex);
}

void uninit_io_data()
{
}

void add_io_data(ULONG count, ULONG size, BOOLEAN is_inbound)
{
	if (is_inbound) {
		FastMutexLocker lock(&g_inbound_mutex);

		g_inbound_data.ulCount += count;
		g_inbound_data.ulSize += size;
	} else {
		FastMutexLocker lock(&g_outbound_mutex);

		g_outbound_data.ulCount += count;
		g_outbound_data.ulSize += size;
	}
}

void retrieve_io_data(__out PacketInfo* inbound, __out PacketInfo* outbound)
{
	//inbound
	ExAcquireFastMutex(&g_inbound_mutex);

	inbound->ulSize = g_inbound_data.ulSize;
	inbound->ulCount = g_inbound_data.ulCount;

	RtlZeroMemory(&g_inbound_data, sizeof(g_inbound_data));
	ExReleaseFastMutex(&g_inbound_mutex);

	//outbound
	ExAcquireFastMutex(&g_outbound_mutex);

	outbound->ulSize = g_outbound_data.ulSize;
	outbound->ulCount = g_outbound_data.ulCount;

	RtlZeroMemory(&g_outbound_data, sizeof(g_outbound_data));
	ExReleaseFastMutex(&g_outbound_mutex);
}

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

	add_io_data(count, total_size, /*inbound*/ true);
}

void push_buffers_info_lists_outbound(PNET_BUFFER_LIST NetBufferLists)
{
	ULONG total_size = 0;
	ULONG count = process_buffer_list(NetBufferLists, total_size);

	UNREFERENCED_PARAMETER(count);
	add_io_data(count, total_size, /*inbound*/ false);
}