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