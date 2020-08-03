#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
class SmSymbol;
class SmProductYearMonth;
class SmProduct
{
public:
	SmProduct();
	~SmProduct();
	std::string Code() const { return _Code; }
	void Code(std::string val) { _Code = val; }
	std::string NameKr() const { return _NameKr; }
	void NameKr(std::string val) { _NameKr = val; }
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	std::string Exchange() const { return _Exchange; }
	void Exchange(std::string val) { _Exchange = val; }
	std::string ExchangeCode() const { return _ExchangeCode; }
	void ExchangeCode(std::string val) { _ExchangeCode = val; }
	std::string ExchangeIndex() const { return _ExchangeIndex; }
	void ExchangeIndex(std::string val) { _ExchangeIndex = val; }
	std::string MarketName() const { return _MarketName; }
	void MarketName(std::string val) { _MarketName = val; }
	std::string MarketCode() const { return _MarketCode; }
	void MarketCode(std::string val) { _MarketCode = val; }
	std::shared_ptr<SmSymbol> AddSymbol(std::string symCode);
	std::vector<std::shared_ptr<SmSymbol>>& GetSymbolList() {
		return _SymbolList;
	}
	std::shared_ptr<SmSymbol> GetRecentMonthSymbol();
	std::shared_ptr<SmSymbol> GetNextMonthSymbol();

	std::shared_ptr<SmProductYearMonth> GetRecentYearMonth();
	std::shared_ptr<SmProductYearMonth> GetNextYearMonth();

	void AddToYearMonth(std::string symbol_code, std::shared_ptr<SmSymbol> symbol);

private:
	// ǰ���ڵ�
	std::string _Code;
	// ǰ�� �ѱ� �̸�
	std::string _NameKr;
	// ǰ�� ���� �̸�
	std::string _Name;
	// �ŷ��� �̸�
	std::string _Exchange;
	/// <summary>
	/// �ŷ��� �ڵ�
	/// 1:CME, 2:CBOT, 3:SGX, 4:LIFFE, 5:EUREX, 6:ICE, 7:OSE, 8:LME
	/// </summary>
	std::string _ExchangeCode;
	// �ŷ��� �ε��� �ڵ�
	std::string _ExchangeIndex;
	// ǰ���� ���� ���� �̸�
	std::string _MarketName;
	/// <summary>
	/// ǰ���� ���� ���� ���� �ڵ�
	/// 101 : Currency(��ȭ)                  
	/// 102 : Index(����)                     
	/// 103 : Fixed Income(�ݸ�)              
	/// 104 : Energy(������)                  
	/// 105 : Metal(�ݼ�)                     
	/// 106 : Agricultural(��깰)            
	/// 107 : Dairy & Beef(���)              
	/// </summary>
	std::string _MarketCode;
	std::vector<std::shared_ptr<SmSymbol>> _SymbolList;
	std::map<std::string, std::shared_ptr<SmProductYearMonth>> _YearMonthMap;

	std::map<std::string, std::string> _DomesticYearTable;
	std::map<std::string, std::string> _DomesticMonthTable;
	std::map<std::string, std::string> _AbroadMonthTable;
	int dow(int y, int m, int d);
};

