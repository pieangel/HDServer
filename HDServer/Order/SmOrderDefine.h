#pragma once
#include <string>

enum class SmOrderError {
	None,
	BadAccount,
	BadSymbol,
	BadPriceType,
};

enum class SmPositionType
{
	None = 0,
	Buy,
	Sell,
	ExitBuy,
	ExitSell
};

enum class SmOrderType
{
	None = 0,
	New,
	Modify,
	Cancel
};

enum class SmPriceType
{
	None = 0,
	Price, // ������
	Market, // ���尡
	Condition, // ����
	BestPrice // �ֻ��� ������
};

//1:FAS, 2:FOK, 3:FAK
enum class SmFilledCondition
{
	None = 0,
	Fas = 1, //
	Fok = 2, //
	Fak = 3
};

enum class SmOrderState
{
	None,
	/// <summary>
	/// ��������
	/// </summary>
	Ledger,
	/// �ű� Ȯ��
	ConfirmNew,
	/// <summary>
	/// ���� Ȯ��
	/// </summary>
	ConfirmModify,
	/// <summary>
	/// ��� Ȯ��
	/// </summary>
	ConfirmCancel,
	/// <summary>
	/// �ֹ�����
	/// </summary>
	Accepted,
	/// <summary>
	/// ü��
	/// </summary>
	Filled,
	/// <summary>
	/// �ű��ֹ��ź�
	/// </summary>
	RejectNew,
	/// <summary>
	/// �����ź�
	/// </summary>
	RejectModify,
	/// <summary>
	/// ��� �ź�
	/// </summary>
	RejectCancel,
	// û��� - �ٸ� �ֹ��� ���ؼ� Ȥ�� �ܰ� û���ϴ� �뵵��
	Settled
};

enum class SmOrderCode {
	None = 100,
	NotFountSymbol,
	UnknownPriceType,
	OrderCreated,
	OrderAccepted,
	OrderFilled,
	PositionCalculated
};


struct SmOrderRequest
{
	// �ֹ��� �� ������� ���̵�
	std::string UserID;
	std::string AccountNo;
	std::string Password;
	std::string SymbolCode;
	//�Ÿű���01 1: �ż�, 2 : �ŵ�
	SmPositionType Position;
	//��������011 : ������, 2 : ���尡
	SmPriceType PriceType;
	//ü������011 : FAS, 2 : FOK, 3 : FAK
	SmFilledCondition FillCondition;
	int OrderPrice;
	//�ֹ�����05�ֹ� ����(ex:"1    ")
	int OrderAmount;
	// �����̳� ��ҽ� �� �ֹ� ��ȣ
	int OriOrderNo = -1;
	// �ֹ���û ���̵�
	int RequestID = 0;
	/// <summary>
	/// �ֹ� Ÿ�� : �ű�, ����, ���
	/// </summary>
	SmOrderType OrderType;
	// Fund Name
	std::string FundName = "";
	// ���� �̸�
	std::string StrategyName = "";
	// �θ� ���� ��ȣ
	std::string ParentAccountNo;
	std::string SystemName;
};
