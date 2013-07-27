#include "ServiceController.h"
#include "servmsg.h"

#include "EventLogger.h"

////////////////////////////////////////////////////////////////////////////////////////
// Install/uninstall routines

// Test if the service is currently installed
BOOL ServiceController::IsInstalled()
{
    BOOL bResult = FALSE;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) {

        // Try to open the service
        SC_HANDLE hService = ::OpenService(hSCM,
                                           m_wsServiceName,
                                           SERVICE_QUERY_CONFIG);
        if (hService) {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }

        ::CloseServiceHandle(hSCM);
    }
    
    return bResult;
}

BOOL ServiceController::Install()
{
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    // Get the executable file path
    WCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));

    // Create the service
    SC_HANDLE hService = ::CreateService(hSCM,
                                         m_wsServiceName,
                                         m_wsServiceName,
                                         SERVICE_ALL_ACCESS,
                                         SERVICE_WIN32_OWN_PROCESS,
                                         SERVICE_DEMAND_START,        // start condition
                                         SERVICE_ERROR_NORMAL,
                                         szFilePath,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL);
    if (!hService) {
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    // make registry entries to support logging messages
    // Add the source name as a subkey under the Application
    // key in the EventLog service portion of the registry.
    WCHAR szKey[256];
    HKEY hKey = NULL;
    wcscpy_s(szKey, L"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
    wcscat_s(szKey, m_wsServiceName);
    if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
    ::RegSetValueEx(hKey,
                    L"EventMessageFile",
                    0,
                    REG_EXPAND_SZ, 
                    (CONST BYTE*)szFilePath,
                    wcslen(szFilePath) + sizeof(WCHAR));     

    // Set the supported types flags.
    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    ::RegSetValueEx(hKey,
                    L"TypesSupported",
                    0,
                    REG_DWORD,
                    (CONST BYTE*)&dwData,
                     sizeof(DWORD));
    ::RegCloseKey(hKey);
	EVLOG_INFO_STR(EVMSG_INSTALLED, m_wsServiceName);

    // Add our description
    wcscpy_s(szKey,L"SYSTEM\\CurrentControlSet\\Services\\");
    wcscat_s(szKey, m_wsServiceName);

    if(::RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey) == ERROR_SUCCESS) {

        ::RegSetValueEx(hKey,
                        L"Description",
                        0,
                        REG_SZ, 
                        (CONST BYTE*) m_wsServiceDesc,
                        (wcslen(m_wsServiceDesc) * sizeof(WCHAR)) + sizeof(WCHAR));     

        ::RegCloseKey(hKey);
    }

    // tidy up
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL ServiceController::Uninstall()
{
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    BOOL bResult = FALSE;
    SC_HANDLE hService = ::OpenService(hSCM,
                                       m_wsServiceName,
                                       DELETE);
    if (hService) {
        if (::DeleteService(hService)) {
            EVLOG_INFO_STR(EVMSG_REMOVED, m_wsServiceName);
            bResult = TRUE;
        } else {
			EVLOG_ERROR_STR(EVMSG_NOTREMOVED, m_wsServiceName);
        }
        ::CloseServiceHandle(hService);
    }
    
    ::CloseServiceHandle(hSCM);
    return bResult;
}