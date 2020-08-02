#include "pch.h"
#include "SmCategory.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "SmProductYearMonth.h"
SmCategory::SmCategory()
{
}


SmCategory::~SmCategory()
{
	
}

std::shared_ptr<SmSymbol> SmCategory::AddSymbol(std::string symCode)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = std::make_shared<SmSymbol>();
	sym->SymbolCode(symCode);
	_SymbolList.push_back(sym);
	symMgr->AddSymbol(sym);
	AddToYearMonth(symCode, sym);
	return sym;
}

std::shared_ptr<SmSymbol> SmCategory::GetRecentMonthSymbol()
{
	if (_SymbolList.size() == 0)
		return nullptr;
	return _SymbolList[0];
}

std::shared_ptr<SmProductYearMonth> SmCategory::GetRecentYearMonth()
{
	if (_YearMonthMap.size() == 0)
		return nullptr;
	else
		return _YearMonthMap.begin()->second;
}

std::shared_ptr<SmProductYearMonth> SmCategory::GetNextYearMonth()
{
	if (_YearMonthMap.size() <= 1)
		return nullptr;
	else
		return (++_YearMonthMap.begin())->second;
}

void SmCategory::AddToYearMonth(std::string symbol_code, std::shared_ptr<SmSymbol> symbol)
{
	if (!symbol)
		return;

	if (std::isdigit(symbol_code.at(2))) { // 국내 상풍
		std::string product_code = symbol_code.substr(0, 3);
		std::string year_month = symbol_code.substr(3, 2);
		auto it = _YearMonthMap.find(year_month);
		std::shared_ptr<SmProductYearMonth> ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = std::make_shared<SmProductYearMonth>();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->SymbolList.push_back(symbol);
	}
	else { // 해외 상품
		std::string product_code = symbol_code.substr(0, 2);
		std::string year = symbol_code.substr(symbol_code.length() - 2, 2);
		std::string month = symbol_code.substr(symbol_code.length() - 3, 1);
		std::string year_month = year + month;
		auto it = _YearMonthMap.find(year_month);
		std::shared_ptr<SmProductYearMonth> ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = std::make_shared<SmProductYearMonth>();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->SymbolList.push_back(symbol);
	}
}

