#pragma once
#include <string>
#include <list>

struct SmQuoteItem {
	std::string	Time;	/* �ð�(HH:MM:SS)												*/
	int	ClosePrice;	/* ���簡														*/
	int	ContQty;	/* ü�ᷮ														*/
	int	MatchKind;	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	int Decimal;
	std::string	Date;
};

struct	SmQuote
{
	std::string SymbolCode;
	/// <summary>
	/// ��
	/// </summary>
	int High = 0;
	/// <summary>
	/// ����
	/// </summary>
	int Low = 0;
	/// <summary>
	/// ����
	/// </summary>
	int Close = 0;
	/// <summary>
	/// �ð�
	/// </summary>
	int Open = 0;
	/// <summary>
	/// ��������
	/// </summary>
	int PreClose = 0;
	/// <summary>
	/// ����
	/// </summary>
	int Expected = 0;
	/// <summary>
	/// ���ϰ�
	/// </summary>
	int PreDayHigh = 0;
	/// <summary>
	/// ��������
	/// </summary>
	int PreDayLow = 0;
	/// <summary>
	/// ���Ͻð�
	/// </summary>
	int PreDayOpen = 0;
	/// <summary>
	/// ���ϴ�񱸺� : + (���), -(�϶�)
	/// </summary>
	std::string SignToPreDay = "+";
	/// <summary>
	/// ���ϴ�� ��°�
	/// </summary>
	int GapFromPreDay = 0;
	/// <summary>
	/// ���ϴ������
	/// </summary>
	std::string RatioToPreday = "0.0";
	/// <summary>
	/// �ü�ȣ��ť
	/// </summary>
	std::list<SmQuoteItem> QuoteItemQ;
	/// <summary>
	/// �ü����������ð�
	/// </summary>
	std::string OriginTime;
	/// <summary>
	/// ü�ᷮ
	/// </summary>
	int Volume;
	std::string Sign;
	// �����ŷ���
	int accVolume;
};