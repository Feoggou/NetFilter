#include "SendPacketsInfo.h"
#include "Pipes.h"

class FastMutexLocker {
public:
	explicit FastMutexLocker(PFAST_MUTEX pMutex) : m_pMutex(pMutex) { Acquire(); }
	~FastMutexLocker() { Release(); }

private:
	void Acquire() { ExAcquireFastMutex(m_pMutex); }
	void Release() { ExReleaseFastMutex(m_pMutex); }

private:
	PFAST_MUTEX	m_pMutex;
};

namespace {
	/*FAST_MUTEX g_inbound_mutex;
	FAST_MUTEX g_outbound_mutex;*/

	PacketInfo g_inbound_data = {0};
	PacketInfo g_outbound_data = {0};
}

void init_io_data()
{
	/*ExInitializeFastMutex(&g_inbound_mutex);
	ExInitializeFastMutex(&g_outbound_mutex);*/
}

void uninit_io_data()
{
}

void add_io_data(ULONG count, ULONG size, BOOLEAN is_inbound)
{
	if (is_inbound) {
		//FastMutexLocker lock(&g_inbound_mutex);

		g_inbound_data.ulCount += count;
		g_inbound_data.ulSize += size;
	} else {
		//FastMutexLocker lock(&g_outbound_mutex);

		g_outbound_data.ulCount += count;
		g_outbound_data.ulSize += size;
	}
}

void retrieve_io_data(__out PacketInfo* inbound, __out PacketInfo* outbound)
{
	//inbound
	//ExAcquireFastMutex(&g_inbound_mutex);

	inbound->ulSize = g_inbound_data.ulSize;
	inbound->ulCount = g_inbound_data.ulCount;

	//RtlZeroMemory(&g_inbound_data, sizeof(g_inbound_data));
	//ExReleaseFastMutex(&g_inbound_mutex);

	//outbound
	//ExAcquireFastMutex(&g_outbound_mutex);

	outbound->ulSize = g_outbound_data.ulSize;
	outbound->ulCount = g_outbound_data.ulCount;

	//RtlZeroMemory(&g_outbound_data, sizeof(g_outbound_data));
	//ExReleaseFastMutex(&g_outbound_mutex);
}

struct Z_ETH_HEADER
{
	BYTE	destination_addr[6];
	BYTE	source_addr[6];
	WORD	type;
};

C_ASSERT(sizeof(Z_ETH_HEADER) == 14);

//C_ASSERT(sizeof(ETHERNET_HEADER) == 14);

enum {EtherType_IPv4 = 0x800, EtherType_IPv6 = 0x86DD};

struct Z_IPV4_HEADER
{
	BYTE version:	4;
	BYTE IHL:		4;
	BYTE TOS;
	WORD total_length;
	WORD identification;
	WORD flags:				3;
	WORD fragment_offset:	13;
	BYTE TTL;
	BYTE protocol;
	WORD header_checksum;
	DWORD source_address;
	DWORD destination_address;
};

C_ASSERT(sizeof(Z_IPV4_HEADER) == 20);

struct Z_TCP_HEADER
{
	WORD source_port;
	WORD destination_port;
	DWORD sequence_number;
	DWORD ack_number;
	WORD data_offset:	4;
	WORD reserved:		6;
	WORD control_bits:	6;
	WORD window;
	WORD checksum;
	WORD urgent_pointer;
};

C_ASSERT(sizeof(Z_TCP_HEADER) == 20);
//C_ASSERT(sizeof(TCP_HDR) == 20);

typedef TCP_HDR tcp_header_t;
typedef IPV4_HEADER ipv4_header_t;
typedef ETHERNET_HEADER eth_header_t;

void read_tcp_header(NET_BUFFER* buffer, ipv4_header_t* pIpHeader)
{
	WORD offset = pIpHeader->TotalLength;

	NDIS_STATUS status = NdisRetreatNetBufferDataStart(buffer, offset, 0, NULL);
	if (status == NDIS_STATUS_SUCCESS)
	{
		tcp_header_t* pTcpHeader = (tcp_header_t*)NdisGetDataBuffer(buffer, sizeof(tcp_header_t), NULL, 1, 0);

		if (pTcpHeader) {
			DbgPrint("have tcp: port destination = 0x%x; source= 0x%x\n", pTcpHeader->th_dport, pTcpHeader->th_sport);
		} else {
			DbgPrint("could not retrieve ip4 header: should have allocated storage in NdisGetDataBuffer!\n");
		}
		NdisAdvanceNetBufferDataStart(buffer, offset, true, NULL);
	}

	else
	{
		DbgPrint("could not use NdisRetreat on ip header\n");
	}
}

void read_ip_header(NET_BUFFER* buffer, eth_header_t* pEthHeader)
{
	if (pEthHeader->Type == RtlUshortByteSwap(EtherType_IPv4))
	{
		DbgPrint("eth type = ipv4 = 0x%x\n", pEthHeader->Type);

		NDIS_STATUS status = NdisRetreatNetBufferDataStart(buffer, sizeof(eth_header_t), 0, NULL);
		if (status == NDIS_STATUS_SUCCESS)
		{
			ipv4_header_t* pIpHeader = (ipv4_header_t*)NdisGetDataBuffer(buffer, sizeof(ipv4_header_t), NULL, 1, 0);
			if (pIpHeader) {
				DbgPrint("have ip: destination=0x%x source=0x%x\n", pIpHeader->DestinationAddress, pIpHeader->SourceAddress);
				read_tcp_header(buffer, pIpHeader);
			} else {
				DbgPrint("could not retrieve ip4 header: should have allocated storage in NdisGetDataBuffer!\n");
			}
			
			NdisAdvanceNetBufferDataStart(buffer, sizeof(eth_header_t), true, NULL);
		}

		else
		{
			DbgPrint("could not use NdisRetreat on ip header\n");
		}
	}

	else
	{
		//DbgPrint("ethertype: 0x%x", pEthHeader->type);
	}

}

void read_eth_header(NET_BUFFER* buffer)
{
	eth_header_t* pEthHeader = (eth_header_t*)NdisGetDataBuffer(buffer, sizeof(eth_header_t), NULL, 1, 0);
	if (pEthHeader) {} else {
		DbgPrint("could not retrieve mac header: should have allocated storage in NdisGetDataBuffer!\n");
		return;
	}

	read_ip_header(buffer, pEthHeader);
}

ULONG process_buffers(PNET_BUFFER_LIST NetBufferLists)
{
	ULONG total_size = 0;

	NET_BUFFER* buffer = NET_BUFFER_LIST_FIRST_NB(NetBufferLists);

	while (buffer) {

		ULONG buffer_size = NET_BUFFER_DATA_LENGTH(buffer);
		DbgPrint("buffer size: %u = 0x%x\n", buffer_size, buffer_size);

		read_eth_header(buffer);

		//I'll assume there are is no total size of buffer size >= 2^32 bytes
		total_size += buffer_size;

		buffer = NET_BUFFER_NEXT_NB(buffer);
	}

	return total_size;
}

ULONG process_buffer_list(PNET_BUFFER_LIST NetBufferLists, ULONG& total_size)
{
	int count = 0;
	NET_BUFFER_LIST* buffer_list = NetBufferLists;

	total_size = 0;

	while (buffer_list) {
		//operations
		total_size += process_buffers(buffer_list);

		buffer_list = NET_BUFFER_LIST_NEXT_NBL(buffer_list);

		++count;
	}

	return count;
}

void push_buffers_info_lists_inbound(PNET_BUFFER_LIST net_buffer_lists)
{
	/*BOOLEAN is_ipv4 = NdisTestNblFlag(net_buffer_lists, NDIS_NBL_FLAGS_IS_IPV4);
	BOOLEAN is_ipv6 = NdisTestNblFlag(net_buffer_lists, NDIS_NBL_FLAGS_IS_IPV6);
	BOOLEAN is_tcp = NdisTestNblFlag(net_buffer_lists, NDIS_NBL_FLAGS_IS_TCP);*/

	//if (is_tcp && (is_ipv4 || is_ipv6))
	{
		ULONG total_size = 0;
		ULONG count = process_buffer_list(net_buffer_lists, total_size);

		add_io_data(count, total_size, /*inbound*/ true);
	}
}

void push_buffers_info_lists_outbound(PNET_BUFFER_LIST net_buffer_lists)
{
	/*BOOLEAN is_ipv4 = NdisTestNblFlag(net_buffer_lists, NDIS_NBL_FLAGS_IS_IPV4);
	BOOLEAN is_ipv6 = NdisTestNblFlag(net_buffer_lists, NDIS_NBL_FLAGS_IS_IPV6);
	BOOLEAN is_tcp = NdisTestNblFlag(net_buffer_lists, NDIS_NBL_FLAGS_IS_TCP);*/

	//if (trueis_tcp && (is_ipv4 || is_ipv6))
	{
		ULONG total_size = 0;
		ULONG count = process_buffer_list(net_buffer_lists, total_size);

		UNREFERENCED_PARAMETER(count);
		add_io_data(count, total_size, /*inbound*/ false);
	}
}