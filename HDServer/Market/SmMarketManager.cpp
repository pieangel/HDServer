#include "pch.h"
#include "SmMarketManager.h"
#include "../Config/SmConfigManager.h"
#include <string>
#include "../Xml/tinyxml2.h"
#include "../Xml/pugixml.hpp"
#include "../Symbol/SmSymbolReader.h"
#include "SmMarket.h"
#include "SmCategory.h"
#include "../Util/VtStringUtil.h"
#include "../Json/json.hpp"
#include "../Log/loguru.hpp"
#include "../Util/SmUtil.h"
#include <chrono>
#include "../Util/SmUtfUtil.h"
#include "../Service/SmServiceDefine.h"
#include "../User/SmUserManager.h"
#include "../User/SmUser.h"
#include "../Symbol/SmSymbol.h"
#include "SmCategory.h"
#include "../Global/SmGlobal.h"
#include "../Server/SmSessionManager.h"
#include "../Market/SmProductYearMonth.h"


using namespace std::chrono;
using namespace nlohmann;


SmMarketManager::SmMarketManager()
{
}


SmMarketManager::~SmMarketManager()
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		delete* it;
	}
}

void SmMarketManager::ReadSymbolsFromFile()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append(_T("\\Config\\Config.xml"));
	std::string dataPath = appPath;
	dataPath.append(_T("\\mst\\"));
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node sym_file_list = app.first_child();
	pugi::xml_node abroad_list = sym_file_list.first_child();
	int index = 0;
	for (auto it = abroad_list.begin(); it != abroad_list.end(); ++it) {
		std::string file_name = it->text().as_string();
		TRACE(file_name.c_str());
		std::string file_path = dataPath + file_name;
		symReader->ReadSymbolFromFile(index++, file_path);
	}
}

SmMarket* SmMarketManager::AddMarket(std::string name)
{
	SmMarket* found_market = FindMarket(name);
	if (found_market)
		return found_market;

	SmMarket* market = new SmMarket();
	market->Name(name);
	_MarketList.emplace_back(market);
	return market;
}

SmCategory* SmMarketManager::FindCategory(std::string mrkt_name, std::string cat_code)
{
	SmMarket* cur_market = FindMarket(mrkt_name);
	if (!cur_market)
		return nullptr;
	return cur_market->FindCategory(cat_code);
}

SmCategory* SmMarketManager::FindCategory(std::string cat_code)
{
	auto it = _CategoryToMarketMap.find(cat_code);
	if (it != _CategoryToMarketMap.end()) {
		std::string market_name = it->second;
		SmMarket* mrkt = FindMarket(market_name);
		if (mrkt)
			return mrkt->FindCategory(cat_code);
		else
			return nullptr;
	}

	return nullptr;
}

/*
std::vector<std::shared_ptr<SmSymbol>> SmMarketManager::GetRecentMonthSymbolList()
{
	std::vector<std::shared_ptr<SmSymbol>> symvec;
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		auto cat_vec = mrkt->GetCategoryList();
		for (auto itc = cat_vec.begin(); itc != cat_vec.end(); ++itc) {
			std::shared_ptr<SmSymbol> sym = (*itc)->GetRecentMonthSymbol();
			if (sym)
				symvec.push_back(sym);
		}
	}

	return symvec;
}
*/


std::vector<std::shared_ptr<SmSymbol>> SmMarketManager::GetRecentMonthSymbolList()
{
	std::vector<std::shared_ptr<SmSymbol>> symvec;
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		auto cat_vec = mrkt->GetCategoryList();
		for (auto itc = cat_vec.begin(); itc != cat_vec.end(); ++itc) {
			
			std::shared_ptr<SmProductYearMonth> ym = (*itc)->GetRecentYearMonth();
			if (ym) {
				if (ym->ProductCode.compare("175") == 0) {
					std::shared_ptr<SmSymbol> recent_symbol = GetRecentSymbol(ym->ProductCode);
					std::string current_date = VtStringUtil::getCurentDate();
					if (current_date.compare(recent_symbol->LastDate()) >= 0) {
						ym = (*itc)->GetNextYearMonth();
					}
				}
				for (auto itym = ym->SymbolList.begin(); itym != ym->SymbolList.end(); ++itym) {
					(*itym)->Quote.SymbolCode = (*itym)->SymbolCode();
					symvec.push_back(*itym);
				}
			}
		}
	}

	return symvec;
}

void SmMarketManager::SendMarketList(std::string user_id)
{
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		json send_object;
		send_object["res_id"] = SmProtocol::res_market_list;
		send_object["total_market_count"] = (int)_MarketList.size();
		send_object["total_category_count"] = GetTotalCategoryCount();
		send_object["market_index"] = (int)i;
		send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str());
		std::vector<SmCategory*>&  catVec = market->GetCategoryList();
		for (size_t j = 0; j < catVec.size(); ++j) {
			SmCategory* cat = catVec[j];
			send_object["category"][j] = {
				{ "code",  cat->Code() },
				{ "name_kr", SmUtfUtil::AnsiToUtf8((char*)cat->NameKr().c_str()) },
				{ "name_en",  cat->Name() },
				{ "exchange_name",  cat->Exchange() },
				{ "exchange_code",  cat->ExchangeCode() },
				{ "market_name",  SmUtfUtil::AnsiToUtf8((char*)cat->MarketName().c_str()) }
			};
		}

		
		std::string content = send_object.dump();
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->SendResultMessage(user_id, content);
	}
	
}

void SmMarketManager::SendMarketList(int session_id)
{
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		json send_object;
		send_object["res_id"] = SmProtocol::res_market_list;
		send_object["total_market_count"] = (int)_MarketList.size();
		send_object["total_category_count"] = GetTotalCategoryCount();
		send_object["market_index"] = (int)i;
		send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str());
		std::vector<SmCategory*>& catVec = market->GetCategoryList();
		for (size_t j = 0; j < catVec.size(); ++j) {
			SmCategory* cat = catVec[j];
			send_object["category"][j] = {
				{ "code",  cat->Code() },
				{ "name_kr", SmUtfUtil::AnsiToUtf8((char*)cat->NameKr().c_str()) },
				{ "name_en",  cat->Name() },
				{ "exchange_name",  cat->Exchange() },
				{ "exchange_code",  cat->ExchangeCode() },
				{ "market_name",  SmUtfUtil::AnsiToUtf8((char*)cat->MarketName().c_str()) }
			};
		}


		std::string content = send_object.dump();
		SmGlobal* global = SmGlobal::GetInstance();
		std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
		sessMgr->send(session_id, content);
	}
}

void SmMarketManager::SendSymbolListByCategory(std::string user_id)
{
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		std::vector<SmCategory*>& cat_list = market->GetCategoryList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			SmCategory* cat = cat_list[j];
			std::vector<std::shared_ptr<SmSymbol>>& sym_list = cat->GetSymbolList();
			for (size_t k = 0; k < sym_list.size(); ++k) {
				SendSymbolMaster(user_id, sym_list[k]);
			}
		}
	}
}

void SmMarketManager::SendSymbolListByCategory(int session_id)
{
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		std::vector<SmCategory*>& cat_list = market->GetCategoryList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			SmCategory* cat = cat_list[j];
			std::vector<std::shared_ptr<SmSymbol>>& sym_list = cat->GetSymbolList();
			for (size_t k = 0; k < sym_list.size(); ++k) {
				SendSymbolMaster(session_id, sym_list[k]);
			}
		}
	}
}

int SmMarketManager::GetTotalCategoryCount()
{
	int total = 0;
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		total += market->GetCategoryList().size();
	}

	return total;
}

int SmMarketManager::GetTotalSymbolCount()
{
	int total = 0;
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		std::vector<SmCategory*>& cat_list = market->GetCategoryList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			total += cat_list[j]->GetSymbolList().size();
		}
	}

	return total;
}

std::shared_ptr<SmSymbol> SmMarketManager::GetRecentSymbol(std::string product_name)
{
	auto it = _CategoryToMarketMap.find(product_name);
	if (it == _CategoryToMarketMap.end())
		return nullptr;
	std::string market_name = it->second;

	return GetRecentSymbol(market_name, product_name);
}

std::shared_ptr<SmSymbol> SmMarketManager::GetRecentSymbol(std::string market_name, std::string product_name)
{
	SmMarket* market = FindMarket(market_name);
	if (!market)
		return nullptr;
	SmCategory* product = market->FindCategory(product_name);
	if (!product)
		return nullptr;
	std::shared_ptr<SmProductYearMonth> ym = product->GetRecentYearMonth();
	if (ym->SymbolList.size() == 0)
		return nullptr;
	return *ym->SymbolList.begin();
}

void SmMarketManager::SendSymbolMaster(std::string user_id, std::shared_ptr<SmSymbol> sym)
{
	if (!sym)
		return;
	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_master;
	send_object["total_symbol_count"] = GetTotalSymbolCount();
	send_object["symbol_code"] = sym->SymbolCode();
	send_object["category_index"] = sym->Index();
	send_object["name_kr"] = SmUtfUtil::AnsiToUtf8((char*)sym->Name().c_str());
	send_object["name_en"] = sym->NameEn().c_str();
	send_object["category_code"] = sym->CategoryCode();
	send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)sym->MarketName().c_str());
	send_object["decimal"] = sym->Decimal();
	send_object["contract_unit"] = sym->CtrUnit();
	send_object["seungsu"] = sym->Seungsu();
	send_object["tick_size"] = sym->TickSize();
	send_object["tick_value"] = sym->TickValue();

	std::string content = send_object.dump(4);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(user_id, content);
}

void SmMarketManager::SendSymbolMaster(int session_id, std::shared_ptr<SmSymbol> sym)
{
	if (!sym)
		return;
	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_master;
	send_object["total_symbol_count"] = GetTotalSymbolCount();
	send_object["symbol_code"] = sym->SymbolCode();
	send_object["category_index"] = sym->Index();
	send_object["name_kr"] = SmUtfUtil::AnsiToUtf8((char*)sym->Name().c_str());
	send_object["name_en"] = sym->NameEn().c_str();
	send_object["category_code"] = sym->CategoryCode();
	send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)sym->MarketName().c_str());
	send_object["decimal"] = sym->Decimal();
	send_object["contract_unit"] = sym->CtrUnit();
	send_object["seungsu"] = sym->Seungsu();
	send_object["tick_size"] = sym->TickSize();
	send_object["tick_value"] = sym->TickValue();
	send_object["atm"] = sym->Atm();
	send_object["near_month"] = sym->NearMonth();
	send_object["last_date"] = sym->LastDate();

	std::string content = send_object.dump(4);
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(session_id, content);
}

SmMarket* SmMarketManager::FindMarket(std::string mrkt_name)
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		if (mrkt->Name().find(mrkt_name) != std::string::npos) {
			return mrkt;
		}
	}

	return nullptr;
}

void SmMarketManager::AddCategoryMarket(std::string cat_code, std::string mrkt_name)
{
	_CategoryToMarketMap[cat_code] = mrkt_name;
}
