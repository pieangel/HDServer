#pragma once
#include <string>
#include "../Order/SmOrderDefine.h"
struct SmPosition
{
	// �������� ������� ��¥
	std::string CreatedDate;
	/// <summary>
	/// �������� ������� �ð�
	/// </summary>
	std::string CreatedTime;
	// �ɺ� �ڵ�
	std::string SymbolCode;
	// �ݵ� �̸�
	std::string FundName;
	// ���� ��ȣ
	std::string AccountNo;
	// ������
	SmPositionType Position = SmPositionType::None; 
	// �������� �ܰ�
	int OpenQty;
	// ������� �ΰ��� Ƚ���� ����Ѵ�. �������� ������� ������ ������ ������ �����ȴ�.
	int FeeCount; //		������
	double TradePL = 0.0f; //	�Ÿż���
	double	AvgPrice = 0.0f;	/*��հ�*/
	double	CurPrice = 0.0f;	/*���簡*/
	double	OpenPL = 0.0f;		/*�򰡼���*/
	// �������� ������ ����� ���̵�
	std::string UserID;
};

