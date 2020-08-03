#pragma once
#include <string>
#include <vector>
struct SmHoga
{
	std::string SymbolCode;
	/// <summary>
	/// 시간 - 해외선물은 해외시간
	/// </summary>
	std::string	Time = "";
	/// <summary>
	/// 국내날짜
	/// </summary>
	std::string DomesticDate = "";
	/// <summary>
	/// 국내 시간
	/// </summary>
	std::string DomesticTime = "";
	/// <summary>
	/// 매도총호가수량
	/// </summary>
	int	TotSellQty = 0;
	/// <summary>
	/// 매수총호가수량
	/// </summary>
	int	TotBuyQty = 0;
	/// <summary>
	/// 매도총호가건수
	/// </summary>
	int	TotSellCnt = 0;
	/// <summary>
	/// 매수총호가건수
	/// </summary>
	int	TotBuyCnt = 0;
	/// <summary>
	/// 호가 아이템
	/// </summary>
	struct	SmHogaItem
	{
		/// <summary>
		/// 매수호가건수
		/// </summary>
		int	BuyCnt = 0;
		/// <summary>
		/// 매수 호가
		/// </summary>
		int	BuyPrice = 0;
		/// <summary>
		/// 매수호가수량
		/// </summary>
		int	BuyQty = 0;
		/// <summary>
		/// 매도호가건수
		/// </summary>
		int	SellCnt = 0;
		/// <summary>
		/// 매도호가
		/// </summary>
		int	SellPrice = 0;
		/// <summary>
		/// 매도호가수량
		/// </summary>
		int	SellQty = 0;
	}	Ary[5];
};


struct SmHogaItem {
	int command = 1; // 처리 명령 1 : 처리, 0 : 종료
	std::string symbol_code; // 심볼코드
	std::string time; // 시간
	std::string buy_price1; // 매수 가격
	std::string sell_price1; // 매도 가격
	std::string buy_qty1; // 매수 수량
	std::string sell_qty1; // 매도 수량
	std::string buy_count1; // 매수 건수
	std::string sell_count1; // 매도 건수

	std::string buy_price2; // 매수 가격
	std::string sell_price2; // 매도 가격
	std::string buy_qty2; // 매수 수량
	std::string sell_qty2; // 매도 수량
	std::string buy_count2; // 매수 건수
	std::string sell_count2; // 매도 건수

	std::string buy_price3; // 매수 가격
	std::string sell_price3; // 매도 가격
	std::string buy_qty3; // 매수 수량
	std::string sell_qty3; // 매도 수량
	std::string buy_count3; // 매수 건수
	std::string sell_count3; // 매도 건수

	std::string buy_price4; // 매수 가격
	std::string sell_price4; // 매도 가격
	std::string buy_qty4; // 매수 수량
	std::string sell_qty4; // 매도 수량
	std::string buy_count4; // 매수 건수
	std::string sell_count4; // 매도 건수

	std::string buy_price5; // 매수 가격
	std::string sell_price5; // 매도 가격
	std::string buy_qty5; // 매수 수량
	std::string sell_qty5; // 매도 수량
	std::string buy_count5; // 매수 건수
	std::string sell_count5; // 매도 건수

	std::string tot_buy_qty; // 총매수 호가 수량
	std::string tot_sell_qty; // 총매도 호가 수량
	std::string tot_buy_count; // 총매수 호가 건수
	std::string tot_sell_count; // 총매도 호가 건수
};