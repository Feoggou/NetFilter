#include "DataDeviceThread.h"
#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <fstream>

////TODO: already defined in driver
//struct PacketInfo
//{
//	ULONG ulCount;
//	ULONG ulSize;
//};

DataDeviceThread::
	DataDeviceThread(HWND hInboundPackageCountWnd, HWND hInboundPackageSizeWnd,
	HWND hOutboundPackageCountWnd, HWND hOutboundPackageSizeWnd)
	: m_hInboundPackageCountWnd(hInboundPackageCountWnd),
	m_hInboundPackageSizeWnd(hInboundPackageSizeWnd),
	m_hOutboundPackageCountWnd(hOutboundPackageCountWnd),
	m_hOutboundPackageSizeWnd(hOutboundPackageSizeWnd),
	m_hConn(INVALID_HANDLE_VALUE),
	m_bOrderStop(false)
{
}

DataDeviceThread::DataDeviceThread(void)
	: m_hInboundPackageCountWnd(NULL),
	m_hInboundPackageSizeWnd(NULL),
	m_hOutboundPackageCountWnd(NULL),
	m_hOutboundPackageSizeWnd(NULL),
	m_hConn(INVALID_HANDLE_VALUE),
	m_bOrderStop(false)
{
}


DataDeviceThread::~DataDeviceThread(void)
{
	if (m_hConn) {
		CloseHandle(m_hConn);
		m_hConn = INVALID_HANDLE_VALUE;
	}
}

bool DataDeviceThread::Connect()
{
	while (INVALID_HANDLE_VALUE == m_hConn)
	{
		m_hConn = CreateFileA("\\\\.\\OSRMSPassthroughExtData", GENERIC_READ|GENERIC_WRITE,
			NULL,NULL,OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);

		if(m_hConn == INVALID_HANDLE_VALUE) {
			std::cerr << "could not create pipe: " << GetLastError() << std::endl;
		}

		Sleep(1000);
	}

	return true;
}

void DataDeviceThread::Stop()
{
	m_bOrderStop = true;
}

void DataDeviceThread::OnStart()
{
#ifdef _DEBUG
	Sleep(10000);
#endif

	m_bOrderStop = false;
	bool first_time = true;

	if (!Connect())
		return;

	LARGE_INTEGER time0, time1, freq;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time0);

	//TODO: if exit is being requested, must exit the loop.

	//ULONGLONG value;
	int     nRetCode = 0;

	WCHAR cur_dir_path[MAX_PATH];

	GetCurrentDirectoryW(MAX_PATH, cur_dir_path);
	std::wstring path = cur_dir_path;
	path += L"\\info.txt";

	std::ofstream of(path);
	BYTE* data = new BYTE[2000 * 2];

	while (!m_bOrderStop) {
		QueryPerformanceCounter(&time1);
		double elapsed = double(time1.QuadPart - time0.QuadPart) / freq.QuadPart;
		if (elapsed >= 1) {

			//char dataBuffer[65535];
			DWORD index = 0;
			DWORD bytesWritten, bytesRead;

			//memset(&dataBuffer[0], 2, sizeof(dataBuffer));

			/*if(!WriteFile(m_hConn, &dataBuffer[0], sizeof(dataBuffer), &bytesWritten, NULL)) {

				nRetCode = GetLastError();
				CloseHandle(m_hConn);
				std::cerr << "Error Writing to Data Device. " << nRetCode << std::endl;
				return;

			}*/

			//memset(&dataBuffer[0], index, sizeof(dataBuffer));

			if(!ReadFile(m_hConn, data, 2000 * 2, &bytesRead, NULL)) {

				nRetCode = GetLastError();
				CloseHandle(m_hConn);
				std::cerr << "Error Writing to Data Device. " << nRetCode << std::endl;
				return;

			}

			BYTE inbound_source[4], inbound_destination[4], outbound_source[4], outbound_destination[4];

			memcpy(inbound_source, data, 4);
			memcpy(inbound_destination, data + 4, 4);
			WORD inbound_size = 0;
			memcpy(&inbound_size, data + 8, 2);

			of << "inbound source: " << inbound_source[0] << '.' << inbound_source[1] << '.' << inbound_source[2] << '.' << inbound_source[3] << std::endl;
			of << "inbound destination: " << inbound_destination[0] << '.' << inbound_destination[1] << '.' <<
				inbound_destination[2] << '.' << inbound_destination[3] << std::endl;
			of << "inbound data size: " << inbound_size << std::endl;

			if (inbound_size > 0) {
				char* inbound_data = new char[inbound_size];
				memcpy(&inbound_data, data + 10, inbound_size);

				of << "inbound data: " << inbound_data << std::endl << std::endl;

				delete[] inbound_data;
			}

			//----------------------------
			data += 2000;

			memcpy(outbound_source, data, 4);
			memcpy(outbound_destination, data + 4, 4);
			WORD outbound_size = 0;
			memcpy(&outbound_size, data + 8, 2);

			of << "outbound source: " << outbound_source[0] << '.' << outbound_source[1] << '.' << outbound_source[2] << '.' << outbound_source[3] << std::endl;
			of << "outbound destination: " << outbound_destination[0] << '.' << outbound_destination[1] << '.' <<
				outbound_destination[2] << '.' << outbound_destination[3] << std::endl;
			of << "outbound data size: " << outbound_size << std::endl;

			if (outbound_size > 0) {
				char* outbound_data = new char[outbound_size];
				memcpy(&outbound_data, data + 10, outbound_size);

				of << "outbound data: " << outbound_data << std::endl << std::endl;

				delete[] outbound_data;
			}

			of << "-------------------------------------" << std::endl;
		}
	}

	delete data;
}