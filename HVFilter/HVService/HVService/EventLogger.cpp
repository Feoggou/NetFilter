#include "EventLogger.h"

EventLogger* EventLogger::s_pInst = NULL;

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions

// This function makes an entry into the application event log
void EventLogger::LogEvent(WORD wType, DWORD dwID, LPCWSTR pszS1, LPCWSTR pszS2, LPCWSTR pszS3)
{
    const WCHAR* ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;

    int iStr = 0;
    for (int i = 0; i < 3; i++) {
        if (ps[i] != NULL) iStr++;
    }
        
    // Check the event source has been registered and if
    // not then register it now
    if (!m_hEventSource) {
        m_hEventSource = ::RegisterEventSource(/*local machine*/ NULL, /*source name*/ m_wsServiceName);
    }

    if (m_hEventSource) {
        ::ReportEvent(m_hEventSource, wType, 0, dwID, /*sid*/ NULL, (WORD) iStr, 0, ps, NULL);
    }
}