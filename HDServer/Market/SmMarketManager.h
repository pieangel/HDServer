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
	/// �̸����� ���� ��ü�� �߰��Ѵ�.
	/// </summary>
	/// <param name="name"></param>
	/// <returns>������ ���� ��ü�� �����ش�.</returns>
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
	// �������� ����� �ʱ�ȭ �Ѵ�.
	void InitDmFutures();
	// �ؿܼ��� ����� �ʱ�ȭ �Ѵ�.
	void InitAbroadProducts();
	std::shared_ptr<SmSymbol> GetRecentSymbol(std::string market_name, std::string product_name);
	std::shared_ptr<SmSymbol> GetRecentSymbol(std::string product_name);
	void SendSymbolMaster(std::string user_id, std::shared_ptr<SmSymbol> sym);
	void SendSymbolMaster(int session_id, std::shared_ptr<SmSymbol> sym);
	// ���� ���
	std::vector<std::shared_ptr<SmMarket>> _MarketList;
	/// ǰ���� ���� ���� �̸� ���� ǥ
	/// <summary>
	/// �� ǰ�� ���Ͽ� �����̸��� ��Ī���� �ش�.
	/// Ű : ǰ�� �ڵ� 
	/// �� : ���� �̸�
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
	// ���� ���� �ֿ� ��� - ��Ǵ� ��ϸ� ��� ������
	std::vector<std::string> _MainFutureVector;
	// �ؿ� ���� ���
	std::set<std::string> _AbroadProductSet;
	// ���� ���� ��� ���
	std::vector<SmRunInfo> _FutureRunVector;
	// ���� �ɼ� ��� ���
	std::vector<SmRunInfo> _OptionRunVector;
};

