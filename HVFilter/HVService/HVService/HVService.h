#pragma once

#include "Service.h"
#include "HVioctl.h"

class HVService : public Service
{
public:
	HVService(void);
	~HVService(void);

	BOOL OnInit() /*override*/;
    void Run() /*override*/;
    BOOL OnUserControl(DWORD dwOpcode) /*override*/;
    void OnDeviceEvent(DWORD dwEventType,LPVOID lpEventData) /*override*/;

    BOOL GetRequest(POSR_COMM_CONTROL_REQUEST PRequest,LPOVERLAPPED POverlapped);
    BOOL SendResponse(POSR_COMM_CONTROL_RESPONSE PResponse,LPOVERLAPPED POverlapped);
    BOOL GetRequestSendResponse(POSR_COMM_CONTROL_REQUEST PRequest,
                                POSR_COMM_CONTROL_RESPONSE PResponse,
                                LPOVERLAPPED POverlapped);

private:
    void SaveStatus();

    // Control parameters
    int	m_iStartParam;
    int	m_iIncParam;

    // Current state
    int	m_iState;

    BOOL	m_bConnected;
    HANDLE	m_hOsrControl;
};

