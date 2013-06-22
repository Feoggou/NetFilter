#pragma once

#include "Window.h"

class Dialog: public Window
{
public:
	Dialog(HWND hParent, int resource_id) : Window(hParent), m_resourceID(resource_id) {}
	INT_PTR CreateModal();

	void SetHandle(HWND hWnd) {m_hWnd = hWnd;}

private:
	static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnInitDialog() {}
	virtual INT_PTR OnDialogProcedure(UINT uMsg, WPARAM wParam, LPARAM lParam) {return 0;}
	void OnClose() override;
	void OnDestroy() override;
	void OnCommand(WORD source, WORD id, HWND hControl) override;
	void OnNotify(NMHDR*) override{}
	virtual void OnOK() {}
	virtual void OnCancel() {}

private:
	Dialog(const Dialog&);
	Dialog& operator=(const Dialog&);

private:
	const int m_resourceID;
};