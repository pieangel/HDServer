#pragma once
#include <string>
#include <vector>
struct SmHoga
{
	std::string SymbolCode;
	/// <summary>
	/// �ð� - �ؿܼ����� �ؿܽð�
	/// </summary>
	std::string	Time = "";
	/// <summary>
	/// ������¥
	/// </summary>
	std::string DomesticDate = "";
	/// <summary>
	/// ���� �ð�
	/// </summary>
	std::string DomesticTime = "";
	/// <summary>
	/// �ŵ���ȣ������
	/// </summary>
	int	TotSellQty = 0;
	/// <summary>
	/// �ż���ȣ������
	/// </summary>
	int	TotBuyQty = 0;
	/// <summary>
	/// �ŵ���ȣ���Ǽ�
	/// </summary>
	int	TotSellCnt = 0;
	/// <summary>
	/// �ż���ȣ���Ǽ�
	/// </summary>
	int	TotBuyCnt = 0;
	/// <summary>
	/// ȣ�� ������
	/// </summary>
	struct	SmHogaItem
	{
		/// <summary>
		/// �ż�ȣ���Ǽ�
		/// </summary>
		int	BuyCnt = 0;
		/// <summary>
		/// �ż� ȣ��
		/// </summary>
		int	BuyPrice = 0;
		/// <summary>
		/// �ż�ȣ������
		/// </summary>
		int	BuyQty = 0;
		/// <summary>
		/// �ŵ�ȣ���Ǽ�
		/// </summary>
		int	SellCnt = 0;
		/// <summary>
		/// �ŵ�ȣ��
		/// </summary>
		int	SellPrice = 0;
		/// <summary>
		/// �ŵ�ȣ������
		/// </summary>
		int	SellQty = 0;
	}	Ary[5];
};


struct SmHogaItem {
	int command = 1; // ó�� ��� 1 : ó��, 0 : ����
	std::string symbol_code; // �ɺ��ڵ�
	std::string time; // �ð�
	std::string buy_price1; // �ż� ����
	std::string sell_price1; // �ŵ� ����
	std::string buy_qty1; // �ż� ����
	std::string sell_qty1; // �ŵ� ����
	std::string buy_count1; // �ż� �Ǽ�
	std::string sell_count1; // �ŵ� �Ǽ�

	std::string buy_price2; // �ż� ����
	std::string sell_price2; // �ŵ� ����
	std::string buy_qty2; // �ż� ����
	std::string sell_qty2; // �ŵ� ����
	std::string buy_count2; // �ż� �Ǽ�
	std::string sell_count2; // �ŵ� �Ǽ�

	std::string buy_price3; // �ż� ����
	std::string sell_price3; // �ŵ� ����
	std::string buy_qty3; // �ż� ����
	std::string sell_qty3; // �ŵ� ����
	std::string buy_count3; // �ż� �Ǽ�
	std::string sell_count3; // �ŵ� �Ǽ�

	std::string buy_price4; // �ż� ����
	std::string sell_price4; // �ŵ� ����
	std::string buy_qty4; // �ż� ����
	std::string sell_qty4; // �ŵ� ����
	std::string buy_count4; // �ż� �Ǽ�
	std::string sell_count4; // �ŵ� �Ǽ�

	std::string buy_price5; // �ż� ����
	std::string sell_price5; // �ŵ� ����
	std::string buy_qty5; // �ż� ����
	std::string sell_qty5; // �ŵ� ����
	std::string buy_count5; // �ż� �Ǽ�
	std::string sell_count5; // �ŵ� �Ǽ�

	std::string tot_buy_qty; // �Ѹż� ȣ�� ����
	std::string tot_sell_qty; // �Ѹŵ� ȣ�� ����
	std::string tot_buy_count; // �Ѹż� ȣ�� �Ǽ�
	std::string tot_sell_count; // �Ѹŵ� ȣ�� �Ǽ�
};