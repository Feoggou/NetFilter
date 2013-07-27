#pragma once

#include <Windows.h>

class ServiceController
{
public:
	ServiceController(LPCWSTR wsServiceName, LPCWSTR wsServiceDesc)
		: m_wsServiceName(wsServiceName), m_wsServiceDesc(wsServiceDesc)
	{}

	BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();

private:
	LPCWSTR m_wsServiceName;
	LPCWSTR m_wsServiceDesc;
};