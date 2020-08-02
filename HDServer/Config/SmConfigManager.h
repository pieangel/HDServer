#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
class SmConfigManager : public TemplateSingleton<SmConfigManager>
{
public:
	SmConfigManager();
	~SmConfigManager();

	std::string GetApplicationPath();
};

