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
	// 마지막 주문번호에서 하나증가 시킨 값이 주문의 시작번호이다.
	_ID = last_order_no + 1;
}
