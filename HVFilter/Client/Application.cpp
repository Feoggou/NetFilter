#include "Application.h"
#include "MainDialog.h"

Application* Application::s_pInst = nullptr;
HINSTANCE Application::s_hInst = NULL;

Application::Application(HINSTANCE hInst)
{
	s_hInst = hInst;

	_ASSERT(!s_pInst);

	s_pInst = this;
}


Application::~Application(void)
{
}

void Application::ShowMainDialog()
{
	_ASSERT(!m_pMainDlg.get());

	m_pMainDlg.reset(new MainDialog);
	m_pMainDlg->CreateModal();
}

HWND Application::GetMainWindow()
{
	return m_pMainDlg->GetHandle();
}