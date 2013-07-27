#pragma once

#include <Windows.h>
#include <crtdbg.h>

#include "DebugMsg.h"

#define HANDLEREX

class Service
{
public:
	Service(const WCHAR* szServiceName,const WCHAR* szServiceDescription);
	virtual ~Service(void);

    void SetStatus(DWORD dwState);
    BOOL Initialize();
    virtual void Run();
	virtual BOOL OnInit();
    virtual void OnStop();
    virtual void OnInterrogate();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
    virtual void OnDeviceEvent(DWORD dwEventType,LPVOID lpEventData);
    virtual BOOL OnUserControl(DWORD dwOpcode);
    
    // static member functions
    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
#ifndef HANDLEREX
    static void WINAPI Handler(DWORD dwOpcode);
#else   
    static DWORD WINAPI HandlerEx(DWORD dwOpcode,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext);
#endif  

	LPCWSTR	GetName() const {return m_szServiceName;} 
	LPCWSTR	GetDescription() const {return m_szServiceDescription;} 
	int GetMajorVersion() const {return m_iMajorVersion;}
	int GetMinorVersion() const {return m_iMinorVersion;}
	int GetExitCode() const {return m_Status.dwWin32ExitCode;}

private:
	void _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	DWORD _HandlerEx(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

public:
	static Service* GetService() {_ASSERT(s_pService); return s_pService;}

private:
	static Service*		s_pService;

protected:
	DebugMessage	m_dbgMsg;

	// data members
    WCHAR m_szServiceName[64];
    WCHAR m_szServiceDescription[256];
    int m_iMajorVersion;
    int m_iMinorVersion;
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_Status;
    BOOL m_bIsRunning;
    BOOL m_bDebugging;

};

