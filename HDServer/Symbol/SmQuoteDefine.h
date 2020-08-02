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