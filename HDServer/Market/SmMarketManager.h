#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include "../Symbol/SmRunInfo.h"
class SmMarket;
class SmProduct;
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
	std::shared_ptr<SmMarket> AddMarket(std::string name);
	std::shared_ptr<SmProduct> FindProduct(std::string mrkt_name, std::string cat_code);
	std::shared_ptr<SmMarket> FindMarket(std::string mrkt_name);
	void AddCategoryMarket(std::string cat_code, std::string mrkt_name);
	std::shared_ptr<SmProduct> FindProduct(std::string cat_code);
	std::vector<std::shared_ptr<SmSymbol>> GetRecentMonthSymbolList();
	void SendMarketList(std::string user_id);
	void SendSymbolListByCategory(std::string user_id);
	void SendMarketList(int session_id);
	void SendSymbolListByCategory(int session_id);
	int GetTotalCategoryCount();
	int GetTotalSymbolCount();
	std::vector<std::shared_ptr<SmMarket>>& GetMarketList() {
		return _MarketList;
	}
	std::vector<std::string>& GetMainDmFutures() {
		return _MainFutureVector;
	}
	std::set<std::string>& GetAbroadProductSet() {
		return _AbroadProductSet;
	}
	void LoadRunInfo();
	std::vector<SmRunInfo> GetRunInfoList();
private:
	void AddFutureRunInfo(SmRunInfo run_info);
	void AddOptionRunInfo(SmRunInfo run_info);
	// 국내선물 목록을 초기화 한다.
	void InitDmFutures();
	// 해외선물 목록을 초기화 한다.
	void InitAbroadProducts();
	std::shared_ptr<SmSymbol> GetRecentSymbol(std::string market_name, std::string product_name);
	std::shared_ptr<SmSymbol> GetRecentSymbol(std::string product_name);
	void SendSymbolMaster(std::string user_id, std::shared_ptr<SmSymbol> sym);
	void SendSymbolMaster(int session_id, std::shared_ptr<SmSymbol> sym);
	// 시장 목록
	std::vector<std::shared_ptr<SmMarket>> _MarketList;
	/// 품목이 속한 시장 이름 대응 표
	/// <summary>
	/// 각 품목에 대하여 시장이름을 매칭시켜 준다.
	/// 키 : 품목 코드 
	/// 값 : 시장 이름
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
	// 국내 선물 주요 목록 - 운영되는 목록만 모아 놓은것
	std::vector<std::string> _MainFutureVector;
	// 해외 선물 목록
	std::set<std::string> _AbroadProductSet;
	// 국내 선물 모든 목록
	std::vector<SmRunInfo> _FutureRunVector;
	// 국내 옵션 모든 목록
	std::vector<SmRunInfo> _OptionRunVector;
};

