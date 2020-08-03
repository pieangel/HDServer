#include "pch.h"
#include "SmProduct.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "SmProductYearMonth.h"
#include "../Util/VtStringUtil.h"
SmProduct::SmProduct()
{
	_DomesticYearTable["A"] = "2006";
	_DomesticYearTable["B"] = "2007";
	_DomesticYearTable["C"] = "2008";
	_DomesticYearTable["D"] = "2009";
	_DomesticYearTable["E"] = "2010";
	_DomesticYearTable["F"] = "2011";
	_DomesticYearTable["G"] = "2012";
	_DomesticYearTable["H"] = "2013";
	_DomesticYearTable["J"] = "2014";
	_DomesticYearTable["K"] = "2015";
	_DomesticYearTable["L"] = "2016";
	_DomesticYearTable["M"] = "2017";
	_DomesticYearTable["N"] = "2018";
	_DomesticYearTable["P"] = "2019";
	_DomesticYearTable["Q"] = "2020";
	_DomesticYearTable["R"] = "2021";
	_DomesticYearTable["S"] = "2022";
	_DomesticYearTable["T"] = "2023";
	_DomesticYearTable["V"] = "2024";
	_DomesticYearTable["W"] = "2025";

	_DomesticMonthTable["1"] = "01";
	_DomesticMonthTable["2"] = "02";
	_DomesticMonthTable["3"] = "03";
	_DomesticMonthTable["4"] = "04";
	_DomesticMonthTable["5"] = "05";
	_DomesticMonthTable["6"] = "06";
	_DomesticMonthTable["7"] = "07";
	_DomesticMonthTable["8"] = "08";
	_DomesticMonthTable["9"] = "09";
	_DomesticMonthTable["A"] = "10";
	_DomesticMonthTable["B"] = "11";
	_DomesticMonthTable["C"] = "12";

	_AbroadMonthTable["F"] = "01";
	_AbroadMonthTable["G"] = "02";
	_AbroadMonthTable["H"] = "03";
	_AbroadMonthTable["J"] = "04";
	_AbroadMonthTable["K"] = "05";
	_AbroadMonthTable["M"] = "06";
	_AbroadMonthTable["N"] = "07";
	_AbroadMonthTable["Q"] = "08";
	_AbroadMonthTable["U"] = "09";
	_AbroadMonthTable["V"] = "10";
	_AbroadMonthTable["X"] = "11";
	_AbroadMonthTable["Z"] = "12";
}


SmProduct::~SmProduct()
{
	
}

std::shared_ptr<SmSymbol> SmProduct::AddSymbol(std::string symCode)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = std::make_shared<SmSymbol>();
	sym->SymbolCode(symCode);
	_SymbolList.push_back(sym);
	symMgr->AddSymbol(sym);
	//AddToYearMonth(symCode, sym);
	return sym;
}

std::shared_ptr<SmSymbol> SmProduct::GetRecentMonthSymbol()
{
	if (_SymbolList.size() == 0)
		return nullptr;
	return _SymbolList[0];
}

std::shared_ptr<SmSymbol> SmProduct::GetNextMonthSymbol()
{
	if (_SymbolList.size() < 2)
		return nullptr;
	return _SymbolList[1];
}

std::shared_ptr<SmProductYearMonth> SmProduct::GetRecentYearMonth()
{
	if (_YearMonthMap.size() == 0)
		return nullptr;
	else
		return _YearMonthMap.begin()->second;
}

std::shared_ptr<SmProductYearMonth> SmProduct::GetNextYearMonth()
{
	if (_YearMonthMap.size() <= 1)
		return nullptr;
	else
		return (++_YearMonthMap.begin())->second;
}

void SmProduct::AddToYearMonth(std::string symbol_code, std::shared_ptr<SmSymbol> symbol)
{
	if (!symbol)
		return;
	std::string local_date_time = VtStringUtil::getCurentDate();
	std::string local_year_month = local_date_time.substr(0, 6);

	if (isdigit(symbol_code.at(2))) { // 국내 상풍
		std::string product_code = symbol_code.substr(0, 3);
		std::string year = symbol_code.substr(3, 1);
		year = _DomesticYearTable[year];
		std::string month = symbol_code.substr(4, 1);
		month = _DomesticMonthTable[month];
		std::string year_month_temp = year + month;
		std::string year_month = year + "-" + month;
		if (year_month_temp.compare(local_year_month) < 0)
			return;
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
		std::string year = "20";
		year.append(symbol_code.substr(symbol_code.length() - 2, 2));
		std::string month = symbol_code.substr(symbol_code.length() - 3, 1);
		month = _AbroadMonthTable[month];
		std::string year_month = year + "-" + month;
		std::string year_month_temp = year + month;
		if (year_month_temp.compare(local_year_month) < 0)
			return;
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

// 주어진 날짜가 몇요일인지 알아내는 함수
int SmProduct::dow(int y, int m, int d)
{
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	y -= m < 3;
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
