#pragma once
#include <string>
struct HdProductInfo
{
	// ǰ�� �ڵ�
	std::string code;
	// �Ҽ���
	int decimal;
	// ���� ƽũ��
	int intTickSize;
	// ƽũ�� - ���ڿ�
	std::string tickSize;
	// ƽ��ġ - ����
	int tickValue;
	// �¼�
	int tradeWin;
	// ȣ�� ���� ����
	int hogaUnit;
	// ǰ�� �̸�
	std::string name;
};