#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <memory>
class SmSymbol;
class SmSymbolManager : public TemplateSingleton<SmSymbolManager>
{
public:
	SmSymbolManager();
	~SmSymbolManager();
	void AddSymbol(std::shared_ptr<SmSymbol> sym);
	std::shared_ptr<SmSymbol> FindSymbol(std::string symCode);
	std::map<std::string, std::shared_ptr<SmSymbol>>& GetSymbolMap() {
		return _SymbolMap;
	}
	
private:
	std::map<std::string, std::shared_ptr<SmSymbol>> _SymbolMap;
};

