#pragma once
#include <map>
#include <string>
#include "../pch.h"

enum class SmAppState
{
	// 초기 상태
	None = 0, 
	// 지수 선물 심볼 받음
	JisuFtSymbolReceived,
	// 지수 옵션 심볼 받음
	JisuOpSymbolReceived,
	// 상품 선물 심볼 받음
	CommodityFtSymbolReceived,
	// 미니 코스피 선물 심볼 받음
	MiniFtSymbolReceived,
	// 미니 코스피 선물 옵션 심볼 받음
	MinitOpSymbolReceived,
	// EFT 심볼 받음
	ETFSymbolReceived,
	/// <summary>
	/// 위클리 옵션 심볼 받음
	/// </summary>
	WeeklyOpSymbolReceived,
	// 해외 선물 심볼 목록 받음
	AbroadSymbolReceived
};

enum class SmMarketType
{
	None = 0,
	Stock, // 일반 주식
	Etf, // ETF
	StockFuture, // 주식 선물
	DomesticFuture, // 국내 선물
	DomesticOption, // 국내 옵션
	AbroadFuture, // 해외 선물
	AbroadOption // 해외 옵션
};


enum class SmTaskType
{
	// 초기 상태
	None = 0,
	// 지수 선물 심볼 
	JisuFtSymbol,
	// 지수 옵션 심볼 
	JisuOpSymbol,
	// 상품 선물 심볼 
	CommodityFtSymbol,
	// 미니 코스피 선물 심볼 
	MiniFtSymbol,
	// 미니 코스피 선물 옵션 심볼 
	MiniOpSymbol,
	// EFT 심볼 받음
	EtfSymbol,
	/// <summary>
	/// 위클리 옵션 심볼 받음
	/// </summary>
	WeeklyOpSymbol,
	// 해외 선물 심볼 목록 받음
	AbroadSymbol,
	EbSymbolCode, // 심볼 코드
	EbSymbolMaster, // 심볼 마스터
	EBAccountAsset, // 예탁 잔고
	EBAccountProfitLoss, // 계좌별 손익
	EBProductRemain, // 종목별 잔고와 평가 손익
	EBAccountOrder, // 계좌별 주문 내역
	HdSymbolMaster, // 해외 종목 정보
	HdSymbolSise, // 해외 종목 시세
	HdSymbolHoga, // 해외 종목 호가
	HdSymbolChartData, // 해외 종목 차트 데이터
	HdAccountAsset, // 해외 계좌 
	HdAccountProfitLoss, // 해외 계좌 손익
	HdAccountSymbolRemain, // 해외 종목 잔고
	HdAcceptedOrderList, // 해외 미체결 주문
	HdDmSymbolCodeList, // 국내 심볼 코드 리스트
	HdDmSymbolMaster // 국내 심볼 마스터
};

struct SmTaskArg {
	// 요청 아이디
	int ArgId = 0;
	// 요청했는지 여부
	bool Requested = false;
	// 받았는지 여부
	bool Received = false;
	SmTaskType TaskType = SmTaskType::None;
	// 작업 설명
	std::string Desc;
	// 작업 내용
	std::string Detail;
	// 매개변수 맵
	std::map<std::string, std::string> ArgMap;

	void AddArg(std::string key, std::string value) {
		ArgMap[key] = value;
	}

	std::string GetArg(std::string key) {
		std::string value;
		auto it = ArgMap.find(key);
		if (it != ArgMap.end()) {
			value = it->second;
		}

		return value;
	}
};

struct SmTaskInfo {
	SmTaskType TaskType;
	std::string TaskTitle;
	std::string TaskDetail;
	int TotalTaskCount;
	int RemainTaskCount;
	std::map<int, SmTaskArg> argMap;
};


#define WMU_SHOW_TR		( WM_USER + 1000 )
#define WMU_SET_MESSAGE	( WM_USER + 1001 )

enum
{
	DATA_TYPE_STRING = 0,		// 문자열
	DATA_TYPE_LONG,				// 정수
	DATA_TYPE_FLOAT,			// 실수
	DATA_TYPE_FLOAT_DOT,		// 실수( 소숫점을 가지고 있음 )
	DATA_TYPY_FLOAT_TO_LONG     // 실수에서 소수점을 제거한 후에 정수로 바꾼 수
};
#define DATA_TYPE_DOUBLE	DATA_TYPE_FLOAT
#define DATA_TYPE_DOT		DATA_TYPE_FLOAT_DOT



