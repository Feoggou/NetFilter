#include "MainDialog.h"
#include "resource.h"

#include "Application.h"

#include <algorithm>

MainDialog::MainDialog() 
	: Dialog(nullptr, IDD_MAINDLG),
	m_hPackageCountEdit(NULL),
	m_hPackageSizeEdit(NULL)
{}

void MainDialog::OnCommand(WORD source, WORD id, HWND /*hControl*/) 
{
	if (id == IDCLOSE) {
		OnClose();
		return;
	}

	//TODO: else?
}

void MainDialog::OnInitDialog()
{
	m_hPackageCountEdit = GetDlgItem(m_hWnd, IDC_PACKETS_COUNT);
	m_hPackageSizeEdit = GetDlgItem(m_hWnd, IDC_PACKETS_SIZE);

	m_pipeThread.SetControls(m_hPackageCountEdit, m_hPackageSizeEdit);
	m_pipeThread.Start();
}