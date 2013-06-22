#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>
#include <Commdlg.h>

#define LODWORD(l)           ((DWORD)(((ULONGLONG)(l)) & 0xffffffff))
#define HIDWORD(l)           ((DWORD)((((ULONGLONG)(l)) >> 32) & 0xffffffff))

//warning: more secure versions
#pragma warning (disable: 4996)