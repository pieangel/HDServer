#pragma once
#include <string>
#include <list>

struct SmQuoteItem {
	std::string	Time;	/* 시간(HH:MM:SS)												*/
	int	ClosePrice;	/* 현재가														*/
	int	ContQty;	/* 체결량														*/
	int	MatchKind;	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	int Decimal;
	std::string	Date;
};

struct	SmQuote
{
	std::string SymbolCode;
	/// <summary>
	/// 고가
	/// </summary>
	int High = 0;
	/// <summary>
	/// 저가
	/// </summary>
	int Low = 0;
	/// <summary>
	/// 종가
	/// </summary>
	int Close = 0;
	/// <summary>
	/// 시가
	/// </summary>
	int Open = 0;
	/// <summary>
	/// 전일종가
	/// </summary>
	int PreClose = 0;
	/// <summary>
	/// 예상가
	/// </summary>
	int Expected = 0;
	/// <summary>
	/// 전일고가
	/// </summary>
	int PreDayHigh = 0;
	/// <summary>
	/// 전일저가
	/// </summary>
	int PreDayLow = 0;
	/// <summary>
	/// 전일시가
	/// </summary>
	int PreDayOpen = 0;
	/// <summary>
	/// 전일대비구분 : + (상승), -(하락)
	/// </summary>
	std::string SignToPreDay = "+";
	/// <summary>
	/// 전일대비 상승값
	/// </summary>
	int GapFromPreDay = 0;
	/// <summary>
	/// 전일대비등락율
	/// </summary>
	std::string RatioToPreday = "0.0";
	/// <summary>
	/// 시세호가큐
	/// </summary>
	std::list<SmQuoteItem> QuoteItemQ;
	/// <summary>
	/// 시세수신최종시간
	/// </summary>
	std::string OriginTime;
	/// <summary>
	/// 체결량
	/// </summary>
	int Volume;
	std::string Sign;
	// 누적거래량
	int accVolume;
};

struct SmQuoteData {
	int command = 1; // 처리 명령 1 : 처리, 0 : 종료
	std::string symbol_code; // 심볼코드
	std::string time; // 시간
	std::string volume; // 수량
	std::string up_down; // 체결 구분
	std::string close; // 현재가
	std::string open; // 시가
	std::string high; // 고가
	std::string low; // 저가
	std::string to_preday; // 전일대비
	std::string up_down_rate; // 등락율
	std::string sign;
	std::string acc_vol;
	std::string local_date;
};

struct SmTickData {
	std::string symbol_code;
	int command = 1;
	std::string hourmin;
	std::string tick_time;
	std::string local_date;
	int close = 0;
	int volume = 0;
};