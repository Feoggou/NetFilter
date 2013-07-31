#include "Thread.h"
#include "Debug.h"


Thread::Thread(void)
	: m_hThread(INVALID_HANDLE_VALUE)
{
}


Thread::~Thread(void)
{
	Close();
}

bool Thread::IsRunning() const
{
	return m_hThread != INVALID_HANDLE_VALUE;
}

DWORD Thread::StartProc(void* param)
{
	ASSERT(param);

	Thread* pThis = reinterpret_cast<Thread*>(param);

	pThis->OnStart();

	return 0;
}

void Thread::Start()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartProc, this, 0, 0);
}

void Thread::Stop()
{}

void Thread::Close()
{
	if (m_hThread != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

void Thread::Wait(int msecs)
{
	WaitForSingleObject(m_hThread, msecs);
}