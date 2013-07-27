#include <Windows.h>
#include <cwchar>

#include "HVService.h"
#include "ServiceController.h"
#include "EventLogger.h"
#include "DebugMsg.h"

////////////////////////////////////////////////////////////////////////////////////////
// Default command line argument parsing

// Returns TRUE if it found an arg it recognised, FALSE if not
// Note: processing some arguments causes output to stdout to be generated.
BOOL ParseStandardArgs(int argc, WCHAR* argv[], Service* pSvc)
{
	ServiceController sc(pSvc->GetName(), pSvc->GetDescription());
    // See if we have any command line args we recognise
    if (argc <= 1) return FALSE;

    if (_wcsicmp(argv[1], L"-v") == 0) {

        // Spit out version info
        wprintf(L"%ls Version %d.%d\n", pSvc->GetName(), pSvc->GetMajorVersion(), pSvc->GetMinorVersion());
        wprintf(L"The service is %s installed\n", sc.IsInstalled() ? L"currently" : L"not");
        return TRUE; // say we processed the argument

    } else if (_wcsicmp(argv[1], L"-i") == 0) {

        // Request to install.
        if (sc.IsInstalled()) {
            wprintf(L"%ls is already installed\n", pSvc->GetName());
        } else {
            // Try and install the copy that's running
            if (sc.Install()) {
                wprintf(L"%ls installed\n", pSvc->GetName());
            } else {
                wprintf(L"l%s failed to install. Error %d\n", pSvc->GetName(), GetLastError());
            }
        }
        return TRUE; // say we processed the argument

    } else if (_wcsicmp(argv[1], L"-u") == 0) {

        // Request to uninstall.
        if (!sc.IsInstalled()) {
            wprintf(L"%ls is not installed\n", pSvc->GetName());
        } else {
            // Try and remove the copy that's installed
            if (sc.Uninstall()) {
                // Get the executable file path
                WCHAR szFilePath[_MAX_PATH];
                ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
                wprintf(L"%ls removed. (You must delete the file (%s) yourself.)\n", pSvc->GetName(), szFilePath);
            } else {
                wprintf(L"Could not remove %ls. Error %d\n", pSvc->GetName(), GetLastError());
            }
        }
        return TRUE; // say we processed the argument
    
    } 
    
    // Don't recognise the args
    return FALSE;
}

BOOL StartHVService(LPWSTR wsServiceName)
{
    SERVICE_TABLE_ENTRY st[] = {
        {wsServiceName, Service::ServiceMain},
        {NULL, NULL}
    };

	DebugMessage dbgMsg(wsServiceName);

    dbgMsg(L"Calling StartServiceCtrlDispatcher()");
    BOOL b = ::StartServiceCtrlDispatcher(st);
    if(!b) {
        dbgMsg(L"Error starting 0x%x.",GetLastError());
    }
    dbgMsg(L"Returned from StartServiceCtrlDispatcher()");
    return b;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HVService service;
	int argc = -1;

	WCHAR* wsCmdLine = GetCommandLineW();
	WCHAR** argv = CommandLineToArgvW(wsCmdLine, &argc);

	EventLogger evLog(service.GetName());

	// Parse for standard arguments (install, uninstall, version etc.)
	if (!ParseStandardArgs(argc, argv, &service)) {

		// Didn't find any standard args so start the service
		// Uncomment the DebugBreak line below to enter the debugger
		// when the service is started.
		
		//DebugBreak();
		StartHVService(const_cast<LPWSTR>(service.GetName()));
	}

	LocalFree(argv);

	// When we get here, the service has been stopped
	int nRetCode = service.GetExitCode();

	return nRetCode;
}