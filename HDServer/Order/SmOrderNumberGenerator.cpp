#include "pch.h"
#include "SmOrderNumberGenerator.h"
#include "../Database/SmMongoDBManager.h"
int SmOrderNumberGenerator::_ID = 0;

void SmOrderNumberGenerator::SetID(int id)
{
	_ID = id;
}

void SmOrderNumberGenerator::LoadOrderNo()
{
	int last_order_no = SmMongoDBManager::GetInstance()->GetOrderNo();
	// ������ �ֹ���ȣ���� �ϳ����� ��Ų ���� �ֹ��� ���۹�ȣ�̴�.
	_ID = last_order_no + 1;
}
