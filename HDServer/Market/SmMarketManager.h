#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
class SmMarket;
class SmCategory;
class SmSymbol;
class SmMarketManager : public TemplateSingleton<SmMarketManager>
{
public:
	SmMarketManager();
	~SmMarketManager();
	void ReadSymbolsFromFile();
	/// <summary>
	/// 이름으로 시장 객체를 추가한다.
	/// </summary>
	/// <param name="name"></param>
	/// <returns>생성된 시장 객체를 돌려준다.</returns>
	SmMarket* AddMarket(std::string name);
	SmCategory* FindCategory(std::string mrkt_name, std::string cat_code);
	SmMarket* FindMarket(std::string mrkt_name);
	void AddCategoryMarket(std::string cat_code, std::string mrkt_name);
	SmCategory* FindCategory(std::string cat_code);
	std::vector<std::shared_ptr<SmSymbol>> GetRecentMonthSymbolList();
	void SendMarketList(std::string user_id);
	void SendSymbolListByCategory(std::string user_id);
	void SendMarketList(int session_id);
	void SendSymbolListByCategory(int session_id);
	int GetTotalCategoryCount();
	int GetTotalSymbolCount();
	std::vector<SmMarket*>& GetMarketList() {
		return _MarketList;
	}
private:
	std::shared_ptr<SmSymbol> GetRecentSymbol(std::string market_name, std::string product_name);
	std::shared_ptr<SmSymbol> GetRecentSymbol(std::string product_name);
	void SendSymbolMaster(std::string user_id, std::shared_ptr<SmSymbol> sym);
	void SendSymbolMaster(int session_id, std::shared_ptr<SmSymbol> sym);
	std::vector<SmMarket*> _MarketList;
	/// 품목이 속한 시장 이름 대응 표
	/// <summary>
	/// 각 품목에 대하여 시장이름을 매칭시켜 준다.
	/// 키 : 품목 코드 
	/// 값 : 시장 이름
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
};

