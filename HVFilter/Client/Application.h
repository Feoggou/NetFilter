#pragma once

#include "General.h"
#include <memory>

class MainDialog;

class Application
{
public:
	Application(HINSTANCE hInst);
	~Application(void);

	void ShowMainDialog();

	HWND GetMainWindow();

public:
	static HINSTANCE GetHInstance() {_ASSERT(s_pInst); return s_hInst;}
	static Application* GetInst() {_ASSERT(s_pInst); return s_pInst;}

private:
	static HINSTANCE	s_hInst;
	static Application*	s_pInst;

	std::unique_ptr<MainDialog>	m_pMainDlg;
};

#define theApp Application::GetInst()