#include "MainDialog.h"
#include "resource.h"

#include "Application.h"

#include <algorithm>

MainDialog::MainDialog() 
	: Dialog(nullptr, IDD_MAINDLG),
	m_hInboundPackageCountEdit(NULL),
	m_hInboundPackageSizeEdit(NULL),
	m_hOutboundPackageCountEdit(NULL),
	m_hOutboundPackageSizeEdit(NULL)
{}

void MainDialog::OnCommand(WORD source, WORD id, HWND /*hControl*/) 
{
	if (id == IDCLOSE) {
		m_dataDeviceThread.Stop();

		OnClose();
		return;
	}

	//TODO: else?
}

void MainDialog::OnInitDialog()
{
	m_hInboundPackageCountEdit = GetDlgItem(m_hWnd, IDC_PACKETS_COUNT_INBOUND);
	m_hInboundPackageSizeEdit = GetDlgItem(m_hWnd, IDC_PACKETS_SIZE_INBOUND);

	m_hOutboundPackageCountEdit = GetDlgItem(m_hWnd, IDC_PACKETS_COUNT_OUTBOUND);
	m_hOutboundPackageSizeEdit = GetDlgItem(m_hWnd, IDC_PACKETS_SIZE_OUTBOUND);

	m_dataDeviceThread.SetControls(m_hInboundPackageCountEdit, m_hInboundPackageSizeEdit,
		m_hOutboundPackageCountEdit, m_hOutboundPackageSizeEdit);

	m_dataDeviceThread.Start();
}