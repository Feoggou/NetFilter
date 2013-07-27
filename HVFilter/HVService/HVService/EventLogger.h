#pragma once

#include <windows.h>
#include <crtdbg.h>

class EventLogger
{
public:
	explicit EventLogger(LPCWSTR wsServiceName)
		: m_wsServiceName(wsServiceName),
		m_hEventSource(NULL)
	{
		_ASSERT(!s_pInst);

		s_pInst = this;
	}

	~EventLogger()
	{
		if (m_hEventSource) {
			::DeregisterEventSource(m_hEventSource);
			m_hEventSource = NULL;
		}
	}

	void LogEvent(WORD wType, DWORD dwID, LPCWSTR pszS1 = NULL, LPCWSTR pszS2 = NULL, LPCWSTR pszS3 = NULL);

	static EventLogger* GetInst() {_ASSERT(s_pInst); return s_pInst;}

private:
	HANDLE m_hEventSource;
	LPCWSTR	m_wsServiceName;

	static EventLogger*	s_pInst;
};

#define theEvLogger EventLogger::GetInst()

#define EVLOG_ERROR(msg_id) theEvLogger->LogEvent(EVENTLOG_ERROR_TYPE, msg_id)
#define EVLOG_ERROR_STR(msg_id, wstr) theEvLogger->LogEvent(EVENTLOG_ERROR_TYPE, msg_id, wstr)

#define EVLOG_INFO(msg_id) theEvLogger->LogEvent(EVENTLOG_INFORMATION_TYPE, msg_id)
#define EVLOG_INFO_STR(msg_id, wstr) theEvLogger->LogEvent(EVENTLOG_INFORMATION_TYPE, msg_id, wstr)