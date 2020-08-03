#include "pch.h"
#include "SmSymbol.h"
#include "../Json/json.hpp"
#include "../Service/SmServiceDefine.h"
#include "../Database/SmTimeSeriesDBManager.h"
#include "../Symbol/SmQuoteDefine.h"
#include "../Util/SmUtil.h"
#include "../Server/SmSessionManager.h"
#include "../Service/SmServiceDefine.h"
#include "../Config/SmConfigManager.h"
#include <chrono>
#include <sstream>
#include <fstream>
#include "../Util/VtStringUtil.h"
#include "../Log/loguru.hpp"
#include <algorithm>
#include "../Chart/SmChartDataManager.h"
#include "../Chart/SmChartData.h"
#include "../Log/loguru.hpp"
#include "afxwin.h"

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
	quote["acc_volume"] = Quote.accVolume;
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


void SmSymbol::MakeCurrChartDataByTimer(std::string cur_hour_min)
{
	std::lock_guard<std::mutex> lock(_mutex);

	try {

		if (_MinDataMap.size() == 0)
			return;

		std::string start_hourmin = SmUtil::Format("%02d%02d", _StartHour, _StartMin);
		std::string end_hourmin = SmUtil::Format("%02d%02d", _EndHour, _EndMin);
		int start = std::stoi(start_hourmin);
		int end = std::stoi(end_hourmin);
		int cur = std::stoi(cur_hour_min);

		if (std::isdigit(_SymbolCode.at(2))) { // 국내선물
			if (cur < start || cur > end + 1)
				return;
		}
		else { // 해외선물
			if (end < cur && cur < start)
				return;
		}

		// 기존 차트 데이터에서 찾아봐야 한다.
		std::vector<int> cycle_vector;
		cycle_vector.push_back(1);
		cycle_vector.push_back(5);
		cycle_vector.push_back(10);
		cycle_vector.push_back(15);
		cycle_vector.push_back(30);
		cycle_vector.push_back(60);

		int cur_hour = std::stoi(cur_hour_min.substr(0, 2));
		int cur_min = std::stoi(cur_hour_min.substr(2, 2));

		auto it = _MinDataMap.rbegin();
		SmChartDataItem last_data = it->second;

		std::string prev_date, prev_time;

		for (size_t i = 0; i < cycle_vector.size(); ++i) {
			int cycle = cycle_vector[i];

			int next_hour = cur_hour;
			int next_min = cur_min + cycle;
			if (next_min >= 60) {
				next_hour = next_hour + 1;
				next_min = 60 - next_min;
			}
			int hour_diff = 0;
			if (next_hour >= StartHour()) {
				hour_diff = next_hour - StartHour();
			}
			else {
				hour_diff = next_hour - StartHour();
				hour_diff += 24;
			}
			int min_diff = 0;
			if (next_min >= StartMin()) {
				min_diff = next_min - StartMin() + 1;
			}
			else {
				min_diff = next_min - StartMin();
				min_diff += 60 + 1;
			}

			int total_min_diff = hour_diff * 60 + min_diff;

			// 주기가 아니면 만들지 않는다.
			if (cycle != 1 && total_min_diff % cycle != 1)
				continue;
			// 타이머에 의해서 만들어진 데이터는 저장하지 않는다.
			if (last_data.temp)
				continue;




			SmChartDataItem data;
			data.chartType = SmChartType::MIN;
			data.symbolCode = _SymbolCode;
			data.cycle = cycle;
			auto local_date = VtStringUtil::getCurentDate();
			data.date = local_date;

			std::string next_index = SmUtil::Format("%02d%02d", next_hour, next_min);
			std::string time = next_index;
			time.append("00");
			data.time = time;

			SendCycleChartData(data);

			SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
			std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
			if (chart_data) {

				data.o = last_data.c;
				data.h = last_data.c;
				data.l = last_data.c;
				data.c = last_data.c;
				data.v = last_data.v;


				chart_data->AddChartData(std::move(data));

				CString msg;
				msg.Format(_T("MakeCurrChartDataByTimer ::code = %s, cycle = %d, date = %s, t = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), data.symbolCode.c_str(), data.cycle, data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
				TRACE(msg);
			}
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void SmSymbol::MakePrevChartDataByTimer(std::string cur_hour_min)
{
	try {
		std::lock_guard<std::mutex> lock(_mutex);

		int cur_hour = std::stoi(cur_hour_min.substr(0, 2));
		int cur_min = std::stoi(cur_hour_min.substr(2, 2));


		std::vector<int> date_time = SmUtil::GetLocalDateTime();
		//int cur_hour = date_time[3];
		//int cur_min = date_time[4];
		int hour_diff = 0;

		if (cur_hour >= StartHour()) {
			hour_diff = cur_hour - StartHour();
		}
		else {
			hour_diff = cur_hour - StartHour();
			hour_diff += 24;
		}
		int min_diff = 0;
		if (cur_min >= StartMin()) {
			min_diff = cur_min - StartMin() + 1;
		}
		else {
			min_diff = cur_min - StartMin();
			min_diff += 60 + 1;
		}

		int total_min_diff = hour_diff * 60 + min_diff;

		CString msg;
		msg.Format("symbolcode = %s, starthour = %d, startmin = %d, cur_hour = %d, cur_min = %d, total_min_diff = %d\n", SymbolCode().c_str(), StartHour(), StartMin(), cur_hour, cur_min, total_min_diff);
		TRACE(msg);

		std::vector<int> cycle_vector;
		cycle_vector.push_back(1);
		cycle_vector.push_back(5);
		cycle_vector.push_back(10);
		cycle_vector.push_back(15);
		cycle_vector.push_back(30);
		cycle_vector.push_back(60);


		int prev_hour = cur_hour;
		int prev_min = cur_min - 1;
		if (prev_min < 0) {
			prev_hour = prev_hour - 1;
			prev_min += 60;
		}

		std::string prev_index = SmUtil::Format("%02d%02d", prev_hour, prev_min);
		std::string cur_index = SmUtil::Format("%02d%02d", cur_hour, cur_min);

		for (size_t i = 0; i < cycle_vector.size(); ++i) {
			int cycle = cycle_vector[i];
			// 먼저 모아진 분데이터에서 차트 주기데이터를 만든다. 모든 주기는 1분 데이터를 기준으로 한다.
			MakePrevChartData(total_min_diff, SymbolCode(), prev_index, cur_index, cycle);
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void SmSymbol::UpdateMinData(SmQuoteData tick_data)
{
	try {
		std::string hour_min = tick_data.time.substr(0, 4);
		int close = std::stoi(tick_data.close);
		auto it = _MinDataMap.find(hour_min);
		if (it != _MinDataMap.end()) { // 데이터가 있는 경우 업데이트 한다.
			SmChartDataItem& data = it->second;
			// 타이머에 의해 임시로 만들어진 데이터일 경우 모두 교체해 준다.
			if (data.temp) {
				data.o = close;
				data.h = close;
				data.l = close;
				data.c = close;
				data.v = std::stoi(tick_data.volume);
			}
			else { // 틱에 의해 만들어진 데이터이면 업데이트 해준다.
				// 고가 업데이트
				if (close > data.h)
					data.h = close;
				// 저가 업데이트
				if (close < data.l)
					data.l = close;
				// 종가 업데이트
				data.c = close;
				// 거래량 업데이트
				data.v += std::stoi(tick_data.volume);
			}
		}
		else {
			SmChartDataItem data;
			data.chartType = SmChartType::MIN;
			data.symbolCode = tick_data.symbol_code;
			data.cycle = 1;
			if (tick_data.local_date.length() == 0) {
				auto local_date = VtStringUtil::getCurentDate();
				data.date = local_date;
			}
			auto local_date = VtStringUtil::getCurentDate();
			data.date = local_date;
			std::string time = hour_min;
			time.append("00");
			data.time = time;

			data.o = close;
			data.h = close;
			data.l = close;
			data.c = close;
			data.v = std::stoi(tick_data.volume);
			_MinDataMap[hour_min] = data;

			std::vector<int> cycle_vector;
			cycle_vector.push_back(1);
			cycle_vector.push_back(5);
			cycle_vector.push_back(10);
			cycle_vector.push_back(15);
			cycle_vector.push_back(30);
			cycle_vector.push_back(60);

			int cur_hour = std::stoi(hour_min.substr(0, 2));
			int cur_min = std::stoi(hour_min.substr(2, 2));


			for (size_t i = 0; i < cycle_vector.size(); ++i) {
				int cycle = cycle_vector[i];
				int next_hour = cur_hour;
				int next_min = cur_min + cycle;
				if (next_min >= 60) {
					next_hour = next_hour + 1;
					next_min = 60 - next_min;
				}
				int hour_diff = 0;
				if (next_hour >= StartHour()) {
					hour_diff = next_hour - StartHour();
				}
				else {
					hour_diff = next_hour - StartHour();
					hour_diff += 24;
				}
				int min_diff = 0;
				if (next_min >= StartMin()) {
					min_diff = next_min - StartMin() + 1;
				}
				else {
					min_diff = next_min - StartMin();
					min_diff += 60 + 1;
				}

				int total_min_diff = hour_diff * 60 + min_diff;

				// 주기가 아니면 만들지 않는다.
				if (cycle != 1 && total_min_diff % cycle != 1)
					continue;


				data.chartType = SmChartType::MIN;
				data.symbolCode = tick_data.symbol_code;
				data.cycle = cycle;
				if (tick_data.local_date.length() == 0) {
					auto local_date = VtStringUtil::getCurentDate();
					data.date = local_date;
				}
				std::string next_index = SmUtil::Format("%02d%02d", next_hour, next_min);
				std::string time = next_index;
				time.append("00");
				data.time = time;

				data.o = close;
				data.h = close;
				data.l = close;
				data.c = close;
				data.v = std::stoi(tick_data.volume);

				SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
				std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
				if (chart_data) {
					chart_data->AddChartData(std::move(data));

					CString msg;
					msg.Format(_T("UpdateMinData ::code = %s, cycle = %d, date = %s, t = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), data.symbolCode.c_str(), data.cycle, data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
					TRACE(msg);
				}
				else {
					chart_data = chartDataMgr->AddChartData(data);
					chart_data->AddChartData(std::move(data));
					CString msg;
					msg.Format(_T("UpdateMinData ::code = %s, cycle = %d, date = %s, t = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), data.symbolCode.c_str(), data.cycle, data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
					TRACE(msg);
				}
			}
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void SmSymbol::UpdateTickData(SmQuoteData tick_data)
{
	try {
		tick_count_120++;
		tick_count_300++;

		int close = std::stoi(tick_data.close);
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		// 여기서 새로운 데이터를 만들고 완성된 봉을 보낸다.
		if (tick_count_120 % 120 == 0) {
			SmChartDataItem data;
			data.cycle = 120;
			data.chartType = SmChartType::TICK;
			data.symbolCode = _SymbolCode;

			std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
			// 차트 데이터가 없으면 만든다.
			if (!chart_data) {
				chart_data = chartDataMgr->AddChartData(tick_data.symbol_code, (int)SmChartType::TICK, 120);
			}

			// 완성된 봉을 먼저 보낸다.
			SmChartDataItem* prev_data = chart_data->GetChartDataItem(last_tick_time_120);
			if (prev_data) {
				SendCycleChartData(*prev_data);
			}

			auto local_date = VtStringUtil::getCurentDate();
			data.date = local_date;
			data.time = tick_data.time;
			data.o = close;
			data.h = close;
			data.l = close;
			data.c = close;
			data.v = std::stoi(tick_data.volume);
			// 새로운 데이터를 보낸다.
			SendCycleChartData(data);
			// 새로운 데이터를 추가해 준다.
			chart_data->AddChartData(std::move(data));
			// 마지막 시간을 저장해 준다.
			last_tick_time_120 = tick_data.time;
		}
		else {
			SmChartDataItem data;
			data.cycle = 120;
			data.chartType = SmChartType::TICK;
			data.symbolCode = _SymbolCode;
			std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
			// 차트 데이터가 없으면 만든다.
			if (!chart_data) {
				chart_data = chartDataMgr->AddChartData(tick_data.symbol_code, (int)SmChartType::TICK, 120);
			}
			SmChartDataItem* prev_data = chart_data->GetChartDataItem(last_tick_time_120);
			// 기존 데이터가 있으면 기존 데이터를 업데이트 해준다.
			if (prev_data) {
				if (close > prev_data->h)
					prev_data->h = close;
				if (close < prev_data->l)
					prev_data->l = close;
				prev_data->c = close;
				prev_data->v += std::stoi(tick_data.volume);
			}
		}
		// 여기서 새로운 데이터를 만들고 완성된 봉을 보낸다.
		if (tick_count_300 % 300 == 0) {
			SmChartDataItem data;
			data.cycle = 300;
			data.chartType = SmChartType::TICK;
			data.symbolCode = _SymbolCode;
			std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
			// 차트 데이터가 없으면 만든다.
			if (!chart_data) {
				chart_data = chartDataMgr->AddChartData(tick_data.symbol_code, (int)SmChartType::TICK, 300);
			}

			// 완성된 봉을 먼저 보낸다.
			SmChartDataItem* prev_data = chart_data->GetChartDataItem(last_tick_time_300);
			if (prev_data) {
				SendCycleChartData(*prev_data);
			}

			auto local_date = VtStringUtil::getCurentDate();
			data.date = local_date;
			data.time = tick_data.time;
			data.o = close;
			data.h = close;
			data.l = close;
			data.c = close;
			data.v = std::stoi(tick_data.volume);
			// 새로운 데이터를 보낸다.
			SendCycleChartData(data);
			// 새로운 데이터를 추가해 준다.
			chart_data->AddChartData(std::move(data));
			// 마지막 시간을 저장해 준다.
			last_tick_time_300 = tick_data.time;
		}
		else {
			SmChartDataItem data;
			data.cycle = 300;
			data.chartType = SmChartType::TICK;
			data.symbolCode = _SymbolCode;

			std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
			// 차트 데이터가 없으면 만든다.
			if (!chart_data) {
				chart_data = chartDataMgr->AddChartData(tick_data.symbol_code, (int)SmChartType::TICK, 300);
			}
			SmChartDataItem* prev_data = chart_data->GetChartDataItem(last_tick_time_300);
			// 기존 데이터가 있으면 기존 데이터를 업데이트 해준다.
			if (prev_data) {
				if (close > prev_data->h)
					prev_data->h = close;
				if (close < prev_data->l)
					prev_data->l = close;
				prev_data->c = close;
				prev_data->v += std::stoi(tick_data.volume);
			}
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void SmSymbol::UpdateChartData(SmQuoteData tick_data)
{
	for (auto it = _ChartDataMap.begin(); it != _ChartDataMap.end(); ++it) {
		std::shared_ptr<SmChartData> chart_data = it->second;
		if (chart_data->Received()) {
			chart_data->UpdateChartData(tick_data);
		}
	}
}

void SmSymbol::MakePrevChartData(int total_min_diff, std::string symbol_code, std::string prev_hour_min, std::string chart_hour_min, int cycle)
{
	try {

		// 모아진 데이터가 없으면 만들지 않는다.
		if (_MinDataMap.size() < (size_t)cycle)
			return;

		// 주기가 아니면 만들지 않는다.
		if (cycle != 1 && total_min_diff % cycle != 1)
			return;

		// 현재 시간이 과거의 봉을 표현한다.
		// 5분봉 차트일 경우 9시 00분이면 차트 표시는 9시 5분차트가 된다.
		// 따라서 현재 시간이 곧 과거의 차트가 된다.
		auto end = _MinDataMap.find(prev_hour_min);
		if (end == _MinDataMap.end())
			return;

		SmChartDataItem data;
		data.chartType = SmChartType::MIN;
		auto local_date = VtStringUtil::getCurentDate();
		data.date = local_date;
		data.cycle = cycle;
		std::string time = chart_hour_min;
		time.append("00");
		data.time = time;
		data.symbolCode = symbol_code;

		for (int i = cycle - 1; i >= 0; i--) {
			auto it = std::prev(end, i);
			if (it == _MinDataMap.end())
				return;

			std::string key = it->first;
			CString msg;
			msg.Format(_T("MakePrevChartData for time = %s\n"), key.c_str());
			TRACE(msg);

			SmChartDataItem& cur_data = it->second;

			if (data.o == 0) {
				data.o = cur_data.o;
			}
			if (data.h == 0) {
				data.h = cur_data.h;
			}
			else {
				if (cur_data.h > data.h) {
					data.h = cur_data.h;
				}
			}
			if (data.l == 0) {
				data.l = cur_data.l;
			}
			else {
				if (cur_data.l < data.l) {
					data.l = cur_data.l;
				}
			}
			data.c = cur_data.c;
		}



		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->FindChartData(data.GetDataKey());
		if (chart_data) {
			chart_data->AddChartData(std::move(data));

			std::map<std::string, SmChartDataItem>& data_list = chart_data->GetDataItemList();
			for (auto it = data_list.begin(); it != data_list.end(); ++it) {
				SendCycleChartData(it->second);
			}

			CString msg;
			msg.Format(_T("MakePrevChartData ::code = %s, cycle = %d, date = %s, t = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), data.symbolCode.c_str(), data.cycle, data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
			TRACE(msg);
		}
		else {
			SendCycleChartData(data);
			//chart_data = chartDataMgr->CreateChartData(data);
			//chart_data->AddChartData(std::move(data));
			CString msg;
			msg.Format(_T("MakePrevChartData ::code = %s, cycle = %d, date = %s, t = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), data.symbolCode.c_str(), data.cycle, data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
			TRACE(msg);
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void SmSymbol::SendCycleChartData(SmChartDataItem item)
{
	json send_object;
	send_object["req_id"] = SmProtocol::req_cycle_data_resend_onebyone;
	send_object["symbol_code"] = item.symbolCode;
	send_object["chart_type"] = item.chartType;
	send_object["cycle"] = item.cycle;
	send_object["date"] = item.date;
	send_object["time"] = item.time;
	send_object["o"] = item.o;
	send_object["h"] = item.h;
	send_object["l"] = item.l;
	send_object["c"] = item.c;
	send_object["v"] = item.v;

	std::string content = send_object.dump();
	//SmSessionManager::GetInstance()->Send(content);
}

