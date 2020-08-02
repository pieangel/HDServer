#include "SmMongoDBManager.h"
#include <chrono>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <string>
#include <iostream>
#include "../Market/SmMarketManager.h"
#include "../Symbol/SmSymbolReader.h"
#include "../Config/SmConfigManager.h"
#include "../Util/SmUtfUtil.h"
#include "../Market/SmMarket.h"
#include "../Market/SmCategory.h"
#include "../Symbol/SmSymbol.h"
#include <codecvt>
#include <locale>
#include "../Symbol/SmSymbolManager.h"
#include "../Service/SmTimeSeriesServiceManager.h"
#include "../Util/VtStringUtil.h"
#include "../Chart/SmChartDataManager.h"
#include "../Chart/SmChartData.h"
#include "../Service/SmServiceDefine.h"
#include "../Global/SmGlobal.h"
#include "../Account/SmAccount.h"
#include "../Server/SmSessionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrder.h"
#include "../Account/SmAccountManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Order/SmOrderNumberGenerator.h"
#include "../Json/json.hpp"
#include "../Config/SmConfigManager.h"
#include "../Xml/pugixml.hpp"
#include <windows.h>
#include "../Account/SmAccountManager.h"
#include "../Log/loguru.hpp"
#include <list>

using namespace nlohmann;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#define DebugOut( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
}

SmMongoDBManager::SmMongoDBManager()
{
	InitDatabase();
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append("\\Config\\Config.xml");
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node domestic_list_node = doc.child("application").child("domestic_list");
	pugi::xml_node domestic_node = domestic_list_node.first_child();
	while (domestic_node) {
		std::string code = domestic_node.text().as_string();
		_DomesticList.insert(code);
		domestic_node = domestic_node.next_sibling();
	}
}

SmMongoDBManager::~SmMongoDBManager()
{
	if (_Client) {
		delete _Client;
		_Client = nullptr;
	}

	

	if (_ConnPool) {
		delete _ConnPool;
		_ConnPool = nullptr;
	}

	if (_URI) {
		delete _URI;
		_URI = nullptr;
	}


	if (_Instance) {
		delete _Instance;
		_Instance = nullptr;
	}
}

void SmMongoDBManager::Test()
{
	if (!_Client)
		return;

	auto db = (*_Client)["andromeda"];

	// TODO: fix dates

	std::string name = "company";

	// @begin: cpp-insert-a-document
	bsoncxx::document::value restaurant_doc = make_document(
		kvp("address",
			make_document(kvp("street", "2 Avenue"),
				kvp("zipcode", 10075),
				kvp("building", "1480"),
				kvp("coord", make_array(-73.9557413, 40.7720266)))),
		kvp("borough", "Manhattan"),
		kvp("cuisine", "Italian"),
		kvp("grades",
			make_array(
				make_document(kvp("date", bsoncxx::types::b_date{ std::chrono::milliseconds{12323} }),
					kvp("grade", "A"),
					kvp("score", 11)),
				make_document(
					kvp("date", bsoncxx::types::b_date{ std::chrono::milliseconds{121212} }),
					kvp("grade", "B"),
					kvp("score", 17)))),
		kvp("name", name),
		kvp("restaurant_id", "41704623"));

	// We choose to move in our document here, which transfers ownership to insert_one()
	auto res = db["andromeda"].insert_one(std::move(restaurant_doc));
	// @end: cpp-insert-a-document

	auto builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value doc_value = builder
		<< "name" << "MongoDB"
		<< "type" << "database"
		<< "count" << 1
		<< "versions" << bsoncxx::builder::stream::open_array
		<< "v3.2" << "v3.0" << "v2.6"
		<< close_array
		<< "info" << bsoncxx::builder::stream::open_document
		<< "x" << 203
		<< "y" << 102
		<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::finalize;

	res = db["database"].insert_one(std::move(doc_value));


	std::vector<bsoncxx::document::value> documents;
	for (int i = 0; i < 100; i++) {
		documents.push_back(
			bsoncxx::builder::stream::document{} << "i" << i << finalize);
	}

	mongocxx::collection coll = db["test"];
	coll.insert_many(documents);


	bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
		coll.find_one(document{} << "i" << 18 << finalize);
	if (maybe_result) {
		std::cout << bsoncxx::to_json(*maybe_result) << "\n";
	}



	mongocxx::cursor cursor = coll.find(
		document{} << "i" << open_document <<
		"$gt" << 5 <<
		"$lte" << 10
		<< close_document << finalize);
	for (auto doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << "\n";
	}



	coll.update_one(document{} << "i" << 10 << finalize,
		document{} << "$set" << open_document <<
		"i" << 110 << close_document << finalize);

	bsoncxx::stdx::optional<mongocxx::result::update> result =
		coll.update_many(
			document{} << "i" << open_document <<
			"$lt" << 100 << close_document << finalize,
			document{} << "$inc" << open_document <<
			"i" << 100 << close_document << finalize);

	if (result) {
		std::cout << result->modified_count() << "\n";
	}

}

void SmMongoDBManager::ReadSymbol()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	std::string dir = symReader->GetWorkingDir();
	std::string name = dir;
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();

	//DbTest();

	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();

	mrktMgr->ReadSymbolsFromFile();

	SaveMarketsToDatabase();

	SaveSymbolsToDatabase();
}

void SmMongoDBManager::LoadMarketList()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["market_list"];
		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string market_name = json_object["market_name"];
			market_name = SmUtfUtil::Utf8ToAnsi(market_name);

			SmMarket* newMarket = marketMgr->AddMarket(market_name);
			int market_index = json_object["market_index"];
			newMarket->Name(market_name);
			newMarket->Index(market_index);
			// Get the product list
			auto product_list = json_object["product_list"];
			int count = product_list.size();
			for (int i = 0; i < count; ++i) {
				auto product = product_list[i];
				int product_index = product["product_index"];
				std::string product_code = product["product_code"];
				std::string product_name_kr = product["product_name_kr"];
				product_name_kr = SmUtfUtil::Utf8ToAnsi(product_name_kr);
				std::string product_name_en = product["product_name_en"];
				std::string exchange_name = product["exchange_name"];
				std::string exchange_code = product["exchange_code"];
				std::string market_name2 = product["market_name"];
				market_name2 = SmUtfUtil::Utf8ToAnsi(market_name2);

				// ETF가 아닐때는 국내 시장 목록에 있는지 확인을 한다.
				if (market_index != 8) {
					if (product_code.length() > 2 && std::isdigit(product_code.at(2))) {
						auto it = _DomesticList.find(product_code);
						if (it == _DomesticList.end())
							continue;
					}
				}
				
				SmCategory* category = newMarket->AddCategory(product_code);
				category->Code(product_code);
				category->Name(product_name_en);
				category->NameKr(product_name_kr);
				category->Exchange(exchange_name);
				category->ExchangeCode(exchange_code);
				category->MarketName(market_name2);
				marketMgr->AddCategoryMarket(product_code, market_name2);
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadSymbolList()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;
		mongocxx::collection coll = db["symbol_list"];
		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string market_name = json_object["market_name"];
			market_name = SmUtfUtil::Utf8ToAnsi(market_name);
			std::string product_code = json_object["product_code"];
			SmCategory* foundCategory = marketMgr->FindCategory(market_name, product_code);
			if (!foundCategory)
				continue;

			std::string symbol_code = json_object["symbol_code"];
			std::string symbol_name_kr = json_object["symbol_name_kr"];
			symbol_name_kr = SmUtfUtil::Utf8ToAnsi(symbol_name_kr);
			std::string symbol_name_en = json_object["symbol_name_en"];
			int symbol_index = json_object["symbol_index"];
			int decimal = json_object["decimal"];
			double contract_unit = json_object["contract_unit"];
			int seungsu = json_object["seungsu"];
			double tick_size = json_object["tick_size"];
			double tick_value = json_object["tick_value"];
			int atm = json_object["atm"];
			int near_month = json_object["near_month"];
			std::string last_date = json_object["last_date"];
			std::shared_ptr<SmSymbol> symbol = foundCategory->AddSymbol(symbol_code);
			symbol->Index(symbol_index);
			symbol->SymbolCode(symbol_code);
			symbol->CategoryCode(product_code);
			symbol->Name(symbol_name_kr);
			symbol->NameEn(symbol_name_en);
			symbol->CtrUnit(contract_unit);
			symbol->TickSize(tick_size);
			symbol->TickValue(tick_value);
			symbol->Decimal(decimal);
			symbol->Seungsu(seungsu);
			symbol->MarketName(market_name);
			symbol->Atm(atm);
			symbol->NearMonth(near_month);
			symbol->LastDate(last_date);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadRecentQuoteList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;
		mongocxx::collection coll = db["quote"];
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string symbol_code = json_object["symbol_code"];
			std::string sign_to_preday = json_object["sign_to_preday"];
			int gap_from_preday = json_object["gap_from_preday"];
			std::string ratio_to_preday = json_object["ratio_to_preday"];
			int acc_volume = json_object["acc_volume"];
			int open = json_object["open"];
			int high = json_object["high"];
			int low = json_object["low"];
			int close = json_object["close"];
			std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symbol_code);
			if (sym) {
				sym->Quote.SignToPreDay = sign_to_preday;
				sym->Quote.GapFromPreDay = gap_from_preday;
				sym->Quote.RatioToPreday = ratio_to_preday;
				sym->Quote.accVolume = acc_volume;
				sym->Quote.Open = open;
				sym->Quote.High = high;
				sym->Quote.Low = low;
				sym->Quote.Close = close;
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadRecentHogaList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;
		mongocxx::collection coll = db["hoga"];
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string symbol_code = json_object["symbol_code"];

			OutputDebugString("\n");
			OutputDebugString(symbol_code.c_str());
			
			std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symbol_code);
			if (sym) {
				int buy_price1 = json_object["buy_price1"];
				int buy_qty1 = json_object["buy_qty1"];
				int buy_cnt1 = json_object["buy_cnt1"];
				int sell_price1 = json_object["sell_price1"];
				int sell_qty1 = json_object["sell_qty1"];
				int sell_cnt1 = json_object["sell_cnt1"];

				sym->Hoga.Ary[0].BuyPrice = buy_price1;
				sym->Hoga.Ary[0].BuyQty = buy_qty1;
				sym->Hoga.Ary[0].BuyCnt = buy_cnt1;
				sym->Hoga.Ary[0].SellPrice = sell_price1;
				sym->Hoga.Ary[0].SellQty = sell_qty1;
				sym->Hoga.Ary[0].SellCnt = sell_cnt1;

				int buy_price2 = json_object["buy_price2"];
				int buy_qty2 = json_object["buy_qty2"];
				int buy_cnt2 = json_object["buy_cnt2"];
				int sell_price2 = json_object["sell_price2"];
				int sell_qty2 = json_object["sell_qty2"];
				int sell_cnt2 = json_object["sell_cnt2"];

				sym->Hoga.Ary[1].BuyPrice = buy_price2;
				sym->Hoga.Ary[1].BuyQty = buy_qty2;
				sym->Hoga.Ary[1].BuyCnt = buy_cnt2;
				sym->Hoga.Ary[1].SellPrice = sell_price2;
				sym->Hoga.Ary[1].SellQty = sell_qty2;
				sym->Hoga.Ary[1].SellCnt = sell_cnt2;

				int buy_price3 = json_object["buy_price3"];
				int buy_qty3 = json_object["buy_qty3"];
				int buy_cnt3 = json_object["buy_cnt3"];
				int sell_price3 = json_object["sell_price3"];
				int sell_qty3 = json_object["sell_qty3"];
				int sell_cnt3 = json_object["sell_cnt3"];

				sym->Hoga.Ary[2].BuyPrice = buy_price3;
				sym->Hoga.Ary[2].BuyQty = buy_qty3;
				sym->Hoga.Ary[2].BuyCnt = buy_cnt3;
				sym->Hoga.Ary[2].SellPrice = sell_price3;
				sym->Hoga.Ary[2].SellQty = sell_qty3;
				sym->Hoga.Ary[2].SellCnt = sell_cnt3;

				int buy_price4 = json_object["buy_price4"];
				int buy_qty4 = json_object["buy_qty4"];
				int buy_cnt4 = json_object["buy_cnt4"];
				int sell_price4 = json_object["sell_price4"];
				int sell_qty4 = json_object["sell_qty4"];
				int sell_cnt4 = json_object["sell_cnt4"];

				sym->Hoga.Ary[3].BuyPrice = buy_price4;
				sym->Hoga.Ary[3].BuyQty = buy_qty4;
				sym->Hoga.Ary[3].BuyCnt = buy_cnt4;
				sym->Hoga.Ary[3].SellPrice = sell_price4;
				sym->Hoga.Ary[3].SellQty = sell_qty4;
				sym->Hoga.Ary[3].SellCnt = sell_cnt4;

				int buy_price5 = json_object["buy_price5"];
				int buy_qty5 = json_object["buy_qty5"];
				int buy_cnt5 = json_object["buy_cnt5"];
				int sell_price5 = json_object["sell_price5"];
				int sell_qty5 = json_object["sell_qty5"];
				int sell_cnt5 = json_object["sell_cnt5"];

				sym->Hoga.Ary[4].BuyPrice = buy_price5;
				sym->Hoga.Ary[4].BuyQty = buy_qty5;
				sym->Hoga.Ary[4].BuyCnt = buy_cnt5;
				sym->Hoga.Ary[4].SellPrice = sell_price5;
				sym->Hoga.Ary[4].SellQty = sell_qty5;
				sym->Hoga.Ary[4].SellCnt = sell_cnt5;

				int tot_buy_qty = json_object["tot_buy_qty"];
				int tot_buy_cnt = json_object["tot_buy_cnt"];
				int tot_sell_qty = json_object["tot_sell_qty"];
				int tot_sell_cnt = json_object["tot_sell_cnt"];
				std::string domestic_date = json_object["domestic_date"];
				std::string domestic_time = json_object["domestic_time"];
				std::string time = json_object["time"];

				sym->Hoga.TotBuyCnt = tot_buy_cnt;
				sym->Hoga.TotBuyQty = tot_buy_qty;
				sym->Hoga.TotSellCnt = tot_sell_cnt;
				sym->Hoga.TotSellQty = tot_sell_qty;
				sym->Hoga.DomesticDate = domestic_date;
				sym->Hoga.DomesticTime = domestic_time;
				sym->Hoga.Time = time;
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartDataFromDB(SmChartDataRequest&& data_req)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];
		
		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		long long data_count = coll.count_documents({});
		// 데이터가 없거나 요청한 갯수보다 적으면 서버에 요청을 한다.
		if (data_count == 0 || data_count < data_req.count) {
			if (tsMgr->GetSiseSocketCount() > 0) {
				//SendChartDataOneByOne(data_req);
				tsMgr->ResendChartDataRequest(data_req);
				return;
			}
		} 

		auto dt = VtStringUtil::GetCurrentDateTimeNoSecond();
		std::string d_t = dt.first + dt.second;

		bsoncxx::stdx::optional<bsoncxx::document::value> found_symbol =
			coll.find_one(bsoncxx::builder::stream::document{} << "date_time" << d_t << finalize);
		// 최신 데이터가 현재 날짜와 같지 않으면 서버에 요청한다.
		if (!found_symbol) {
			if (tsMgr->GetSiseSocketCount() > 0) {
				//SendChartDataOneByOne(data_req);
				tsMgr->ResendChartDataRequest(data_req);
				return;
			}
		}
		
		SendChartDataOneByOne(data_req);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartDataOneByOne(SmChartDataRequest data_req)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		
		opts.sort(make_document(kvp("date_time", -1)));
		
		auto item_count = db[data_req.GetDataKey()].count_documents({});
		if (item_count == 0)
			return;

		int64_t limit = item_count > data_req.count ? data_req.count : item_count;
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(limit);

		mongocxx::cursor cursor = coll.find({}, opts);
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
		int count = 1;
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			SmChartDataItem data;
			data.current_count = count;
			data.total_count = (int)limit;
			data.symbolCode = data_req.symbolCode;
			data.chartType = data_req.chartType;
			data.cycle = data_req.cycle;
			data.date = date;
			data.time = time;
			data.h = h;
			data.l = l;
			data.o = o;
			data.c = c;
			data.v = v;

			char buffer[4096];
			sprintf(buffer, "SendChartDataOnebyOne%s\n", date_time.c_str());
			OutputDebugString(buffer);

			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->SendChartData(data_req, data);
			count++;
			
		}

		
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartData(SmChartDataRequest data_req)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(data_req.count);
		mongocxx::cursor cursor = coll.find({}, opts);
		//int total_count = std::distance(cursor.begin(), cursor.end());
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			SmChartDataItem data;
			data.symbolCode = data_req.symbolCode;
			data.chartType = data_req.chartType;
			data.cycle = data_req.cycle;
			data.date = date;
			data.time = time;
			data.h = h;
			data.l = l;
			data.o = o;
			data.c = c;
			data.v = v;
			chart_data->PushChartDataItemToFront(data);
		}

		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		tsMgr->SendChartData(data_req, chart_data);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendQuote(std::string symbol_code)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["quote"];

		
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "symbol_code" << symbol_code << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			json_object["res_id"] = (int)SmProtocol::res_realtime_sise;
			std::string content = json_object.dump(4);
			SmGlobal* global = SmGlobal::GetInstance();
			std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
			sessMgr->send(content);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendHoga(std::string symbol_code)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["hoga"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "symbol_code" << symbol_code << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			json_object["res_id"] = (int)SmProtocol::res_realtime_hoga;
			std::string content = json_object.dump(4);
			SmGlobal* global = SmGlobal::GetInstance();
			std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
			sessMgr->send(content);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartCycleData(SmChartDataRequest data_req)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(data_req.count);

		json send_object;
		send_object["res_id"] = SmProtocol::res_chart_cycle_data;
		send_object["symbol_code"] = data_req.symbolCode;
		send_object["chart_type"] = (int)data_req.chartType;
		send_object["cycle"] = data_req.cycle;
		send_object["total_count"] = data_req.count;

		mongocxx::cursor cursor = coll.find({}, opts);
		//int total_count = std::distance(cursor.begin(), cursor.end());
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
		int k = 0;
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			send_object["data"][k++] = {
			{ "date_time",  date_time },
			{ "high", h },
			{ "low",  l },
			{ "open",  o },
			{ "close", c },
			{ "volume", v }
			};
		}

		std::string content = send_object.dump(4);
		SmGlobal* global = SmGlobal::GetInstance();
		std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
		sessMgr->send(content);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

std::tuple<int, int, int> SmMongoDBManager::GetAccountNo(int type)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_no"];

		std::string account_type;
		type == 0 ? account_type = "abroad" : account_type = "domestic";

		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "account_type" << account_type << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			int first = json_object["first"];
			int second = json_object["second"];
			int last = json_object["last"];
			return std::make_tuple(first, second, last);
		}
		else {
			if (type == 0) // 해외
				return std::make_tuple(100, 1000, 1000);
			else // 국내
				return std::make_tuple(300, 1000, 1000);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return std::make_tuple(100, 1000, 1000);
}

void SmMongoDBManager::SaveUserInfo(std::string user_id, std::string pwd)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["user_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> found_user =
			coll.find_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
		if (found_user) {
			coll.update_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "password" << pwd
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "user_id" << user_id
				<< "password" << pwd
				<< bsoncxx::builder::stream::finalize;
			auto res = db["user_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

std::pair<std::string, std::string> SmMongoDBManager::GetUserInfo(std::string user_id)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["user_list"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			std::string pwd = json_object["password"];
			return std::make_pair(user_id, pwd);
		}
		else {
			return std::make_pair("", "");
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return std::make_pair("", "");
}

bool SmMongoDBManager::RemoveUserInfo(std::string user_id)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["user_list"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
		if (maybe_result) {
			coll.delete_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
			return true;
		}
		else {
			return false;
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return false;
}


void SmMongoDBManager::SaveAccountInfo(std::shared_ptr<SmAccount> acnt)
{
	if (!acnt)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_acnt =
			coll.find_one(bsoncxx::builder::stream::document{} << "account_no" << acnt->AccountNo() << finalize);
		if (fount_acnt) {
			coll.update_one(bsoncxx::builder::stream::document{} << "account_no" << acnt->AccountNo() << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "account_type" << acnt->AccountType()
				<< "account_no" << acnt->AccountNo()
				<< "user_id" << acnt->UserID()
				<< "account_name" << acnt->AccountName()
				<< "password" << acnt->Password()
				<< "initial_balance" << acnt->InitialBalance()
				<< "trade_profit_loss" << acnt->TradePL()
				<< "open_profit_loss" << acnt->OpenPL()
				<< "total_trade_profit_loss" << acnt->TotalTradePL()
				<< "fee_count" << acnt->FeeCount()
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "account_type" << acnt->AccountType()
				<< "account_no" << acnt->AccountNo()
				<< "user_id" << acnt->UserID()
				<< "account_name" << acnt->AccountName()
				<< "password" << acnt->Password()
				<< "initial_balance" << acnt->InitialBalance()
				<< "trade_profit_loss" << acnt->TradePL()
				<< "open_profit_loss" << acnt->OpenPL()
				<< "total_trade_profit_loss" << acnt->TotalTradePL()
				<< "fee_count" << acnt->FeeCount()
				<< bsoncxx::builder::stream::finalize;
			auto res = db["account_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

// 매매수익과 평가 손익을 업데이트 한다.
void SmMongoDBManager::UpdateAccountInfo(std::shared_ptr<SmAccount> acnt)
{
	if (!acnt)
		return;
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_acnt =
			coll.find_one(bsoncxx::builder::stream::document{} 
				<< "account_no" << acnt->AccountNo() 
				<< finalize);
		if (fount_acnt) {
			coll.update_one(bsoncxx::builder::stream::document{} << "account_no" << acnt->AccountNo() << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "initial_balance" << acnt->InitialBalance()
				<< "trade_profit_loss" << acnt->TradePL()
				<< "open_profit_loss" << acnt->OpenPL()
				<< "total_trade_profit_loss" << acnt->TotalTradePL()
				<< "fee_count" << acnt->FeeCount()
				<< close_document << finalize);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

/*
auto cursor = db["restaurants"].find(
	make_document(kvp("$or",
		make_array(make_document(kvp("cuisine", "Italian")),
			make_document(kvp("address.zipcode", "10075"))))));
*/

std::vector<std::shared_ptr<SmAccount>> SmMongoDBManager::GetAccountList(std::string user_id)
{
	std::vector<std::shared_ptr<SmAccount>> account_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];

		auto cursor2 = db["restaurants"].find(
			make_document(kvp("$or",
				make_array(make_document(kvp("cuisine", "Italian")),
					make_document(kvp("address.zipcode", "10075"))))));

		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{} 
			<< "user_id" << user_id << finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
			std::string account_no = json_object["account_no"];
			std::string user_id = json_object["user_id"];
			std::string account_name = json_object["account_name"];
			std::string password = json_object["password"];
			double initial_valance = json_object["initial_balance"];
			double trade_profit_loss = json_object["trade_profit_loss"];
			double open_profit_loss = json_object["open_profit_loss"];
			double total_trade_profit_loss = json_object["total_trade_profit_loss"];
			double fee_count = json_object["fee_count"];
			int account_type = json_object["account_type"];
			acnt->AccountType(account_type);
			acnt->AccountNo(account_no);
			acnt->AccountName(account_name);
			acnt->UserID(user_id);
			acnt->Password(password);
			acnt->InitialBalance(initial_valance);
			acnt->TradePL(trade_profit_loss);
			acnt->OpenPL(open_profit_loss);
			acnt->FeeCount((int)fee_count);
			acnt->TotalTradePL(total_trade_profit_loss);
			account_list.push_back(acnt);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return account_list;
}

std::shared_ptr<SmAccount> SmMongoDBManager::GetAccount(std::string account_no)
{
	std::shared_ptr<SmAccount> acnt = nullptr;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
		<< "account_no" << account_no << finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			acnt = std::make_shared<SmAccount>();
			std::string account_no = json_object["account_no"];
			std::string user_id = json_object["user_id"];
			std::string account_name = json_object["account_name"];
			std::string password = json_object["password"];
			double initial_valance = json_object["initial_balance"];
			double trade_profit_loss = json_object["trade_profit_loss"];
			double open_profit_loss = json_object["open_profit_loss"];
			int account_type = json_object["account_type"];
			acnt->AccountType(account_type);
			acnt->AccountNo(account_no);
			acnt->AccountName(account_name);
			acnt->UserID(user_id);
			acnt->Password(password);
			acnt->InitialBalance(initial_valance);
			acnt->TradePL(trade_profit_loss);
			acnt->OpenPL(open_profit_loss);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return acnt;
}

void SmMongoDBManager::AddPosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> found_posi =
			coll.find_one(bsoncxx::builder::stream::document{} 
				<< "account_no" << posi->AccountNo 
				<< "symbol_code" << posi->SymbolCode
				<< finalize);
		if (found_posi) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee_count" << posi->FeeCount
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< "user_id" << posi->UserID
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee_count" << posi->FeeCount
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< "user_id" << posi->UserID
				<< bsoncxx::builder::stream::finalize;
			auto res = db["position_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::UpdatePosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> found_posi =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< finalize);
		if (found_posi) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee_count" << posi->FeeCount
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< "user_id" << posi->UserID
				<< close_document << finalize);

			/*
			// 잔고가 있을 때는 포지션을 업데이트 한다.
			if (posi->OpenQty != 0) {
				
			}
			
			else { // 잔고가 없을 때는 포지션을 없앤다.
				coll.delete_one(bsoncxx::builder::stream::document{} 
					<< "account_no" << posi->AccountNo
					<< "symbol_code" << posi->SymbolCode
					<< finalize);
			}
			*/
		}
		else { // 포지션이 없을 때는 삽입해 준다.
			bsoncxx::document::value doc_value = builder
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee_count" << posi->FeeCount
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< "user_id" << posi->UserID
				<< bsoncxx::builder::stream::finalize;
			auto res = db["position_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}


void SmMongoDBManager::AddOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_date" << order->OrderDate
				<< "order_time" << order->OrderTime
				<< "order_no" << order->OrderNo
				<< "account_no" << order->AccountNo
				<< "symbol_code" << order->SymbolCode
				<< "order_amount" << order->OrderAmount
				<< "price_type" << (int)order->PriceType
				<< "order_price" << order->OrderPrice
				<< "position_type" << (int)order->Position
				<< "order_type" << (int)order->OrderType
				<< "ori_order_no" << order->OriOrderNo
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_condition" << (int)order->FilledCondition
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "order_state" << (int)order->OrderState
				<< "symbol_decimal" << order->SymbolDecimal
				<< "remain_qty" << order->RemainQty
				<< "strategy_name" << order->StrategyName
				<< "system_name" << order->SystemName
				<< "fund_name" << order->FundName
				<< "user_id" << order->UserID
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "order_date" << order->OrderDate
				<< "order_time" << order->OrderTime
				<< "order_no" << order->OrderNo
				<< "account_no" << order->AccountNo
				<< "symbol_code" << order->SymbolCode
				<< "order_amount" << order->OrderAmount
				<< "price_type" << (int)order->PriceType
				<< "order_price" << order->OrderPrice
				<< "position_type" << (int)order->Position
				<< "order_type" << (int)order->OrderType
				<< "ori_order_no" << order->OriOrderNo
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_condition" << (int)order->FilledCondition
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "order_state" << (int)order->OrderState
				<< "symbol_decimal" << order->SymbolDecimal
				<< "remain_qty" << order->RemainQty
				<< "strategy_name" << order->StrategyName
				<< "system_name" << order->SystemName
				<< "fund_name" << order->FundName
				<< "user_id" << order->UserID
				<< bsoncxx::builder::stream::finalize;
			auto res = db["order_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::OnAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_state" << (int)order->OrderState
				<< close_document << finalize);
		}
		
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::OnFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "remain_qty" << order->RemainQty
				<< "order_state" << (int)order->OrderState
				<< close_document << finalize);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["filled_order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_date" << order->OrderDate
				<< "order_time" << order->OrderTime
				<< "order_no" << order->OrderNo
				<< "account_no" << order->AccountNo
				<< "symbol_code" << order->SymbolCode
				<< "order_amount" << order->OrderAmount
				<< "price_type" << (int)order->PriceType
				<< "order_price" << order->OrderPrice
				<< "position_type" << (int)order->Position
				<< "order_type" << (int)order->OrderType
				<< "ori_order_no" << order->OriOrderNo
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_condition" << (int)order->FilledCondition
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "order_state" << (int)order->OrderState
				<< "symbol_decimal" << order->SymbolDecimal
				<< "remain_qty" << order->RemainQty
				<< "strategy_name" << order->StrategyName
				<< "system_name" << order->SystemName
				<< "fund_name" << order->FundName
				<< "user_id" << order->UserID
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "order_date" << order->OrderDate
				<< "order_time" << order->OrderTime
				<< "order_no" << order->OrderNo
				<< "account_no" << order->AccountNo
				<< "symbol_code" << order->SymbolCode
				<< "order_amount" << order->OrderAmount
				<< "price_type" << (int)order->PriceType
				<< "order_price" << order->OrderPrice
				<< "position_type" << (int)order->Position
				<< "order_type" << (int)order->OrderType
				<< "ori_order_no" << order->OriOrderNo
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_condition" << (int)order->FilledCondition
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "order_state" << (int)order->OrderState
				<< "symbol_decimal" << order->SymbolDecimal
				<< "remain_qty" << order->RemainQty
				<< "strategy_name" << order->StrategyName
				<< "system_name" << order->SystemName
				<< "fund_name" << order->FundName
				<< "user_id" << order->UserID
				<< bsoncxx::builder::stream::finalize;
			auto res = db["filled_order_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::RemoveFilledOrder(std::string order_date, int order_no)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["filled_order_list"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} 
				<< "order_date" << order_date 
				<< "order_no" << order_no
				<< finalize);
		if (maybe_result) {
			coll.delete_one(bsoncxx::builder::stream::document{} 
				<< "order_date" << order_date
				<< "order_no" << order_no
				<< finalize);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::ChangeOrderState(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_state" << (int)order->OrderState
				<< "remain_qty" << order->RemainQty
				<< close_document << finalize);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadFilledOrders()
{

}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetAcceptedOrderList(std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< "order_state" << (int)SmOrderState::Accepted
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetFilledOrderList(std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< "order_state" << (int)SmOrderState::Filled
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetOrderList(std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			//<< "order_date" << date_time.first
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();
			
			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetOrderList(std::string date, std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			//<< "order_date" << date
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetOrderList(std::string account_no, int count, int skip)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("order_no", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(count);
		opts.skip(skip);

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(
			bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< finalize, opts);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmPosition>> SmMongoDBManager::GetPositionList(std::string account_no)
{
	std::vector<std::shared_ptr<SmPosition>> posi_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
		
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"].get<std::string>();
			posi->CreatedTime = json_object["created_time"].get<std::string>();
			posi->SymbolCode = json_object["symbol_code"].get<std::string>();
			posi->AccountNo = json_object["account_no"].get<std::string>();
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->FeeCount =json_object["fee_count"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];
			posi->UserID = json_object["user_id"].get<std::string>();
			
			posi_list.push_back(posi);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return posi_list;
}

std::vector<std::shared_ptr<SmPosition>> SmMongoDBManager::GetPositionList(std::string date, std::string account_no)
{
	std::vector<std::shared_ptr<SmPosition>> posi_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"].get<std::string>();
			posi->CreatedTime = json_object["created_time"].get<std::string>();
			posi->SymbolCode = json_object["symbol_code"].get<std::string>();
			posi->AccountNo = json_object["account_no"].get<std::string>();
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->FeeCount = json_object["fee_count"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];
			posi->UserID = json_object["user_id"].get<std::string>();

			posi_list.push_back(posi);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return posi_list;
}

std::shared_ptr<SmPosition> SmMongoDBManager::GetPosition(std::string account_no, std::string symbol_code)
{
	std::shared_ptr<SmPosition> posi = nullptr;
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date_time.first
			<< "account_no" << account_no
			<< "symbol_code" << symbol_code
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"].get<std::string>();
			posi->CreatedTime = json_object["created_time"].get<std::string>();
			posi->SymbolCode = json_object["symbol_code"].get<std::string>();
			posi->AccountNo = json_object["account_no"].get<std::string>();
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->FeeCount = json_object["fee_count"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];
			posi->UserID = json_object["user_id"].get<std::string>();

		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return posi;

}

std::shared_ptr<SmPosition> SmMongoDBManager::GetPosition(std::string date, std::string account_no, std::string symbol_code)
{
	std::shared_ptr<SmPosition> posi = nullptr;
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date
			<< "account_no" << account_no
			<< "symbol_code" << symbol_code
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"].get<std::string>();
			posi->CreatedTime = json_object["created_time"].get<std::string>();
			posi->SymbolCode = json_object["symbol_code"].get<std::string>();
			posi->AccountNo = json_object["account_no"].get<std::string>();
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->FeeCount = json_object["fee_count"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];
			posi->UserID = json_object["user_id"].get<std::string>();

		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return posi;
}

void SmMongoDBManager::LoadAccountList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		SmAccountManager* acntMgr = SmAccountManager::GetInstance();

		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			auto id = json_object["$oid"];
			std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
			std::string account_no = json_object["account_no"];
			std::string user_id = json_object["user_id"];
			std::string account_name = json_object["account_name"];
			std::string password = json_object["password"];
			double initial_valance = json_object["initial_balance"];
			double trade_profit_loss = json_object["trade_profit_loss"];
			double open_profit_loss = json_object["open_profit_loss"];
			double total_trade_profit_loss = json_object["total_trade_profit_loss"];
			int fee_count = json_object["fee_count"];
			acnt->AccountNo(account_no);
			acnt->AccountName(account_name);
			acnt->UserID(user_id);
			acnt->Password(password);
			acnt->InitialBalance(initial_valance);
			acnt->TradePL(trade_profit_loss);
			acnt->OpenPL(open_profit_loss);
			acnt->TotalTradePL(total_trade_profit_loss);
			acnt->FeeCount(fee_count);
			acntMgr->AddAccount(acnt);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadFilledOrderList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		SmTotalOrderManager* totalOrderMgr = SmTotalOrderManager::GetInstance();

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "order_state" << (int)SmOrderState::Filled
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			//SmOrderNumberGenerator::SetID(order->OrderNo);
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();
			// 체결된 주문에 추가시킨다.
			totalOrderMgr->AddFilledOrder(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadAcceptedOrderList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		SmTotalOrderManager* totalOrderMgr = SmTotalOrderManager::GetInstance();

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "order_state" << (int)SmOrderState::Accepted
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"].get<std::string>();
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"].get<std::string>();
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			//SmOrderNumberGenerator::SetID(order->OrderNo);
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"].get<std::string>();
			order->FilledTime = json_object["filled_time"].get<std::string>();
			order->OrderDate = json_object["order_date"].get<std::string>();
			order->OrderTime = json_object["order_time"].get<std::string>();
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"].get<std::string>();
			order->SystemName = json_object["system_name"].get<std::string>();
			order->FundName = json_object["fund_name"].get<std::string>();
			order->UserID = json_object["user_id"].get<std::string>();

			totalOrderMgr->AddAcceptedOrder(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadPositionList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();

		SmTotalPositionManager* tpMgr = SmTotalPositionManager::GetInstance();
		
		// 잔고가 0보다 큰 모든 포지션을 가져온다.
		/*
		mongocxx::cursor cursor = coll.find(
			bsoncxx::builder::stream::document{} << "open_qty" << open_document <<
			"$gt" << 0
			<< close_document << finalize);
			*/
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"].get<std::string>();
			posi->CreatedTime = json_object["created_time"].get<std::string>();
			posi->SymbolCode = json_object["symbol_code"].get<std::string>();
			posi->AccountNo = json_object["account_no"].get<std::string>();
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->FeeCount = json_object["fee_count"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];
			posi->UserID = json_object["user_id"].get<std::string>();

			tpMgr->AddPosition(posi);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveChartDataRequest(SmChartDataRequest req)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["chart_data_request"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_no =
			coll.find_one(bsoncxx::builder::stream::document{} << "symbol_code" << req.symbolCode << "chart_type" << (int)req.chartType << "chart_cycle" << req.cycle << finalize);
		if (!fount_no)
		{
			bsoncxx::document::value doc_value = builder
				<< "symbol_code" << req.symbolCode
				<< "chart_type" << (int)req.chartType
				<< "chart_cycle" << req.cycle
				<< bsoncxx::builder::stream::finalize;
			auto res = db["chart_data_request"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendOrderList(int session_id, std::string account_no, std::string date_time)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		std::vector<std::shared_ptr<SmOrder>> order_list = GetOrderList(date_time, account_no);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}


void SmMongoDBManager::SaveFee(std::shared_ptr<SmFee> fee)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["fee_info"];

		builder::stream::document builder{};

		bsoncxx::document::value doc_value = builder
			<< "account_no" << fee->AccountNo
			<< "symbol_code" << fee->SymbolCode
			<< "date" << fee->Date
			<< "time" << fee->Time
			<< "fee_count" << fee->FeeCount
			<< bsoncxx::builder::stream::finalize;
		auto res = db["fee_info"].insert_one(std::move(doc_value));
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadFee()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["fee_info"];

		builder::stream::document builder{};

		std::string date = VtStringUtil::getCurentDate();

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "date" << date
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::string account_no = json_object["account_no"];
			std::shared_ptr<SmAccount> account = SmAccountManager::GetInstance()->FindAccount(account_no);
			if (account) {
				std::shared_ptr<SmFee> fee = std::make_shared<SmFee>();
				std::string symbol_code = json_object["symbol_code"];
				fee->SymbolCode = symbol_code;
				fee->AccountNo = account_no;
				fee->Date = json_object["date"].get<std::string>();
				fee->FeeCount = json_object["fee_count"];
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveTradePL(std::shared_ptr<SmAccount> account, std::shared_ptr<SmPosition> posi, double current_tradePL, std::string user_id, int liq_count, double liq_value)
{
	if (!account)
		return;

	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["trade_profit_loss_history"];

		builder::stream::document builder{};

		bsoncxx::document::value doc_value = builder
			<< "date_time" << date_time.first + date_time.second
			<< "date" << date_time.first
			<< "time" << date_time.second
			<< "account_no" << account->AccountNo()
			<< "symbol_code" << posi->SymbolCode
			<< "liq_value" << liq_value // 수익을 발생시킨 가격
			<< "liq_count" << liq_count // 수익을 발생시킨 주문의 갯수
			<< "current_profit_loss" << current_tradePL // 지금 발생한 수익
			<< "trade_profit_loss" << posi->TradePL // 종목 누적 수익
			<< "acc_profit_loss" << account->TradePL() // 계좌 누적 수익
			<< "total_profit_loss" << account->TotalTradePL() // 계좌 개설 후 총 누적 수익
			<< "current_account_amount" << account->InitialBalance() // 현재 계좌의 총액
			<< "user_id" << user_id
			<< bsoncxx::builder::stream::finalize;
		auto res = db["trade_profit_loss_history"].insert_one(std::move(doc_value));
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadDailyChartData(SmChartDataRequest req)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[req.GetDailyKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;

		// 과거의 것이 뒤에 오게 한다.
		opts.sort(make_document(kvp("date_time", -1)));

		auto item_count = db[req.GetDailyKey()].count_documents({});
		if (item_count == 0)
			return;

		int64_t limit = item_count > req.count ? req.count : item_count;
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(limit);

		mongocxx::cursor cursor = coll.find({}, opts);
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
		int count = 1;
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			SmChartDataItem data;
			data.current_count = count;
			data.total_count = (int)limit;
			data.symbolCode = req.symbolCode;
			data.chartType = req.chartType;
			data.cycle = req.cycle;
			data.date = date;
			data.time = time;
			data.date_time = date_time;
			data.h = h;
			data.l = l;
			data.o = o;
			data.c = c;
			data.v = v;

			char buffer[4096];
			sprintf(buffer, "SendChartDataOnebyOne count = %d, o = %d, h = %d, l = %d, c = %d, %s\n", count, o, h, l, c, date_time.c_str());
			OutputDebugString(buffer);

			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->SendChartData(req, data);
			count++;

		}
		
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadChartData(SmChartDataRequest req)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		std::string data_key;
		if (req.chartType == SmChartType::DAY)
			data_key = req.GetDailyKey();
		else
			data_key = req.GetDataKey();

		mongocxx::collection coll = db[data_key];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;

		// 과거의 것이 앞에 오게 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		// 데이터의 갯수를 읽어 온다.
		auto item_count = db[data_key].count_documents({});
		if (item_count == 0)
			return;

		// 최대 갯수를 넘지않게 옵션을 조절해 준다.
		int64_t limit = item_count > req.count ? req.count : item_count;
		// 전체 데이터에서 최신의 것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		opts.limit(limit);

		mongocxx::cursor cursor = coll.find({}, opts);
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
		std::list<SmChartDataItem> data_list;
		int count = 1;
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			SmChartDataItem data;
			data.current_count = count;
			data.total_count = (int)limit;
			data.symbolCode = req.symbolCode;
			data.chartType = req.chartType;
			data.cycle = req.cycle;
			data.date = date;
			data.time = time;
			data.date_time = date_time;
			data.h = h;
			data.l = l;
			data.o = o;
			data.c = c;
			data.v = v;

			char buffer[4096];
			sprintf(buffer, "LoadChartData symbol_code = %s, date_time = %s, cycle = %d, count = %d, o = %d, h = %d, l = %d, c = %d\n", req.symbolCode.c_str(), date_time.c_str(), req.cycle, count, o, h, l, c);
			OutputDebugString(buffer);

			data_list.push_front(data);

			// 차트데이터에 추가한다.
			chart_data->AddData(data);

			count++;

		}

		count = 1;
		for (auto it = data_list.begin(); it != data_list.end(); ++it) {
			SmChartDataItem data = *it;
			data.current_count = count;
			// 차트데이터를 보낸다.
			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->SendChartData(req, data);
			count++;
		}
		// 차트 데이터를 받았을 설정한다.
		chart_data->Received(true);

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
		LOG_F(INFO, "LoadChartData exception msg = %s", e.what());
	}
}

void SmMongoDBManager::GetChartData(SmChartDataRequest req)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		std::string data_key;
		if (req.chartType == SmChartType::DAY)
			data_key = req.GetDailyKey();
		else
			data_key = req.GetDataKey();

		mongocxx::collection coll = db[data_key];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;

		// 과거의 것이 앞에 오게 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		// 데이터의 갯수를 읽어 온다.
		auto item_count = db[data_key].count_documents({});
		if (item_count == 0)
			return;

		// 최대 갯수를 넘지않게 옵션을 조절해 준다.
		int64_t limit = item_count > req.count ? req.count : item_count;
		// 전체 데이터에서 최신의 것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		opts.limit(limit);

		mongocxx::cursor cursor = coll.find({}, opts);
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
		std::list<SmChartDataItem> data_list;
		int count = 1;
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			SmChartDataItem data;
			data.current_count = count;
			data.total_count = (int)limit;
			data.symbolCode = req.symbolCode;
			data.chartType = req.chartType;
			data.cycle = req.cycle;
			data.date = date;
			data.time = time;
			data.date_time = date_time;
			data.h = h;
			data.l = l;
			data.o = o;
			data.c = c;
			data.v = v;

			char buffer[4096];
			sprintf(buffer, "LoadChartData symbol_code = %s, date_time = %s, cycle = %d, count = %d, o = %d, h = %d, l = %d, c = %d\n", req.symbolCode.c_str(), date_time.c_str(), req.cycle, count, o, h, l, c);
			//OutputDebugString(buffer);

			data_list.push_front(data);

			// 차트데이터에 추가한다.
			chart_data->AddData(data);

			count++;

		}

		// 차트 데이터를 받았을 설정한다.
		chart_data->Received(true);

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
		LOG_F(INFO, "LoadChartData exception msg = %s", e.what());
	}
}

std::vector<double> SmMongoDBManager::GetDailyData(SmChartDataRequest req)
{
	std::vector<double> close_vector;

	try
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[req.GetDailyKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;

		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		// 과거의 것이 뒤에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		auto item_count = db[req.GetDailyKey()].count_documents({});
		if (item_count == 0)
			return close_vector;

		int64_t limit = item_count > req.count ? req.count : item_count;
		
		opts.limit(limit);

		mongocxx::cursor cursor = coll.find({}, opts);
		
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];
			
			close_vector.push_back(c);
		}

		return close_vector;

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return close_vector;
}

void SmMongoDBManager::SaveAccountNo(int type, int first, int second, int last)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["account_no"];

		builder::stream::document builder{};

		std::string account_type;

		type == 0 ? account_type = "abroad" : account_type = "domestic";

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_no =
			coll.find_one(bsoncxx::builder::stream::document{} << "account_type" << account_type << finalize);
		if (fount_no) {
			coll.update_one(bsoncxx::builder::stream::document{} << "account_type" << account_type << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "account_type" << account_type
				<< "first" << first
				<< "second" << second
				<< "last" << last
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "account_type" << account_type
				<< "first" << first
				<< "second" << second
				<< "last" << last
				<< bsoncxx::builder::stream::finalize;
			auto res = db["account_no"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveCurrentOrderNo(int order_no)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["order_no"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_no =
			coll.find_one(bsoncxx::builder::stream::document{} << "current_no" << "current_no" << finalize);
		if (fount_no) {
			coll.update_one(bsoncxx::builder::stream::document{} << "current_no" << "current_no" << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "current_no" << "current_no"
				<< "order_no" << order_no
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "current_no" << "current_no"
				<< "order_no" << order_no
				<< bsoncxx::builder::stream::finalize;
			auto res = db["order_no"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

int SmMongoDBManager::GetOrderNo()
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_no"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "current_no" << "current_no" << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			int order_no = json_object["order_no"];
			return order_no;
		}
		else {
			return 1;
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return 1;
}

std::vector<SmTrade> SmMongoDBManager::GetTradeList(std::string user_id)
{
	std::vector<SmTrade> trade_list;
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["trade_profit_loss_history"];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;

		// 과거의 것이 뒤에 오게 한다.
		opts.sort(make_document(kvp("date_time", -1)));

		


		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "user_id" << user_id
			<< finalize, opts);
		
		//mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
		//	<< "user_id" << user_id
		//	<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			SmTrade trade;
			trade.date_time = json_object["date_time"].get<std::string>();
			trade.date = json_object["date"].get<std::string>();
			trade.time = json_object["time"].get<std::string>();
			trade.account_no = json_object["account_no"].get<std::string>();
			trade.symbol_code = json_object["symbol_code"].get<std::string>();
			trade.liq_count = json_object["liq_count"];
			trade.liq_value = json_object["liq_value"];
			trade.current_profit_loss = json_object["current_profit_loss"];
			trade.trade_profit_loss = json_object["trade_profit_loss"];
			trade.total_profit_loss = json_object["total_profit_loss"];
			//total_account_amount
			trade.total_account_amount = json_object["current_account_amount"];
			trade.total_account_amount = round(trade.total_account_amount * 100) / 100;
			trade_list.push_back(trade);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return trade_list;
}

void SmMongoDBManager::SaveMarketsToDatabase()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["market_list"];
		
		builder::stream::document builder{};

		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		std::vector<SmMarket*>& marketList = marketMgr->GetMarketList();
		for (size_t i = 0; i < marketList.size(); ++i) {
			SmMarket* market = marketList[i];
			bsoncxx::stdx::optional<bsoncxx::document::value> found_market =
				coll.find_one(bsoncxx::builder::stream::document{} << "market_name" << SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str()) << finalize);
			if (!found_market) {
				auto in_array = builder << "product_list" << builder::stream::open_array;
				std::vector<SmCategory*>& catVec = market->GetCategoryList();
				for (size_t j = 0; j < catVec.size(); ++j) {
					SmCategory* cat = catVec[j];
					bsoncxx::stdx::optional<bsoncxx::document::value> found_product =
						coll.find_one(bsoncxx::builder::stream::document{} << "prodcut_list.product_code" << cat->Code() << finalize);
					if (!found_product) {
						in_array = in_array << builder::stream::open_document
							<< "product_index" << (int)j
							<< "product_code" << cat->Code()
							<< "product_name_kr" << SmUtfUtil::AnsiToUtf8((char*)cat->NameKr().c_str())
							<< "product_name_en" << cat->Name()
							<< "exchange_name" << cat->Exchange()
							<< "exchange_code" << cat->ExchangeCode()
							<< "market_name" << SmUtfUtil::AnsiToUtf8((char*)cat->MarketName().c_str())
							<< builder::stream::close_document;
					}
				}
				auto after_array = in_array << builder::stream::close_array;
				after_array << "market_index" << (int)i
					<< "market_name" << SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str());
				bsoncxx::document::value doc = after_array << builder::stream::finalize;
				auto res = db["market_list"].insert_one(std::move(doc));
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveSymbolsToDatabase()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["symbol_list"];

		builder::stream::document builder{};

		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		std::vector<SmMarket*>& marketList = marketMgr->GetMarketList();

		for (size_t i = 0; i < marketList.size(); ++i) {
			SmMarket* market = marketList[i];
			std::vector<SmCategory*>& cat_list = market->GetCategoryList();
			for (size_t j = 0; j < cat_list.size(); ++j) {
				SmCategory* cat = cat_list[j];
				std::vector<std::shared_ptr<SmSymbol>>& sym_list = cat->GetSymbolList();
				for (size_t k = 0; k < sym_list.size(); ++k) {
					std::shared_ptr<SmSymbol> sym = sym_list[k];
					bsoncxx::stdx::optional<bsoncxx::document::value> found_symbol =
						coll.find_one(bsoncxx::builder::stream::document{} << "symbol_list" << sym->SymbolCode() << finalize);
					if (!found_symbol) {
						bsoncxx::document::value doc_value = builder
							<< "symbol_code" << sym->SymbolCode()
							<< "symbol_index" << sym->Index()
							<< "symbol_name_kr" << SmUtfUtil::AnsiToUtf8((char*)sym->Name().c_str())
							<< "symbol_name_en" << sym->NameEn()
							<< "product_code" << sym->CategoryCode()
							<< "market_name" << SmUtfUtil::AnsiToUtf8((char*)sym->MarketName().c_str())
							<< "decimal" << sym->Decimal()
							<< "contract_unit" << sym->CtrUnit()
							<< "seungsu" << sym->Seungsu()
							<< "tick_size" << sym->TickSize()
							<< "tick_value" << sym->TickValue()
							<< bsoncxx::builder::stream::finalize;
						auto res = db["symbol_list"].insert_one(std::move(doc_value));
					}
				}
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::InitDatabase()
{
	_Instance = new mongocxx::instance();
	//_URI = new mongocxx::uri{ "mongodb://localhost:27017/?minPoolSize=3&maxPoolSize=10" };
	//_Client = new mongocxx::client(mongocxx::uri{});
	_ConnPool = new mongocxx::pool(mongocxx::uri{ "mongodb://localhost:27017/?minPoolSize=3&maxPoolSize=10" });
	//_ConnPool = new mongocxx::pool(mongocxx::uri{ "mongodb://angelpie.tplinkdns.com:27017/?minPoolSize=3&maxPoolSize=10" });
}
