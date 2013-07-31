#pragma once
#include "General.h"
#include "resource.h"

#include "Dialog.h"
#include "DataDeviceThread.h"

#include <string>

class MainDialog : public Dialog
{
public:
	MainDialog();

	~MainDialog(void) {}

	//PRIVATE FUNCTIONS
private:
	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//MESSAGE HANDLERS
	void OnInitDialog();
	//called when the user presses the "Connect" button
	void OnButtonConnect();
	//called when the user presses the "Create Connection" button
	void CreateConnection();
	//called when the user presses the "Browse" button
	void OnBrowse();

public:
	HWND GetInboundPackageCountEdit() {return m_hInboundPackageCountEdit;}
	HWND GetInboundPackageSizeEdit() {return m_hInboundPackageSizeEdit;}

	HWND GetOutboundPackageCountEdit() {return m_hOutboundPackageCountEdit;}
	HWND GetOutboundPackageSizeEdit() {return m_hOutboundPackageSizeEdit;}

private:
	void OnCommand(WORD source, WORD id, HWND hControl) override;

private:
	HWND	m_hInboundPackageCountEdit;
	HWND	m_hInboundPackageSizeEdit;

	HWND	m_hOutboundPackageCountEdit;
	HWND	m_hOutboundPackageSizeEdit;

	//TODO: it's not the best place for a thread obj here.
	DataDeviceThread			m_dataDeviceThread;
};