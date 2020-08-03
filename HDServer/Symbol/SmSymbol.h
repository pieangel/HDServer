#pragma once
#include <string>
#include <list>
#include <mutex>
#include <map>
#include "SmQuoteDefine.h"
#include "SmHogaDefine.h"
#include "../Chart/SmChartDefine.h"
class SmChartData;
class SmSymbol
{
public:
	SmSymbol();
	~SmSymbol();
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	std::string NameEn() const { return _NameEn; }
	void NameEn(std::string val) { _NameEn = val; }
	SmQuote Quote;
	SmHoga  Hoga;
	std::string GetQuoteByJson();
	std::string GetHogaByJson();
	std::string GetQuoteByJson(int total, int current);
	static std::string GetDummyQuoteByJson(int total, int current);
	std::string GetHogaByJson(int total, int current);
	static std::string GetDummyHogaByJson(int total, int current);
	int Decimal() const { return _Decimal; }
	void Decimal(int val) { _Decimal = val; }
	int Seungsu() const { return _Seungsu; }
	void Seungsu(int val) { _Seungsu = val; }
	std::string CategoryCode() const { return _CategoryCode; }
	void CategoryCode(std::string val) { _CategoryCode = val; }
	std::string MarketName() const { return _MarketName; }
	void MarketName(std::string val) { _MarketName = val; }
	double CtrUnit() const { return _CtrUnit; }
	void CtrUnit(double val) { _CtrUnit = val; }
	double TickValue() const { return _TickValue; }
	void TickValue(double val) { _TickValue = val; }
	double TickSize() const { return _TickSize; }
	void TickSize(double val) { _TickSize = val; }
	int Index() const { return _Index; }
	void Index(int val) { _Index = val; }
	int Atm() const { return _Atm; }
	void Atm(int val) { _Atm = val; }
	int NearMonth() const { return _NearMonth; }
	void NearMonth(int val) { _NearMonth = val; }
	std::string LastDate() const { return _LastDate; }
	void LastDate(std::string val) { _LastDate = val; }

	int StartHour() const { return _StartHour; }
	void StartHour(int val) { _StartHour = val; }
	int StartMin() const { return _StartMin; }
	void StartMin(int val) { _StartMin = val; }
	int StartSecond() const { return _StartSecond; }
	void StartSecond(int val) { _StartSecond = val; }
	int EndHour() const { return _EndHour; }
	void EndHour(int val) { _EndHour = val; }
	int EndMin() const { return _EndMin; }
	void EndMin(int val) { _EndMin = val; }
	int EndSecond() const { return _EndSecond; }
	void EndSecond(int val) { _EndSecond = val; }
	int AccAmount() const { return _AccAmount; }
	void AccAmount(int val) { _AccAmount = val; }

	void MakeCurrChartDataByTimer(std::string cur_hour_min);
	void MakePrevChartDataByTimer(std::string cur_hour_min);

	void UpdateMinData(SmQuoteData tick_data);
	void UpdateTickData(SmQuoteData tick_data);

	void addChartData(std::string dataKey, std::shared_ptr<SmChartData> chart_data) {
		_ChartDataMap[dataKey] = chart_data;
	}

	void removeChartData(std::string dataKey) {
		auto it = _ChartDataMap.find(dataKey);
		if (it != _ChartDataMap.end()) {
			_ChartDataMap.erase(it);
		}
	}

	// 실시간 틱 데이터로 차트데이터를 업데이트 한다.
	void UpdateChartData(SmQuoteData tick_data);

private:
	std::mutex _mutex;
	// 누적 거래량
	int _AccAmount;

	// 품목에서 차지하는 인덱스
	int _Index = 0;
	/// <summary>
	/// 종목 코드 - 종목을 구분하는 키가 된다.
	/// </summary>
	std::string _SymbolCode;
	/// <summary>
	/// 종목 이름
	/// </summary>
	std::string _Name;
	/// <summary>
	/// 영문 종목 이름
	/// </summary>
	std::string _NameEn;
	// 소수점 자리수
	int _Decimal;
	// 거래 승수
	int _Seungsu;
	// 계약단위
	double _CtrUnit;
	// 틱 가치  = 호가 단위 * 계약단위
	double _TickValue;
	// 틱 크기 - 호가 단위
	double _TickSize;
	// 카데고리 코드
	std::string _CategoryCode;
	// 시장 이름
	std::string _MarketName;
	/* ATM구분 1:ATM, 2:ITM, 3:OTM					*/
	int _Atm = 0; // 
	// 근월물
	int _NearMonth;
	// 최종 거래일
	std::string _LastDate;

	int _StartHour = 9;
	int _StartMin = 0;
	int _StartSecond = 0;

	int _EndHour = 15;
	int _EndMin = 45;
	int _EndSecond = 0;

	// 분데이터 맵 키 : 시간과분의 4자리 키, 값은 시,고,저,종, 거래량이 담겨있는 차트 데이터
	std::map<std::string, SmChartDataItem> _MinDataMap;

	// 120틱 카운트
	int tick_count_120 = -1;
	int tick_count_300 = -1;
	std::string last_tick_time_120 = "";
	std::string last_tick_time_300 = "";

	void MakePrevChartData(int total_min_diff, std::string symbol_code, std::string prev_hour_min, std::string chart_hour_min, int cycle);
	// 사이클 데이터를 서버로 전송한다.
	void SendCycleChartData(SmChartDataItem item);

	std::map<std::string, std::shared_ptr<SmChartData>> _ChartDataMap;
};

