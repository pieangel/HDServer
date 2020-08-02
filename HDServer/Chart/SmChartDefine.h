#pragma once
#include <string>
const size_t ChartDataSize = 1500;
enum class SmChartType
{
	NONE = 0,
	TICK,
	MIN,
	DAY,
	WEEK,
	MON,
	YEAR
};

// 지표 타입
enum class SmIndicatorType
{
	SimpleMa, // 단순 이동 평균
	DataVolume, // 
	BollingerBand, // 볼린저 밴드
	PriceChannel, // 가격 채널
	Macd, // macd
	Rsi, // rsi
	Stochastic, // 스토캐스틱
	Trix, // 트릭스
	Atr, // ATR
	Sar, // SAR
	Channel, // 고가 저가 채널
	CompositeMa // 4개의 이동평균
};

enum class SmChartDataReqestType {
	FIRST = 0, // 최초 요구
	CYCLE // 주기 데이터 요구
};

struct SmSiseDataRequest {
	std::string user_id;
	std::string symbol_code;
};

struct SmHogaDataRequest {
	std::string user_id;
	std::string symbol_code;
};

struct SmSymbolMasterRequest {
	std::string user_id;
	std::string symbol_code;
};

struct SmChartDataRequest
{
	SmChartDataReqestType reqType = SmChartDataReqestType::FIRST;
	int command = 1;
	// Socket id
	int session_id;
	std::string product_code;
	std::string chart_id;
	std::string user_id;
	std::string symbolCode;
	SmChartType chartType;
	int cycle = 0;
	int count = 0;
	int next = 0;
	int reqKey = 0;
	// 갭보정 0 : 사용안함, 1 : 사용함
	int seq = 0;
	bool domestic = false;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}

	std::string GetDailyKey() {
		std::string key = product_code;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmIndicatorRequest
{
	int command = 1;
	// Socket id
	int session_id;
	std::string product_code;
	std::string user_id;
	std::string symbolCode;
	SmChartType chartType;
	SmIndicatorType indicatorType;
	int cycle = 0;
	int count = 0;
	bool domestic = false;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}

	std::string GetDailyKey() {
		std::string key = product_code;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmCorrelationRequest
{
	int command = 1;
	// Socket id
	int session_id;
	std::string symbol_code_row;
	std::string symbol_code_col;
	std::string user_id;
	std::string symbolCode;
	SmChartType chartType;
	SmIndicatorType indicatorType;
	int cycle = 0;
	int count = 0;
	bool domestic = false;
	std::string GetDataKey(std::string product_code) {
		std::string key = product_code;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmChartDataItem {
	std::string symbolCode;
	SmChartType chartType;
	int cycle = 0;
	int total_count = 0;
	int current_count = 0;
	std::string date;
	std::string time;
	std::string date_time;
	int o = 0;
	int h = 0;
	int l = 0;
	int c = 0;
	int v = 0;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmSimpleChartDataItem {
	std::string date_time;
	int o = 0;
	int h = 0;
	int l = 0;
	int c = 0;
	int v = 0;
};

