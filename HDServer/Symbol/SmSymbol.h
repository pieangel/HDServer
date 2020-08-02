#pragma once
#include <string>
#include <list>
#include "SmQuoteDefine.h"
#include "SmHogaDefine.h"

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
private:
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
};

