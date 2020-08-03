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

struct SmQuoteData {
	int command = 1; // ó�� ��� 1 : ó��, 0 : ����
	std::string symbol_code; // �ɺ��ڵ�
	std::string time; // �ð�
	std::string volume; // ����
	std::string up_down; // ü�� ����
	std::string close; // ���簡
	std::string open; // �ð�
	std::string high; // ��
	std::string low; // ����
	std::string to_preday; // ���ϴ��
	std::string up_down_rate; // �����
	std::string sign;
	std::string acc_vol;
	std::string local_date;
};

struct SmTickData {
	std::string symbol_code;
	int command = 1;
	std::string hourmin;
	std::string tick_time;
	std::string local_date;
	int close = 0;
	int volume = 0;
};