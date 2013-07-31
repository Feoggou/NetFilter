#pragma once

#include "General.h"

class Thread
{
public:
	Thread(void);
	virtual ~Thread(void);

	void Start();
	bool IsRunning() const;
	void Wait(int msecs = INFINITE);
	void Close();
	virtual void Stop();

protected:
	virtual void OnStart() = 0;

private:
	static DWORD StartProc(LPVOID);

private:
	HANDLE		m_hThread;
};