#include "SmRealtimeRegisterManager.h"
#include "../User/SmUser.h"
#include "../HDCtrl/HdClient.h"
SmRealtimeRegisterManager::SmRealtimeRegisterManager()
{

}

SmRealtimeRegisterManager::~SmRealtimeRegisterManager()
{
	
}

void SmRealtimeRegisterManager::RegisterProduct(std::string symCode)
{
	HdClient* client = HdClient::GetInstance();
	client->RegisterProduct(symCode.c_str());
	_RegisteredProduct.insert(symCode);
}

void SmRealtimeRegisterManager::UnregisterAllProducts()
{
	HdClient* client = HdClient::GetInstance();
	for (auto it = _RegisteredProduct.begin(); it != _RegisteredProduct.end(); ++it) {
		client->UnregisterProduct((*it).c_str());
	}
}
