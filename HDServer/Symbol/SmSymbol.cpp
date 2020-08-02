#include "pch.h"
#include "SmSymbol.h"
#include "../Json/json.hpp"
#include "../Service/SmServiceDefine.h"
#include "../Database/SmTimeSeriesDBManager.h"
using namespace nlohmann;
SmSymbol::SmSymbol()
{
}


SmSymbol::~SmSymbol()
{
}

std::string SmSymbol::GetQuoteByJson()
{
	json quote;
	quote["res_id"] = SmProtocol::res_realtime_sise;
	quote["symbol_code"] = _SymbolCode;
	quote["time"] = Quote.OriginTime;
	quote["high"] = Quote.High;
	quote["low"] = Quote.Low;
	quote["open"] = Quote.Open;
	quote["close"] = Quote.Close;
	quote["sign_to_preday"] = Quote.SignToPreDay;
	quote["gap_from_preday"] = Quote.GapFromPreDay;
	quote["ratio_to_preday"] = Quote.RatioToPreday;

	return quote.dump();
}

std::string SmSymbol::GetQuoteByJson(int total, int current)
{
	json quote;
	quote["total_count"] = total;
	quote["current_count"] = current;
	quote["res_id"] = SmProtocol::res_sise_data;
	quote["symbol_code"] = _SymbolCode;
	quote["time"] = Quote.OriginTime;
	quote["high"] = Quote.High;
	quote["low"] = Quote.Low;
	quote["open"] = Quote.Open;
	quote["close"] = Quote.Close;
	quote["acc_volume"] = Quote.accVolume;
	quote["sign_to_preday"] = Quote.SignToPreDay;
	quote["gap_from_preday"] = Quote.GapFromPreDay;
	quote["ratio_to_preday"] = Quote.RatioToPreday;

	return quote.dump();
}

std::string SmSymbol::GetDummyQuoteByJson(int total, int current)
{
	json quote;
	quote["total_count"] = total;
	quote["current_count"] = current;
	quote["res_id"] = SmProtocol::res_sise_data;
	quote["symbol_code"] = "";
	quote["time"] = "";
	quote["high"] = 0;
	quote["low"] = 0;
	quote["open"] = 0;
	quote["close"] = 0;
	quote["acc_volume"] = 0;
	quote["sign_to_preday"] = "";
	quote["gap_from_preday"] = 0;
	quote["ratio_to_preday"] = "";

	return quote.dump();
}

std::string SmSymbol::GetHogaByJson()
{
	json hoga;
	hoga["res_id"] = SmProtocol::res_realtime_hoga;
	hoga["symbol_code"] = _SymbolCode;
	hoga["time"] = Hoga.Time;
	hoga["domestic_date"] = Hoga.DomesticDate;
	hoga["domestic_time"] = Hoga.DomesticTime;
	hoga["tot_buy_qty"] = Hoga.TotBuyQty;
	hoga["tot_sell_qty"] = Hoga.TotSellQty;
	hoga["tot_buy_cnt"] = Hoga.TotBuyCnt;
	hoga["tot_sell_cnt"] = Hoga.TotSellCnt;

	for (int i = 0; i < 5; i++) {
		hoga["hoga_items"][i]["buy_price"] = Hoga.Ary[i].BuyPrice;
		hoga["hoga_items"][i]["buy_cnt"] = Hoga.Ary[i].BuyCnt;
		hoga["hoga_items"][i]["buy_qty"] = Hoga.Ary[i].BuyQty;
		hoga["hoga_items"][i]["sell_price"] = Hoga.Ary[i].SellPrice;
		hoga["hoga_items"][i]["sell_cnt"] = Hoga.Ary[i].SellCnt;
		hoga["hoga_items"][i]["sell_qty"] = Hoga.Ary[i].SellQty;
	}

	return hoga.dump();
}

std::string SmSymbol::GetHogaByJson(int total, int current)
{
	json hoga;
	hoga["total_count"] = total;
	hoga["current_count"] = current;
	hoga["res_id"] = SmProtocol::res_hoga_data;
	hoga["symbol_code"] = _SymbolCode;
	hoga["time"] = Hoga.Time;
	hoga["domestic_date"] = Hoga.DomesticDate;
	hoga["domestic_time"] = Hoga.DomesticTime;
	hoga["tot_buy_qty"] = Hoga.TotBuyQty;
	hoga["tot_sell_qty"] = Hoga.TotSellQty;
	hoga["tot_buy_cnt"] = Hoga.TotBuyCnt;
	hoga["tot_sell_cnt"] = Hoga.TotSellCnt;

	for (int i = 0; i < 5; i++) {
		hoga["hoga_items"][i]["buy_price"] = Hoga.Ary[i].BuyPrice;
		hoga["hoga_items"][i]["buy_cnt"] = Hoga.Ary[i].BuyCnt;
		hoga["hoga_items"][i]["buy_qty"] = Hoga.Ary[i].BuyQty;
		hoga["hoga_items"][i]["sell_price"] = Hoga.Ary[i].SellPrice;
		hoga["hoga_items"][i]["sell_cnt"] = Hoga.Ary[i].SellCnt;
		hoga["hoga_items"][i]["sell_qty"] = Hoga.Ary[i].SellQty;
	}

	return hoga.dump();
}

std::string SmSymbol::GetDummyHogaByJson(int total, int current)
{
	json hoga;
	hoga["total_count"] = total;
	hoga["current_count"] = current;
	hoga["res_id"] = SmProtocol::res_hoga_data;
	hoga["symbol_code"] = "";
	hoga["time"] = "";
	hoga["domestic_date"] = "";
	hoga["domestic_time"] = "";
	hoga["tot_buy_qty"] = 0;
	hoga["tot_sell_qty"] = 0;
	hoga["tot_buy_cnt"] = 0;
	hoga["tot_sell_cnt"] = 0;

	for (int i = 0; i < 5; i++) {
		hoga["hoga_items"][i]["buy_price"] = 0;
		hoga["hoga_items"][i]["buy_cnt"] = 0;
		hoga["hoga_items"][i]["buy_qty"] = 0;
		hoga["hoga_items"][i]["sell_price"] = 0;
		hoga["hoga_items"][i]["sell_cnt"] = 0;
		hoga["hoga_items"][i]["sell_qty"] = 0;
	}

	return hoga.dump();
}
