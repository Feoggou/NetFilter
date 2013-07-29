#pragma once

#include "Thread.h"

class DataDeviceThread : public Thread
{
public:
	DataDeviceThread(HWND hPackageCountWnd, HWND hPackageSizeWnd);
	DataDeviceThread();
	~DataDeviceThread(void);

	void SetControls(HWND hPackageCountWnd, HWND hPackageSizeWnd)
	{m_hPackageCountWnd = hPackageCountWnd; m_hPackageSizeWnd = hPackageSizeWnd;}

private:
	void OnStart() override;
	bool Connect();

private:
	HWND		m_hPackageCountWnd;
	HWND		m_hPackageSizeWnd;

	HANDLE		m_hConn;

	char		m_sCountText[20];
	char		m_sSizeText[20];
};

