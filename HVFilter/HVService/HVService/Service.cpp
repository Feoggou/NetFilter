#include "Service.h"

#include <cwchar>

#include "servmsg.h"
#include "Stuff.h"
#include "EventLogger.h"


Service* Service::s_pService = NULL;

Service::Service(const WCHAR* szServiceName,const WCHAR* szServiceDescription) 
	: m_bDebugging(FALSE),
	m_dbgMsg(szServiceName)
{
	// copy the address of the current object so we can access it from
    // the static member callback functions. 
    // WARNING: This limits the application to only one CNTService object. 
    
    // Set the default service name and version
    memset(m_szServiceName,0,sizeof(m_szServiceName));
    wcsncpy_s(m_szServiceName, szServiceName, wcslen(szServiceName));
    memset(m_szServiceDescription,0,sizeof(m_szServiceDescription));
    wcsncpy_s(m_szServiceDescription, szServiceDescription, wcslen(szServiceDescription));
    m_iMajorVersion = 1;
    m_iMinorVersion = 0;

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_Status.dwCurrentState = SERVICE_STOPPED;
    m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_Status.dwWin32ExitCode = 0;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    m_bIsRunning = FALSE;
}


Service::~Service(void)
{
	m_dbgMsg(L"CNTService::~CNTService()");
}

//////////////////////////////////////////////////////////////////////////////////////////////

void Service::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	m_dbgMsg(L"Entering CNTService::ServiceMain()");
    // Register the control request handler
    m_Status.dwCurrentState = SERVICE_START_PENDING;

#ifndef HANDLEREX
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, Handler);
#else   HANDLEREX
    m_hServiceStatus = RegisterServiceCtrlHandlerEx(m_szServiceName, HandlerEx, NULL);
#endif  HANDLEREX

    if (m_hServiceStatus == NULL) {
		m_dbgMsg(L"CNTService::ServiceMain() Service Not Registered");
		EVLOG_ERROR(EVMSG_CTRLHANDLERNOTINSTALLED);
        return;
    }

	  m_dbgMsg(L"CNTService::ServiceMain() Service Registered");

    // Start the initialisation
    if (Initialize()) {

        // Do the real work. 
        // When the Run function returns, the service has stopped.
        m_bIsRunning = TRUE;
        m_Status.dwWin32ExitCode = 0;
        m_Status.dwCheckPoint = 0;
        m_Status.dwWaitHint = 0;
        Run();
    }

    // Tell the service manager we are stopped
    SetStatus(SERVICE_STOPPED);
    m_dbgMsg(L"Leaving CNTService::ServiceMain()");
}

// static member function (callback)
void Service::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Get a pointer to the C++ object
	Service* pService = GetService();
	pService->_ServiceMain(dwArgc, lpszArgv);
}

///////////////////////////////////////////////////////////////////////////////////////////
// status functions

void Service::SetStatus(DWORD dwState)
{
    m_dbgMsg(L"CNTService::SetStatus(%lu, %lu)", m_hServiceStatus, dwState);
    m_Status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_Status);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Service initialization

BOOL Service::Initialize()
{
    m_dbgMsg(L"Entering CNTService::Initialize()");

    // Start the initialization
    SetStatus(SERVICE_START_PENDING);
    
    // Perform the actual initialization
    BOOL bResult = OnInit(); 
    
    // Set final state
    m_Status.dwWin32ExitCode = GetLastError();
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    if (!bResult) {
        EVLOG_ERROR(EVMSG_FAILEDINIT);
        SetStatus(SERVICE_STOPPED);
        return FALSE;    
    }
    
    EVLOG_INFO(EVMSG_STARTED);
    SetStatus(SERVICE_RUNNING);

    m_dbgMsg(L"Leaving CNTService::Initialize()");
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main function to do the real work of the service

// This function performs the main work of the service. 
// When this function returns the service has stopped.
void Service::Run()
{
    m_dbgMsg(L"Entering CNTService::Run()");

    while (m_bIsRunning) {
        m_dbgMsg(L"Sleeping...");
        Sleep(5000);
    }

    // nothing more to do
    m_dbgMsg(L"Leaving CNTService::Run()");
}

//////////////////////////////////////////////////////////////////////////////////////
// Control request handlers

// static member function (callback) to handle commands from the
// service control manager

DWORD Service::_HandlerEx(DWORD dwOpcode,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext)
{
	m_dbgMsg(L"CNTService::Handler(%lu)", dwOpcode);
    switch (dwOpcode) {
    case SERVICE_CONTROL_STOP: // 1
        SetStatus(SERVICE_STOP_PENDING);
        OnStop();
        m_bIsRunning = FALSE;
		EVLOG_INFO(EVMSG_STOPPED);
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        OnPause();
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        OnContinue();
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        OnInterrogate();
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        OnShutdown();
        break;

    case SERVICE_CONTROL_DEVICEEVENT:
        OnDeviceEvent(dwEventType,lpEventData);
        break;

    default:
        if (dwOpcode >= SERVICE_CONTROL_USER) {
            if (!OnUserControl(dwOpcode)) {
				EVLOG_ERROR(EVMSG_BADREQUEST);
            }
        } else {
            EVLOG_ERROR(EVMSG_BADREQUEST);
        }
        break;
    }

    // Report current status
	m_dbgMsg(L"Updating status (%lu, %lu)", m_hServiceStatus, m_Status.dwCurrentState);

    ::SetServiceStatus(m_hServiceStatus, &m_Status);

#ifdef HANDLEREX
    return NO_ERROR;
#endif HANDLEREX
}

#ifndef HANDLEREX
void Service::Handler(DWORD dwOpcode)
#else   
DWORD Service::HandlerEx(DWORD dwOpcode,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext)
#endif  
{
    // Get a pointer to the object
    Service* pService = GetService();
	return pService->_HandlerEx(dwOpcode, dwEventType, lpEventData, lpContext);
}
        
// Called when the service is first initialized
BOOL Service::OnInit()
{
    m_dbgMsg(L"CNTService::OnInit()");
	  return TRUE;
}

// Called when the service control manager wants to stop the service
void Service::OnStop()
{
    m_dbgMsg(L"CNTService::OnStop()");
}

// called when the service is interrogated
void Service::OnInterrogate()
{
    m_dbgMsg(L"CNTService::OnInterrogate()");
}

// called when the service is paused
void Service::OnPause()
{
    m_dbgMsg(L"CNTService::OnPause()");
}

// called when the service is continued
void Service::OnContinue()
{
    m_dbgMsg(L"CNTService::OnContinue()");
}

// called when the service is shut down
void Service::OnShutdown()
{
    m_dbgMsg(L"CNTService::OnShutdown()");
}

void Service::OnDeviceEvent(DWORD dwEventType,LPVOID lpEventData)
{
    m_dbgMsg(L"CNTService::OnDeviceEvent(DWORD dwEventType,LPVOID lpEventData)");
}

// called when the service gets a user control message
BOOL Service::OnUserControl(DWORD dwOpcode)
{
    m_dbgMsg(L"CNTService::OnUserControl(%8.8lXH)", dwOpcode);
    return FALSE; // say not handled
}