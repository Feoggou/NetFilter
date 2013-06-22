#pragma once

#include "Thread.h"

class PipeThread : public Thread
{
public:
	PipeThread(HWND hPackageCountWnd, HWND hPackageSizeWnd);
	PipeThread();
	~PipeThread(void);

	void SetControls(HWND hPackageCountWnd, HWND hPackageSizeWnd)
	{m_hPackageCountWnd = hPackageCountWnd; m_hPackageSizeWnd = hPackageSizeWnd;}

private:
	void OnStart() override;
	bool Connect();

private:
	HWND		m_hPackageCountWnd;
	HWND		m_hPackageSizeWnd;

	HANDLE		m_hPipeConn;

	char		m_sCountText[20];
	char		m_sSizeText[20];
};

