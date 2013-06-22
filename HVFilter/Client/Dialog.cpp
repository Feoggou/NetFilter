#include "Dialog.h"
#include "Application.h"
#include "Debug.h"

#include "General.h"

#include <set>
#include <algorithm>

INT_PTR Dialog::CreateModal()
{
	INT_PTR nResult = DialogBoxParamW(Application::GetHInstance(), MAKEINTRESOURCE(m_resourceID), m_hParent, DialogProc, (LPARAM)this);
	if (nResult == -1)
		DisplayError();

	return nResult;
}

INT_PTR CALLBACK Dialog::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static std::set<Dialog*> dialogs;
	Dialog* pThis = nullptr;

	if (uMsg == WM_INITDIALOG) {
		//save pThis for further use and save hDlg for use in the class functions
		pThis = (Dialog*)lParam;
		pThis->SetHandle(hDlg);
		dialogs.insert(pThis);
	} else {
		std::set<Dialog*>::iterator i = std::find_if(std::begin(dialogs), std::end(dialogs), [hDlg](Dialog* p) {return p->GetHandle() == hDlg;});

		if (i != std::end(dialogs)) {
			pThis = *i;
		} else {
			return 0;
		}
	}

	switch (uMsg)
	{
	case WM_INITDIALOG: pThis->OnInitDialog(); break;
	case WM_CLOSE: pThis->OnClose(); break;
	case WM_DESTROY: pThis->OnDestroy(); break;
	case WM_COMMAND: pThis->OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam); break;
	case WM_NOTIFY:	{ NMHDR* pNMHDR = (NMHDR*)lParam; pThis->OnNotify(pNMHDR);}	break;
	case WM_SYSCOMMAND: pThis->OnSysCommand(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;

	case WM_SETTEXT:
		{
			char* a = (char*)lParam;
			int x = 0;
			++x;
		}
	}

	return pThis->OnDialogProcedure(uMsg, wParam, lParam);
}

void Dialog::OnClose()
{
	_ASSERT(m_hWnd);
	EndDialog(m_hWnd, IDCANCEL);
}

void Dialog::OnDestroy()
{
	_ASSERT(m_hWnd);
	EndDialog(m_hWnd, IDCANCEL);
}

void Dialog::OnCommand(WORD /*code*/, WORD id, HWND hControl)
{
	if (hControl) {
		if (id == IDOK) {
			OnOK();
		} else if (id == IDCANCEL) {
			OnCancel();
		}
	}
}