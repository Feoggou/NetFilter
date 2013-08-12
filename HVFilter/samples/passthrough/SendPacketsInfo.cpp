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
C_ASSERT(sizeof(ETHERNET_HEADER) == 14);

enum {EtherType_IPv4 = 0x800, EtherType_IPv6 = 0x86DD};
enum {Protocol_Tcp = 0x06};

struct Z_IPV4_HEADER
{
	BYTE header_length:		4;
	BYTE version:	4;
	BYTE TOS;
	WORD total_length;
	WORD identification;
	WORD fragment_offset:	13;
	WORD flags:				3;
	BYTE TimeToLive;
	BYTE protocol;
	WORD header_checksum;
	DWORD source_address;
	DWORD destination_address;
};

C_ASSERT(sizeof(Z_IPV4_HEADER) == 20);
C_ASSERT(sizeof(IPV4_HEADER) == 20);

struct Z_TCP_HEADER
{
	WORD source_port;
	WORD destination_port;
	DWORD sequence_number;
	DWORD ack_number;
	WORD control_bits:	6;
	WORD reserved:		6;
	WORD data_offset:	4;
	WORD window;
	WORD checksum;
	WORD urgent_pointer;
};

C_ASSERT(sizeof(Z_TCP_HEADER) == 20);
C_ASSERT(sizeof(TCP_HDR) == 20);

typedef TCP_HDR tcp_header_t;
typedef IPV4_HEADER ipv4_header_t;
typedef Z_ETH_HEADER eth_header_t;

namespace
{
	ULONG g_dwCurrentBufferSize;
}

enum OptionKind:BYTE {OptionKind_EndOfOptions = 0x0, OptionKind_NoOption = 0x01, OptionKind_Timestamp = 0x08};

BYTE get_tcp_option_size(BYTE* buffer)
{
	BYTE* pOptionInfo = (BYTE*)buffer;
	ASSERT(pOptionInfo);

	return pOptionInfo[1];
}

DWORD read_tcp_timestamp(BYTE* buffer)
{
	//length = 10, TSval = 4 bytes, echo TSval = 4 bytes
	BYTE length = get_tcp_option_size(buffer);
	ASSERT(length == 10);

	BYTE* pTimestampOption = (BYTE*)buffer;
	ASSERT(pTimestampOption);

	//buffer = kind; buffer + 1 = length; buffer + 2 == timestamp needed (4 bytes). buffer + 6 = timestamp echo (4 bytes).
	DWORD dwTimeStamp = *(DWORD*)(pTimestampOption + 2);
	return RtlUlongByteSwap(dwTimeStamp);
}

void read_tcp_data(BYTE* buffer, DWORD offset /*padding + last option*/, DWORD data_size /*size to read as packet content*/)
{
	if (data_size > 0)
	{
		buffer += offset;

		UNREFERENCED_PARAMETER(data_size);

		BYTE* data = (BYTE*)buffer;
		ASSERT(data);
	}
}

void read_tcp_info(BYTE* buffer, tcp_header_t* pTcpHeader, WORD data_size)
{
	//1. OPTIONS
	ULONG tcp_header_bytes = pTcpHeader->th_len << 2;
	ASSERT(tcp_header_bytes >= sizeof(tcp_header_t));

	if (data_size > 0) {
		int x = 0;
		++x;
	}

	ULONG options_size = tcp_header_bytes - sizeof(tcp_header_t);

	if (options_size == 0) {
		buffer += tcp_header_bytes;
		read_tcp_data(buffer, 0, data_size);
		return;
	} else {
		buffer += sizeof(tcp_header_t);
	}

	BYTE* pOptionKind = (BYTE*)buffer;
	ASSERT(pOptionKind);

	BYTE option_kind = *pOptionKind;

	ULONG bytes_advanced = 0;
	ULONG bytes_to_advance = 0;

	while (option_kind != OptionKind_EndOfOptions && bytes_advanced < options_size)
	{
		if (option_kind == OptionKind_NoOption) {
			//no option -- padding
			bytes_to_advance = 1;

		} else if (option_kind == OptionKind_Timestamp){
			//timestamp

			//1. read timestamp for current packet.
			DWORD dwTimeStamp = read_tcp_timestamp(buffer);
			DbgPrint("timestamp: 0x%x", dwTimeStamp);
			//1.1. store the timestamp somewhere.
			//2. exit loop... or not (perhaps we also read the data, no?)

			bytes_to_advance = 10; // timestamp option size
		} else {
			bytes_to_advance = get_tcp_option_size(buffer);
		}

		buffer += bytes_to_advance;
		bytes_advanced += bytes_to_advance;

		pOptionKind = (BYTE*)buffer;
		ASSERT(pOptionKind);

		option_kind = *pOptionKind;
	}

	//end of options may not exist!!!
	if (bytes_advanced == options_size) {
		read_tcp_data(buffer, 0, data_size);
	} else if (option_kind == OptionKind_EndOfOptions) {
		//the end option byte
		++bytes_advanced;

		DWORD modulo = bytes_advanced % sizeof(DWORD);
		DWORD padding_size = (modulo ? sizeof(DWORD) - modulo : 0ul);

		read_tcp_data(buffer, padding_size, data_size);
	}
}

void read_tcp_header(BYTE* buffer, WORD offset, WORD total_ip_length)
{
	tcp_header_t* pTcpHeader = (tcp_header_t*)buffer;

	ASSERT(pTcpHeader); 

	//DbgPrint("have tcp: port destination = %d; source= %d\n", RtlUshortByteSwap(pTcpHeader->th_dport), RtlUshortByteSwap(pTcpHeader->th_sport));
	WORD tcp_size = total_ip_length - offset;
	//WORD data_and_options_size = tcp_size - (pTcpHeader->th_len << 2);
	WORD data_size = tcp_size - (pTcpHeader->th_len << 2);

	read_tcp_info(buffer, pTcpHeader, data_size);
}

void read_ip_header(BYTE* buffer)
{
	ipv4_header_t* pIpHeader = (ipv4_header_t*)buffer;
	ASSERT(pIpHeader);
	ASSERT(pIpHeader->Version == 0x04);

	WORD offset = 0;
	UINT16 total_length = 0;
	
	if (pIpHeader->Protocol == Protocol_Tcp)
	{
		//DbgPrint("have ip: destination=0x%x source=0x%x\n", pIpHeader->DestinationAddress, pIpHeader->SourceAddress);

		offset = pIpHeader->HeaderLength << 2;//sizeof(ipv4_header_t);
		ASSERT(offset >= sizeof(ipv4_header_t));
		//TODO: is offset==sizeof(ipv4_header_t) == 20?

		buffer += offset;
		total_length = RtlUshortByteSwap(pIpHeader->TotalLength);

		read_tcp_header(buffer, offset, total_length);
	}
}

void read_ethernet_header(BYTE* buffer)
{
	eth_header_t* pEthHeader = (eth_header_t*)buffer;

	if (pEthHeader->type == RtlUshortByteSwap(EtherType_IPv4))
	{
		//DbgPrint("eth type = ipv4 = 0x%x\n", pEthHeader->type);

		buffer += sizeof(eth_header_t);
		read_ip_header(buffer);
	}

	else
	{
		//DbgPrint("ethertype: 0x%x", pEthHeader->type);
	}
}

void read_eth_header(NET_BUFFER* net_buffer, ULONG buffer_size)
{
	/*eth_header_t* pEthHeader = (eth_header_t*)NdisGetDataBuffer(buffer, sizeof(eth_header_t), NULL, 1, 0);
	if (pEthHeader) {} else {
		DbgPrint("could not retrieve mac header: should have allocated storage in NdisGetDataBuffer!\n");
		return;
	}

	read_ip_header(buffer, pEthHeader);*/

	BYTE* buffer = (BYTE*)NdisGetDataBuffer(net_buffer, buffer_size, NULL, 1, 0);
	if (buffer) {
		read_ethernet_header(buffer);
	} else {
		//then perhaps it's not contiguous...

		void* alloc_mem = ExAllocatePoolWithTag(PagedPool, buffer_size, 'BteN');

		buffer = (BYTE*)NdisGetDataBuffer(net_buffer, buffer_size, alloc_mem, 1, 0);
		if (buffer) {
			read_ethernet_header(buffer);
		} else {
			DbgPrint("could not retrieve mac header: should have allocated storage in NdisGetDataBuffer!\n");
		}

		ExFreePoolWithTag(alloc_mem, 'BteN');
		//DbgPrint("could not retrieve mac header: should have allocated storage in NdisGetDataBuffer!\n");
		return;
	}
}

ULONG process_buffers(PNET_BUFFER_LIST NetBufferLists)
{
	ULONG total_size = 0;

	NET_BUFFER* buffer = NET_BUFFER_LIST_FIRST_NB(NetBufferLists);

	while (buffer) {

		ULONG buffer_size = NET_BUFFER_DATA_LENGTH(buffer);
		g_dwCurrentBufferSize = buffer_size;
		//DbgPrint("buffer size: %u = 0x%x\n", buffer_size, buffer_size);

		read_eth_header(buffer, buffer_size);

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