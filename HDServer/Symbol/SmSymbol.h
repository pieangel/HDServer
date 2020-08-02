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
	// ǰ�񿡼� �����ϴ� �ε���
	int _Index = 0;
	/// <summary>
	/// ���� �ڵ� - ������ �����ϴ� Ű�� �ȴ�.
	/// </summary>
	std::string _SymbolCode;
	/// <summary>
	/// ���� �̸�
	/// </summary>
	std::string _Name;
	/// <summary>
	/// ���� ���� �̸�
	/// </summary>
	std::string _NameEn;
	// �Ҽ��� �ڸ���
	int _Decimal;
	// �ŷ� �¼�
	int _Seungsu;
	// ������
	double _CtrUnit;
	// ƽ ��ġ  = ȣ�� ���� * ������
	double _TickValue;
	// ƽ ũ�� - ȣ�� ����
	double _TickSize;
	// ī���� �ڵ�
	std::string _CategoryCode;
	// ���� �̸�
	std::string _MarketName;
	/* ATM���� 1:ATM, 2:ITM, 3:OTM					*/
	int _Atm = 0; // 
	// �ٿ���
	int _NearMonth;
	// ���� �ŷ���
	std::string _LastDate;
};

