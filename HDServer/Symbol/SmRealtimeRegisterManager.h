#pragma once
#include <map>
#include <string>
#include <set>
#include "../Global/TemplateSingleton.h"
class SmUser;
class SmRealtimeRegisterManager : public TemplateSingleton<SmRealtimeRegisterManager>
{
public:
	SmRealtimeRegisterManager();
	~SmRealtimeRegisterManager();
	void RegisterProduct(std::string symCode);
	void UnregisterAllProducts();
private:
	std::set<std::string> _RegisteredProduct;
};

