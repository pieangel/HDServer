#pragma once
#include <string>
#include <vector>
#include <memory>
class SmSymbol;
class SmProductYearMonth
{
public:
	SmProductYearMonth();
	~SmProductYearMonth();
	std::string ProductCode;
	std::string YearMonthCode;
	std::vector<std::shared_ptr<SmSymbol>> SymbolList;
};

