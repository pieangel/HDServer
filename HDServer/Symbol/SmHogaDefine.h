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