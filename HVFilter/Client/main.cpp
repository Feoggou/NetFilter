#include <math.h>
#include <exception>

#include "General.h"
#include "Application.h"

#include "MainDialog.h"
#include "resource.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try {
		Application app(hInstance);

		app.ShowMainDialog();

	} catch (std::exception&) {
		//TODO
	}

	return 0;
}