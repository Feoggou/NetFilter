#pragma once

#include "General.h"

class Window
{
public:
	enum MessageSource {MessageSource_Menu = 0, MessageSource_Accelerator = 1};

protected:
	HWND		m_hWnd;
	HWND		m_hParent;

public:
	Window(HWND hParent) : m_hParent(hParent), m_hWnd(nullptr) {}
	virtual ~Window() = 0 {/*TODO: what do we do with hWnd?*/}

	HWND GetHandle() {return m_hWnd;}

protected:
	virtual void OnSysCommand(WPARAM cmd, int x, int y) {}
	virtual void OnClose() = 0;
	virtual void OnDestroy() = 0;
	virtual void OnCommand(WORD source, WORD id, HWND hControl) {}
	virtual void OnNotify(NMHDR*) {}
	virtual void OnSize(WPARAM type, int new_width, int new_height) {}
};