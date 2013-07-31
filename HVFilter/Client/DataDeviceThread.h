#pragma once

#include "Thread.h"

class DataDeviceThread : public Thread
{
public:
	DataDeviceThread(HWND hInboundPackageCountWnd, HWND hInboundPackageSizeWnd,
		HWND hOutboundPackageCountWnd, HWND hOutboundPackageSizeWnd);
	DataDeviceThread();
	~DataDeviceThread(void);

	void SetControls(HWND hInboundPackageCountWnd, HWND hInboundPackageSizeWnd, 
		HWND hOutboundPackageCountWnd, HWND hOutboundPackageSizeWnd)
	{
			m_hInboundPackageCountWnd = hInboundPackageCountWnd;
			m_hInboundPackageSizeWnd = hInboundPackageSizeWnd;

			m_hOutboundPackageCountWnd = hOutboundPackageCountWnd;
			m_hOutboundPackageSizeWnd = hOutboundPackageSizeWnd;
	}

	void Stop() override;

private:
	void OnStart() override;
	bool Connect();

private:
	HWND		m_hInboundPackageCountWnd;
	HWND		m_hInboundPackageSizeWnd;

	HWND		m_hOutboundPackageCountWnd;
	HWND		m_hOutboundPackageSizeWnd;

	HANDLE		m_hConn;

	char		m_sInboundCountText[20];
	char		m_sInboundSizeText[20];

	char		m_sOutboundCountText[20];
	char		m_sOutboundSizeText[20];

	bool		m_bOrderStop;
};

