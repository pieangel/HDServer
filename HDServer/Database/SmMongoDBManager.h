#pragma once
#include "../Global/TemplateSingleton.h"
#include <mongocxx\config\config.hpp>
#include "../Chart/SmChartDefine.h"
#include <tuple>
#include <string>
#include <vector>
#include <set>
#include "../Position/SmFee.h"
#include "../Order/SmTrade.h"
#include <mutex>
namespace mongocxx
{
	MONGOCXX_INLINE_NAMESPACE_BEGIN
		class instance;
		class client;
		class pool;
		class uri;
	MONGOCXX_INLINE_NAMESPACE_END
};

class SmAccount;
struct SmPosition;
struct SmOrder;
class SmMongoDBManager : public TemplateSingleton<SmMongoDBManager>
{
public:
	SmMongoDBManager();
	~SmMongoDBManager();
	void Test();
	void ReadSymbol();
	void LoadMarketList();
	void LoadSymbolList();
	void LoadRecentQuoteList();
	void LoadRecentHogaList();
	void SendChartDataFromDB(SmChartDataRequest&& data_req);
	void SendChartData(SmChartDataRequest data_req);
	void SendChartDataOneByOne(SmChartDataRequest data_req);
	void SendQuote(std::string symbol_code);
	void SendHoga(std::string symbol_code);
	void SendChartCycleData(SmChartDataRequest data_req);
	void SaveAccountNo(int type, int first, int middle, int last);
	void SaveCurrentOrderNo(int order_no);
	int GetOrderNo();
	std::tuple<int, int, int> GetAccountNo(int type = 0);
	void SaveUserInfo(std::string user_id, std::string pwd);
	std::pair<std::string, std::string> GetUserInfo(std::string user_id);
	bool RemoveUserInfo(std::string user_id);
	void SaveAccountInfo(std::shared_ptr<SmAccount> acnt);
	void UpdateAccountInfo(std::shared_ptr<SmAccount> acnt);
	std::vector<std::shared_ptr<SmAccount>> GetAccountList(std::string user_id);
	std::shared_ptr<SmAccount> GetAccount(std::string account_no);
	void AddPosition(std::shared_ptr<SmPosition> posi);
	void UpdatePosition(std::shared_ptr<SmPosition> posi);
	void AddOrder(std::shared_ptr<SmOrder> order);
	void OnAcceptedOrder(std::shared_ptr<SmOrder> order);
	void OnFilledOrder(std::shared_ptr<SmOrder> order);
	// 체결된 주문을 저장한다.
	void SaveFilledOrder(std::shared_ptr<SmOrder> order);
	// 체결된 주문 목록에서 주어진 조건의 주문을 삭제한다.
	void RemoveFilledOrder(std::string order_date, int order_no);
	void ChangeOrderState(std::shared_ptr<SmOrder> order);
	// 체결된 모든 주문을 로드한다.
	void LoadFilledOrders();
	std::vector<std::shared_ptr<SmOrder>> GetAcceptedOrderList(std::string account_no);
	std::vector<std::shared_ptr<SmOrder>> GetFilledOrderList(std::string account_no);
	std::vector<std::shared_ptr<SmOrder>> GetOrderList(std::string account_no);
	std::vector<std::shared_ptr<SmOrder>> GetOrderList(std::string account_no, int count, int skip);
	std::vector<std::shared_ptr<SmPosition>> GetPositionList(std::string account_no);
	std::shared_ptr<SmPosition> GetPosition(std::string account_no, std::string symbol_code);
	std::vector<std::shared_ptr<SmOrder>> GetOrderList(std::string date, std::string account_no);
	std::vector<std::shared_ptr<SmPosition>> GetPositionList(std::string date, std::string account_no);
	std::shared_ptr<SmPosition> GetPosition(std::string date, std::string account_no, std::string symbol_code);
	/// <summary>
	/// 현재 모든 계좌를 로드한다.
	/// </summary>
	void LoadAccountList();
	/// <summary>
	/// 현재 모든 체결된 주문을 로드한다.
	/// </summary>
	void LoadFilledOrderList();
	/// <summary>
	/// 현재 접수확인된 모든 주문을 로드한다.
	/// </summary>
	void LoadAcceptedOrderList();
	// 현재 존재하는 모든 포지션을 로드한다.
	void LoadPositionList();
	void SaveChartDataRequest(SmChartDataRequest req);
	void SendOrderList(int session_id, std::string account_no, std::string date_time);
	void SaveFee(std::shared_ptr<SmFee> fee);
	void LoadFee();
	// 매개변수 : 계좌, 포지션, 현재 실현 손익, 사용자 아이디, 실현손익을 발생시킨 주문의 갯수 
	void SaveTradePL(std::shared_ptr<SmAccount> account, std::shared_ptr<SmPosition> posi, double current_tradePL, std::string user_id, int liq_count, double liq_value);
	void LoadDailyChartData(SmChartDataRequest req);
	// 차트데이터를 가져온다.
	void LoadChartData(SmChartDataRequest req);
	// 차트데이터를 가져온다.
	void GetChartData(SmChartDataRequest req);
	std::vector<double> GetDailyData(SmChartDataRequest req);
	std::vector<SmTrade> GetTradeList(std::string user_id);
private:
	void SaveMarketsToDatabase();
	void SaveSymbolsToDatabase();
	void InitDatabase();
	mongocxx::instance* _Instance = nullptr;
	mongocxx::client* _Client = nullptr;
	mongocxx::pool* _ConnPool = nullptr;
	mongocxx::uri* _URI = nullptr;
	int _SendDataSplitSize = 20;
	bool _SendingHoga = false;
	std::mutex _mutex;
	std::set<std::string> _DomesticList;
};
