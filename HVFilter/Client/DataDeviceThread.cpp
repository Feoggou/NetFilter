#include "DataDeviceThread.h"
#include "Application.h"

#include <cstdlib>
#include <iostream>


DataDeviceThread::DataDeviceThread(HWND hPackageCountWnd, HWND hPackageSizeWnd)
	: m_hPackageCountWnd(hPackageCountWnd),
	m_hPackageSizeWnd(hPackageSizeWnd),
	m_hConn(INVALID_HANDLE_VALUE)
{
}

DataDeviceThread::DataDeviceThread(void)
	: m_hPackageCountWnd(NULL),
	m_hPackageSizeWnd(NULL),
	m_hConn(INVALID_HANDLE_VALUE)
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

void DataDeviceThread::OnStart()
{
	Sleep(10000);

	if (!Connect())
		return;

	LARGE_INTEGER time0, time1, freq;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time0);

	ULONGLONG value;
	int     nRetCode = 0;

	for (;;) {
		QueryPerformanceCounter(&time1);
		double elapsed = double(time1.QuadPart - time0.QuadPart) / freq.QuadPart;
		if (elapsed >= 1) {

			char dataBuffer[65535];
			DWORD index = 0;
			DWORD bytesWritten, bytesRead;

			memset(&dataBuffer[0],index,sizeof(dataBuffer));

			if(!WriteFile(m_hConn, &dataBuffer[0], sizeof(dataBuffer), &bytesWritten, NULL)) {

				nRetCode = GetLastError();
				CloseHandle(m_hConn);
				std::cerr << "Error Writing to Data Device. " << nRetCode << std::endl;
				return;

			}

			memset(&dataBuffer[0], index, sizeof(dataBuffer));

			if(!ReadFile(m_hConn, &dataBuffer[0], sizeof(dataBuffer), &bytesRead, NULL)) {

				nRetCode = GetLastError();
				CloseHandle(m_hConn);
				std::cerr << "Error Writing to Data Device. " << nRetCode << std::endl;
				return;

			}

			time0 = time1;

			ULONG count = HIDWORD(value);
			ULONG size = LODWORD(value);
			//WM_SETTEXT
			_ultoa(count, m_sCountText, 10);
			_ultoa(size, m_sSizeText, 10);

			SetWindowTextA(m_hPackageCountWnd, m_sCountText);
			SetWindowTextA(m_hPackageSizeWnd, m_sSizeText);
		}
	}
}