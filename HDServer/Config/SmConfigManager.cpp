#include "pch.h"
#include "afxwin.h"
#include "SmConfigManager.h"


SmConfigManager::SmConfigManager()
{
}


SmConfigManager::~SmConfigManager()
{
}

std::string SmConfigManager::GetApplicationPath()
{
	TCHAR path[1024];
	std::string fullPath;
	int n = GetModuleFileName(0, path, sizeof(path) / sizeof(path[0]));
	if (n > 0) {
		fullPath = path;

		size_t pos = fullPath.rfind('\\');
		if (pos != std::string::npos) {
			fullPath = fullPath.substr(0, pos);
		}
	}
	else throw _T("Cannot get application file name.");

	return fullPath;
}
