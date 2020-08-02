#pragma once
#include "SmOrderDefine.h"
#include <vector>

// ������� �ֹ��� accountNo�� �θ��� ���¹�ȣ�� ����ϰ�
// SubAccountNo �� ���� ��ȣ�� ���� �����ؼ� ����Ѵ�.
// �Ϲݰ��� �ֹ��̵� ������� �ֹ��̵�  accountNo���� ���� ������ ��ȣ�� ����ִ�.
struct SmOrder
{
	// �ֹ��� �� ������� ���̵�
	std::string UserID;
	/// <summary>
	/// �ֹ� �䱸 ���� ��ȣ
	/// �������� �ֹ���ȣ�� �ֱ�����
	/// Ŭ���̾�Ʈ ������ �ֹ��� �����ϱ� ���� ��ȣ
	/// </summary>
	int RequestID = 0;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	std::string AccountNo;
	/// <summary>
	/// �ֹ� Ÿ�� : �ű�, ����, ���
	/// </summary>
	SmOrderType OrderType;
	/// <summary>
	/// ������ Ÿ�� : �ŵ�, �ż�
	/// </summary>
	SmPositionType Position;
	/// <summary>
	/// ���� ���� : ������, ���尡, ����, �ֻ��� ������
	/// </summary>
	SmPriceType PriceType;
	/// <summary>
	/// ���� �ڵ�
	/// ��Ʈ�� ������ ���� �ڵ带 ���� ��� ��� ������Ʈ ����� �ȴ�.
	/// </summary>
	std::string SymbolCode;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	int OrderPrice = 0;
	/// <summary>
	/// �ֹ���ȣ
	/// </summary>
	int OrderNo = 0;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	int OrderAmount = 0;
	/// <summary>
	/// �� �ֹ� ��ȣ : �����̳� ��� �� �ʿ���
	/// </summary>
	int OriOrderNo = 0;
	/// <summary>
	/// �ŷ� ��¥
	/// </summary>
	std::string FilledDate;
	std::string FilledTime;
	// �ֹ��� ���� ��¥
	std::string OrderDate;
	std::string OrderTime;
	/// <summary>
	/// ü�� ����
	/// </summary>
	int FilledQty = 0;
	/// <summary>
	/// ü�� ����
	/// </summary>
	int FilledPrice = 0;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	SmOrderState OrderState;
	/// <summary>
	/// ü������ (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)	
	/// </summary>
	SmFilledCondition FilledCondition;
	// ��ǰ�� �Ҽ���
	int SymbolDecimal;
	/// <summary>
	/// ü��� �ܰ� ���� - û���� �Ǹ� 0�� �ȴ�.
	/// �ܰ� ������ �ŵ��� -�� ǥ���ϰ� �ż��� +�� ǥ���Ѵ�.
	/// + 3 : �ż� �ܰ� 3, -2 : �ŵ� �ܰ� 2
	/// </summary>
	int RemainQty = 0;
	// �θ� ���� ��ȣ
	std::string ParentAccountNo;
	// ���� �̸�
	std::string StrategyName = "";
	/// <summary>
	/// The name of the underlying fund.
	/// </summary>
	std::string FundName;
	std::string SystemName;
	/// ������ ����
	int AcceptedPrice;
	// �� �ֹ��� û�� ��Ų �ֹ� ���
	std::vector<int> SettledOrders;
};
