#include "DataDeviceThread.h"
#include "Application.h"

#include <cstdlib>
#include <iostream>

//TODO: already defined in driver
struct PacketInfo
{
	ULONG ulCount;
	ULONG ulSize;
};

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
	Sleep(10000);

	m_bOrderStop = false;

	if (!Connect())
		return;

	LARGE_INTEGER time0, time1, freq;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time0);

	//TODO: if exit is being requested, must exit the loop.

	//ULONGLONG value;
	int     nRetCode = 0;

	while (!m_bOrderStop) {
		QueryPerformanceCounter(&time1);
		double elapsed = double(time1.QuadPart - time0.QuadPart) / freq.QuadPart;
		if (elapsed >= 1) {

			//char dataBuffer[65535];
			DWORD index = 0;
			DWORD bytesWritten, bytesRead;
			
			PacketInfo packet_info[2];

			//memset(&dataBuffer[0], 2, sizeof(dataBuffer));

			/*if(!WriteFile(m_hConn, &dataBuffer[0], sizeof(dataBuffer), &bytesWritten, NULL)) {

				nRetCode = GetLastError();
				CloseHandle(m_hConn);
				std::cerr << "Error Writing to Data Device. " << nRetCode << std::endl;
				return;

			}*/

			//memset(&dataBuffer[0], index, sizeof(dataBuffer));

			if(!ReadFile(m_hConn, &packet_info, sizeof(packet_info), &bytesRead, NULL)) {

				nRetCode = GetLastError();
				CloseHandle(m_hConn);
				std::cerr << "Error Writing to Data Device. " << nRetCode << std::endl;
				return;

			}

			time0 = time1;

			//WM_SETTEXT
			_ultoa(packet_info[0].ulCount, m_sInboundCountText, 10);
			_ultoa(packet_info[0].ulSize, m_sInboundSizeText, 10);

			_ultoa(packet_info[1].ulCount, m_sOutboundCountText, 10);
			_ultoa(packet_info[1].ulSize, m_sOutboundSizeText, 10);

			SetWindowTextA(m_hInboundPackageCountWnd, m_sInboundCountText);
			SetWindowTextA(m_hInboundPackageSizeWnd, m_sInboundSizeText);

			SetWindowTextA(m_hOutboundPackageCountWnd, m_sOutboundCountText);
			SetWindowTextA(m_hOutboundPackageSizeWnd, m_sOutboundSizeText);
		}
	}
}