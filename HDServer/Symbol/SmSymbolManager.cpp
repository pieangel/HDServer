#include "pch.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "../Log/loguru.hpp"
#include "../Config/SmConfigManager.h"
#include "../Util/SmUtil.h"
#include <chrono>
#include <sstream>
#include <fstream>
#include "../Util/VtStringUtil.h"
#include "../Log/loguru.hpp"
#include <algorithm>
#include "../Json/json.hpp"
#include "../Format/format.h"
#include "../Market/SmMarketManager.h"

using namespace nlohmann;

using namespace std::chrono;

SmSymbolManager::SmSymbolManager()
{
	
}


SmSymbolManager::~SmSymbolManager()
{
	
}

void SmSymbolManager::AddSymbol(std::shared_ptr<SmSymbol> sym)
{
	if (!sym)
		return;

	_SymbolMap[sym->SymbolCode()] = sym;
}

std::shared_ptr<SmSymbol> SmSymbolManager::FindSymbol(std::string symCode)
{
	auto it = _SymbolMap.find(symCode);
	if (it != _SymbolMap.end()) {
		return it->second;
	}

	return nullptr;
}

std::shared_ptr<HdProductInfo> SmSymbolManager::AddProductInfo(std::string code)
{
	std::shared_ptr<HdProductInfo> prdtInfo = FindProductInfo(code);
	if (!prdtInfo)
		prdtInfo =  std::make_shared<HdProductInfo>();
	prdtInfo->code = code;

	ProductInfoMap[code] = prdtInfo;

	return prdtInfo;
}

std::shared_ptr<HdProductInfo> SmSymbolManager::FindProductInfo(std::string code)
{
	auto it = ProductInfoMap.find(code);
	if (it != ProductInfoMap.end())
		return it->second;
	else
		return nullptr;
}


std::string& SmSymbolManager::TrimRight(std::string& input, char c)
{
	input.erase(input.find_last_not_of(c) + 1);
	return input;
}

std::string& SmSymbolManager::TrimLeft(std::string& input, char c)
{
	input.erase(0, input.find_first_not_of(c));
	return input;
}

std::string& SmSymbolManager::TrimBoth(std::string& input, char c)
{
	return TrimLeft(TrimRight(input, c), c);
}


void SmSymbolManager::StartCollectData()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::vector<std::shared_ptr<SmSymbol>> symbol_list = mrktMgr->GetRecentMonthSymbolList();

	for (auto it = symbol_list.begin(); it != symbol_list.end(); ++it) {
		std::shared_ptr<SmSymbol> symbol = *it;
		//if (symbol->SymbolCode().compare("CLN20") == 0) {
		//HdClient::GetInstance()->RegisterProduct(symbol->SymbolCode().c_str());
		_ChartSymbolVector.push_back(symbol);
		//}
	}

	StartTimer();
}

void SmSymbolManager::OnTimer()
{
// 	for (auto it = _ChartSymbolVector.begin(); it != _ChartSymbolVector.end(); ++it) {
// 		std::vector<int> date_time = SmUtil::GetLocalDateTime();
// 		int cur_hour = date_time[3];
// 		int cur_min = date_time[4];
// 		std::shared_ptr<SmSymbol> symbol = *it;
// 
// 
// 		int prev_hour = cur_hour;
// 		int prev_min = cur_min - 1;
// 		if (prev_min < 0) {
// 			prev_hour = prev_hour - 1;
// 			prev_min += 60;
// 		}
// 
// 		std::string prev_index = SmUtil::Format("%02d%02d", prev_hour, prev_min);
// 		std::string cur_index = SmUtil::Format("%02d%02d", cur_hour, cur_min);
// 		// 과거봉의 차트 데이터를 만든다.
// 		symbol->MakePrevChartDataByTimer(cur_index);
// 		// 현재 봉의 차트 데이터를 만든다.
// 		symbol->MakeCurrChartDataByTimer(cur_index);
// 	}
}

void SmSymbolManager::StartTimer()
{
	std::vector<int> date_time = SmUtil::GetLocalDateTime();
	int cur_min = date_time[4];

	int cycle = 1;
	int waitTime = 0;
	int cycle_time = cycle * 60;
	int total_seconds = cur_min * 60 + date_time[5];
	int mod_seconds = total_seconds % (cycle * 60);
	int wait_seconds = cycle * 60 - mod_seconds;
	waitTime = wait_seconds;

	auto id = _Timer.add(seconds(waitTime), std::bind(&SmSymbolManager::OnTimer, this), seconds(cycle_time));
	// Add to the request map.
	_CycleDataReqTimerMap[cycle] = id;
}

void SmSymbolManager::LoadProductInfo()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string path = configMgr->GetApplicationPath();
		path.append(_T("\\"));
		path.append("product.cod");
		std::ifstream file(path);

		if (!file.is_open())
			return;

		std::string content;
		std::vector<std::string> line;

		while (std::getline(file, content))
		{
			line.push_back(content);
		}

		std::shared_ptr<HdProductInfo> prdtInfo = nullptr;
		for (auto it = line.begin(); it != line.end(); ++it)
		{
			int index = 0;
			std::string info = *it;
			std::string temp;
			// 제품 코드
			temp = info.substr(index, 8);
			temp = TrimRight(temp, ' ');
			prdtInfo = AddProductInfo(temp);
			index = index + 8;
			// 소수점 자릿수
			temp = info.substr(index, 2);
			prdtInfo->decimal = std::stoi(temp);
			index = index + 2;
			// 틱크기
			temp = info.substr(index, 5);
			temp = TrimLeft(temp, ' ');
			prdtInfo->tickSize = temp;
			temp.erase(std::remove(temp.begin(), temp.end(), '.'), temp.end());
			prdtInfo->intTickSize = std::stoi(temp);
			index = index + 5;
			// 틱 가치
			temp = info.substr(index, 5);
			prdtInfo->tickValue = std::stoi(temp);
			index = index + 5;
			// 승수
			temp = info.substr(index, 10);
			temp = TrimLeft(temp, ' ');
			prdtInfo->tradeWin = std::stoi(temp);
			index = index + 10;
			// 제품 이름
			temp = info.substr(index, 30);
			temp = TrimLeft(temp, ' ');
			prdtInfo->name = temp;
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}
