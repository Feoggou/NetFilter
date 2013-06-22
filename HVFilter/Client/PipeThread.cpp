#include "PipeThread.h"
#include "Application.h"
#include <cstdlib>

//todo: duplicate string literal... there's also one in Pipes.h
#define PIPE_SERVER_NAME "\\\\.\\pipe\\HV_NDIS_UniquePipeName"

PipeThread::PipeThread(HWND hPackageCountWnd, HWND hPackageSizeWnd)
	: m_hPackageCountWnd(hPackageCountWnd),
	m_hPackageSizeWnd(hPackageSizeWnd),
	m_hPipeConn(INVALID_HANDLE_VALUE)
{}

PipeThread::PipeThread()
	: m_hPackageCountWnd(NULL),
	m_hPackageSizeWnd(NULL),
	m_hPipeConn(INVALID_HANDLE_VALUE)
{
}

bool PipeThread::Connect()
{
	while (INVALID_HANDLE_VALUE == m_hPipeConn)
	{
		m_hPipeConn = CreateFileA(PIPE_SERVER_NAME, GENERIC_READ, /*sharing*/0, /*sec attrs*/NULL, OPEN_EXISTING, 
			/*flags & attrs*/0, /*template */NULL);

		if (m_hPipeConn == INVALID_HANDLE_VALUE) {
			//TODO: error
		}

		DWORD dwError = GetLastError();
		if (dwError == ERROR_PIPE_BUSY) {
			if (!WaitNamedPipeA(PIPE_SERVER_NAME, 20000)) {
				//todo: error
				return false;
			}
		}
	}

	return true;
}

PipeThread::~PipeThread(void)
{
	if (m_hPipeConn) {
		CloseHandle(m_hPipeConn);
		m_hPipeConn = INVALID_HANDLE_VALUE;
	}
}

void PipeThread::OnStart()
{
	if (!Connect())
		return;

	LARGE_INTEGER time0, time1, freq;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time0);

	ULONGLONG value;

	for (;;) {
		DWORD cbRead = 0;
		BOOL succeeded  = ReadFile(m_hPipeConn, &value, sizeof(value), &cbRead, /*overlapped*/ NULL);

		if (!succeeded || cbRead != sizeof(value)) {
			//todo: error
			return;
		}

		QueryPerformanceCounter(&time1);
		double elapsed = double(time1.QuadPart - time0.QuadPart) / freq.QuadPart;
		if (elapsed >= 1) {
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