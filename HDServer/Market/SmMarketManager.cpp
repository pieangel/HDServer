#include "pch.h"
#include "SmMarketManager.h"
#include "../Config/SmConfigManager.h"
#include <string>
#include "../Xml/tinyxml2.h"
#include "../Xml/pugixml.hpp"
#include "../Symbol/SmSymbolReader.h"
#include "SmMarket.h"
#include "SmProduct.h"
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
#include "SmProduct.h"
#include "../Global/SmGlobal.h"
#include "../Server/SmSessionManager.h"
#include "../Market/SmProductYearMonth.h"
#include "../Symbol/SmRunInfo.h"


using namespace std::chrono;
using namespace nlohmann;


SmMarketManager::SmMarketManager()
{
	InitDmMarketProducts();
	InitDmFutures();
	InitAbroadProducts();
}


SmMarketManager::~SmMarketManager()
{
// 	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
// 		delete* it;
// 	}
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

std::shared_ptr<SmMarket> SmMarketManager::AddMarket(std::string name)
{
	std::shared_ptr<SmMarket> found_market = FindMarket(name);
	if (found_market)
		return found_market;

	std::shared_ptr<SmMarket> market = std::make_shared<SmMarket>();
	market->Name(name);
	_MarketList.emplace_back(market);
	return market;
}

std::shared_ptr<SmProduct> SmMarketManager::FindProduct(std::string mrkt_name, std::string cat_code)
{
	std::shared_ptr<SmMarket> cur_market = FindMarket(mrkt_name);
	if (!cur_market)
		return nullptr;
	return cur_market->FindProduct(cat_code);
}

std::shared_ptr<SmProduct> SmMarketManager::FindProduct(std::string cat_code)
{
	auto it = _CategoryToMarketMap.find(cat_code);
	if (it != _CategoryToMarketMap.end()) {
		std::string market_name = it->second;
		std::shared_ptr<SmMarket> mrkt = FindMarket(market_name);
		if (mrkt)
			return mrkt->FindProduct(cat_code);
		else
			return nullptr;
	}

	return nullptr;
}


std::vector<std::shared_ptr<SmSymbol>> SmMarketManager::GetRecentMonthSymbolList()
{
	std::vector<std::shared_ptr<SmSymbol>> symvec;
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		std::shared_ptr<SmMarket> mrkt = *it;
		auto cat_vec = mrkt->GetCategoryList();
		for (auto itc = cat_vec.begin(); itc != cat_vec.end(); ++itc) {
			
			std::shared_ptr<SmProductYearMonth> ym = (*itc)->GetRecentYearMonth();
			if (ym) {
				for (auto itym = ym->SymbolList.begin(); itym != ym->SymbolList.end(); ++itym) {
					(*itym)->Quote.SymbolCode = (*itym)->SymbolCode();
					symvec.push_back(*itym);
				}
			}
		}
	}

	return symvec;
}

std::vector<std::shared_ptr<SmSymbol>> SmMarketManager::GetRecentNextMonthSymbolList()
{
	std::vector<std::shared_ptr<SmSymbol>> symvec;
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		std::shared_ptr<SmMarket> mrkt = *it;
		auto cat_vec = mrkt->GetCategoryList();
		for (auto itc = cat_vec.begin(); itc != cat_vec.end(); ++itc) {

			std::shared_ptr<SmProductYearMonth> ym = (*itc)->GetRecentYearMonth();
			if (ym) {
				for (auto itym = ym->SymbolList.begin(); itym != ym->SymbolList.end(); ++itym) {
					(*itym)->Quote.SymbolCode = (*itym)->SymbolCode();
					symvec.push_back(*itym);
				}
			}

			ym = (*itc)->GetNextYearMonth();
			if (ym) {
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
		std::shared_ptr<SmMarket> market = _MarketList[i];
		json send_object;
		send_object["res_id"] = SmProtocol::res_market_list;
		send_object["total_market_count"] = (int)_MarketList.size();
		send_object["total_category_count"] = GetTotalCategoryCount();
		send_object["market_index"] = (int)i;
		send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str());
		std::vector<std::shared_ptr<SmProduct>>&  catVec = market->GetCategoryList();
		for (size_t j = 0; j < catVec.size(); ++j) {
			std::shared_ptr<SmProduct> cat = catVec[j];
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
		std::shared_ptr<SmMarket> market = _MarketList[i];
		json send_object;
		send_object["res_id"] = SmProtocol::res_market_list;
		send_object["total_market_count"] = (int)_MarketList.size();
		send_object["total_category_count"] = GetTotalCategoryCount();
		send_object["market_index"] = (int)i;
		send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str());
		std::vector<std::shared_ptr<SmProduct>>& catVec = market->GetCategoryList();
		for (size_t j = 0; j < catVec.size(); ++j) {
			std::shared_ptr<SmProduct> cat = catVec[j];
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
		std::shared_ptr<SmMarket> market = _MarketList[i];
		std::vector<std::shared_ptr<SmProduct>>& cat_list = market->GetCategoryList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			std::shared_ptr<SmProduct> cat = cat_list[j];
			std::vector<std::shared_ptr<SmSymbol>>& sym_list = cat->GetSymbolList();
			for (size_t k = 0; k < sym_list.size(); ++k) {
				SendSymbolMaster(user_id, sym_list[k]);
			}
		}
	}
}

void SmMarketManager::SendSymbolListByCategory(int session_id)
{
	try {
		int s = 0;
		for (size_t i = 0; i < _MarketList.size(); ++i) {
			std::shared_ptr<SmMarket> market = _MarketList[i];
			std::vector<std::shared_ptr<SmProduct>>& cat_list = market->GetCategoryList();
			for (size_t j = 0; j < cat_list.size(); ++j) {
				std::shared_ptr<SmProduct> cat = cat_list[j];
				std::vector<std::shared_ptr<SmSymbol>>& sym_list = cat->GetSymbolList();
				for (size_t k = 0; k < sym_list.size(); ++k) {
					SendSymbolMaster(session_id, sym_list[k]);
					s++;
				}
			}
		}


		s = s + 1;
	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

int SmMarketManager::GetTotalCategoryCount()
{
	int total = 0;
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		std::shared_ptr<SmMarket> market = _MarketList[i];
		total += market->GetCategoryList().size();
	}

	return total;
}

int SmMarketManager::GetTotalSymbolCount()
{
	int total = 0;
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		std::shared_ptr<SmMarket> market = _MarketList[i];
		std::vector<std::shared_ptr<SmProduct>>& cat_list = market->GetCategoryList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			total += cat_list[j]->GetSymbolList().size();
		}
	}

	return total;
}


void SmMarketManager::InitDmMarketProducts()
{
	std::string market_name = "국내선물";
	std::shared_ptr<SmMarket> market = AddMarket(market_name);
	market->MarketCode("101F");
	std::string product_code = "101";
	std::shared_ptr<SmProduct> product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("국내선물");
	product->Name("Kospi200MF");

	product_code = "105";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200미니선물");
	product->Name("Kospi200MiniF");

	product_code = "106";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스닥150선물");
	product->Name("Kosdaqq150F");

	product_code = "167";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("BMA 선물");
	product->Name("BMA Future");

	product_code = "175";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("USD 선물");
	product->Name("USD Future");

	/*
	std::string market_name = "코스피선물";
	SmMarket* market = AddMarket(market_name);
	market->MarketCode("101F");
	std::string product_code = "101";
	SmProduct* product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200선물");
	product->Name("Kospi200F");

	market_name = "미니코스피선물";
	market = AddMarket(market_name);
	market->MarketCode("105F");

	product_code = "105";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200미니선물");
	product->NameKr("Kospi200MiniF");


	market_name = "코스닥선물";
	market = AddMarket(market_name);
	market->MarketCode("106F");

	product_code = "106";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스닥150선물");
	product->NameKr("Kosdaqq150F");

	market_name = "국채선물";
	market = AddMarket(market_name);
	market->MarketCode("167F");

	product_code = "167";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("BMA 선물");
	product->Name("BMA Future");


	market_name = "달러선물";
	market = AddMarket(market_name);
	market->MarketCode("175F");
	product_code = "175";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("USD 선물");
	product->Name("USD Future");


	market_name = "코스피옵션";
	market = AddMarket(market_name);
	market->MarketCode("101O");
	product_code = "201";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200콜옵션");
	product->Name("Kospi200O");

	product_code = "301";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200풋옵션");
	product->Name("Kospi200O");

	market_name = "코스피위클리옵션";
	market = AddMarket(market_name);
	market->MarketCode("109O");

	product_code = "209";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200위클리콜옵션");
	product->Name("Kospi200WeeklyO");

	product_code = "309";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("코스피200위클리풋옵션");
	product->Name("Kospi200WeeklyO");

	market_name = "코스피미니옵션";
	market = AddMarket(market_name);
	market->MarketCode("105O");

	product_code = "205";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("미니코스피200콜옵션");
	product->NameKr("MiniKospi200O");

	product_code = "305";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->NameKr("미니코스피200풋옵션");
	product->NameKr("MiniKospi200O");

	market_name = "코스닥옵션";
	market = AddMarket(market_name);
	market->MarketCode("106O");

	product_code = "206";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->Name("코스닥150콜옵션");
	product->NameKr("Kosqaq150O");

	product_code = "306";
	product = market->FindAddProduct(product_code);
	AddProduct(product);
	AddCategoryMarket(product_code, market_name);
	product->MarketName(market_name);
	product->Name("코스닥150풋옵션");
	product->NameKr("Kosqaq150O");
	*/
}

void SmMarketManager::AddFutureRunInfo(SmRunInfo run_info)
{
	_FutureRunVector.push_back(run_info);
}

void SmMarketManager::AddOptionRunInfo(SmRunInfo run_info)
{
	_OptionRunVector.push_back(run_info);
}


void SmMarketManager::LoadRunInfo()
{
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetApplicationPath();
	appPath.append(_T("\\"));
	appPath.append(_T("config.xml"));

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		// 설정 파일이 없을 때
// 		std::cout << "Parse error: " << result.description()
// 			<< ", character pos= " << result.offset;
		return;
	}

	pugi::xml_node application = doc.child("application");
	pugi::xml_node running_list = application.child("runnig_list");
	if (running_list) {
		pugi::xml_node future_list = running_list.child("future_list");
		if (future_list) {
			for (pugi::xml_node future_node = future_list.first_child(); future_node; future_node = future_node.next_sibling()) {
				std::string code = future_node.attribute("code").as_string();
				std::string name = future_node.attribute("name").as_string();
				std::string custom_name = future_node.attribute("custom_name").as_string();
				SmRunInfo run_info;
				run_info.Code = code;
				run_info.Name = name;
				run_info.UserDefinedName = custom_name;
				AddFutureRunInfo(run_info);
			}
		}

		pugi::xml_node option_list = running_list.child("option_list");
		if (option_list) {
			for (pugi::xml_node option_node = option_list.first_child(); option_node; option_node = option_node.next_sibling()) {
				std::string call_code = option_node.attribute("call").as_string();
				std::string put_code = option_node.attribute("put").as_string();
				std::string name = option_node.attribute("name").as_string();
				std::string code = option_node.child_value();
				SmRunInfo run_info;
				run_info.CallCode = call_code;
				run_info.PutCode = put_code;
				run_info.Name = name;
				run_info.UserDefinedName = name;
				run_info.Code = code;
				AddOptionRunInfo(run_info);
			}
		}
	}
}

std::vector<SmRunInfo> SmMarketManager::GetRunInfoList()
{
	std::vector<SmRunInfo> arg_list;
	for (auto it = _FutureRunVector.begin(); it != _FutureRunVector.end(); ++it) {
		SmRunInfo& info = *it;
		arg_list.push_back(info);
	}


	for (auto it = _OptionRunVector.begin(); it != _OptionRunVector.end(); ++it) {
		SmRunInfo& info = *it;
		arg_list.push_back(info);
	}


	return arg_list;
}

void SmMarketManager::AddProduct(std::shared_ptr<SmProduct> product)
{
	if (!product)
		return;
	_ProductMap[product->Code()] = product;
}

void SmMarketManager::InitDmFutures()
{
	_MainFutureVector.push_back("101F");
	_MainFutureVector.push_back("105F");
	_MainFutureVector.push_back("106F");
	_MainFutureVector.push_back("167F");
	_MainFutureVector.push_back("175F");
}


void SmMarketManager::InitAbroadProducts()
{
	_AbroadProductSet.insert("6A");
	_AbroadProductSet.insert("6B");
	_AbroadProductSet.insert("6E");
	_AbroadProductSet.insert("6J");
	_AbroadProductSet.insert("E7");
	_AbroadProductSet.insert("J7");
	_AbroadProductSet.insert("M6A");
	_AbroadProductSet.insert("M6B");
	_AbroadProductSet.insert("M6E");
	_AbroadProductSet.insert("ES");
	_AbroadProductSet.insert("NIY");
	_AbroadProductSet.insert("NKD");
	_AbroadProductSet.insert("NQ");
	_AbroadProductSet.insert("OES");
	_AbroadProductSet.insert("ONQ");
	_AbroadProductSet.insert("SP");
	_AbroadProductSet.insert("GE");
	_AbroadProductSet.insert("M2K"); // 미니러셀2000
	_AbroadProductSet.insert("CL"); // 크루드 오일
	_AbroadProductSet.insert("HO"); // 난방유
	_AbroadProductSet.insert("NG"); // 천연가스
	_AbroadProductSet.insert("QG"); // 미니천연가스
	_AbroadProductSet.insert("QM"); // 미니크루드 오일
	_AbroadProductSet.insert("RB"); // 가솔린
	_AbroadProductSet.insert("GC");
	_AbroadProductSet.insert("MGC");
	_AbroadProductSet.insert("OGC");
	_AbroadProductSet.insert("OSI");
	_AbroadProductSet.insert("QO");
	_AbroadProductSet.insert("SI");
	_AbroadProductSet.insert("YM");
	_AbroadProductSet.insert("ZF");
	_AbroadProductSet.insert("ZN");
	_AbroadProductSet.insert("ZQ");
	_AbroadProductSet.insert("OZC");
	_AbroadProductSet.insert("OZS");
	_AbroadProductSet.insert("OZW");
	_AbroadProductSet.insert("ZC");
	_AbroadProductSet.insert("ZL");
	_AbroadProductSet.insert("ZM");
	_AbroadProductSet.insert("ZS");
	_AbroadProductSet.insert("ZW");
	_AbroadProductSet.insert("CN");
	_AbroadProductSet.insert("NK");
	_AbroadProductSet.insert("TW");
	//	_AbroadProductSet.insert("BRN");
	_AbroadProductSet.insert("HSI"); // 항생
	_AbroadProductSet.insert("VX");
	_AbroadProductSet.insert("GF");
	_AbroadProductSet.insert("HF");
	_AbroadProductSet.insert("HG");
	_AbroadProductSet.insert("LE");
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
	std::shared_ptr<SmMarket> market = FindMarket(market_name);
	if (!market)
		return nullptr;
	std::shared_ptr<SmProduct> product = market->FindProduct(product_name);
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
	try {
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

		send_count++;

		LOG_F(INFO, _T(" symbol = %s, count : %d"), sym->SymbolCode().c_str(), send_count);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

std::shared_ptr<SmMarket> SmMarketManager::FindMarket(std::string mrkt_name)
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		std::shared_ptr<SmMarket> mrkt = *it;
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
