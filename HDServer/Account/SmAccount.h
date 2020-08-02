#pragma once
#include <string>
#include <map>
#include <vector>
#include "../Position/SmFee.h"
struct SmPosition;
struct SmOrder;
struct SmFee;
class SmAccount
{
private:
	std::string _AccountNo;
	std::string _AccountName;
	std::string _Password;

	// ����� ���̵� - �ݵ�� �̸��Ϸ� �Ѵ�.
	std::string _UserID;
	// �����ڻ��� 1���
	double _InitialBalance = 100000000;
	// �Ÿŷ� �̷���� ���� - ���� �Ÿŷ� ���� ����
	double _TradePL = 0.0;
	// ���� û����� ���� �ֹ����� ���� �� ����
	double _OpenPL = 0.0;
	// ������ �Ÿ� ���� - �ʱ� �ں��ݿ��� ���۵� ���� ��� ������ ���� ����
	double _TotalTradePL = 0.0;
	// ������� �ΰ��� Ƚ���� ����Ѵ�. �������� ������� ������ ������ ������ �����ȴ�.
	int _FeeCount; //		������
	double _Fee;
	typedef std::vector<std::shared_ptr<SmFee>> SmFeeVector;
	// key : symbol code, value : SmFeeVector
	std::map<std::string, SmFeeVector> SymbolFeeMap;
	// ���� ���� :: 0 : �ؿ�, 1 : ����
	int _AccountType = 0;
public:
	void UpdateFee(int fee_count) {
		_FeeCount += fee_count;
	}
	// �Ÿ� ������ ������Ʈ �Ѵ�.
	void UpdateTradePL(double trade_pl) {
		// ���� ��¥ ���� �Ÿ� ����
		_TradePL += trade_pl;
		// ��¥�� ������� ���� �Ÿ� ����
		_TotalTradePL += trade_pl;
		// �Ÿ� ������ ������ ���� ��Ź��
		_InitialBalance += trade_pl;
	}
	// ���¸� ������ �ʱ�ȭ ��Ų��. ���� �Ÿſ� �Ҵ�� �ݾ��� �ٽ� �����Ѵ�.
	void Reset();
	std::string GetAccountInfoByJSon();
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
	std::string AccountName() const { return _AccountName; }
	void AccountName(std::string val) { _AccountName = val; }
	std::string UserID() const { return _UserID; }
	void UserID(std::string val) { _UserID = val; }
	std::string Password() const { return _Password; }
	void Password(std::string val) { _Password = val; }
	double InitialBalance() const { return _InitialBalance; }
	void InitialBalance(double val) { _InitialBalance = val; }
	double TradePL() const { return _TradePL; }
	void TradePL(double val) { _TradePL = val; }
	double OpenPL() const { return _OpenPL; }
	void OpenPL(double val) { _OpenPL = val; }
	double TotalTradePL() const { return _TotalTradePL; }
	void TotalTradePL(double val) { _TotalTradePL = val; }
	int AccountType() const { return _AccountType; }
	void AccountType(int val) { _AccountType = val; }
	int FeeCount() const { return _FeeCount; }
	void FeeCount(int val) { _FeeCount = val; }
	double Fee() const { return _Fee; }
	void Fee(double val) { _Fee = val; }
};

