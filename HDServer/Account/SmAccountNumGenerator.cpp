#include "pch.h"
#include "SmAccountNumGenerator.h"
#include "../Database/SmMongoDBManager.h"
#include <tuple>

std::string SmAccountNumGenerator::GetNewAccountNumber(int type)
{
	std::string first, second, last, whole;

	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	
	auto acc_no = mongoMgr->GetAccountNo(type);
	_First = std::get<0>(acc_no);
	_Second = std::get<1>(acc_no);
	_Last = std::get<2>(acc_no);
	if (_Last < 9999)
		_Level = 0;
	else if (_Second < 9999)
		_Level = 1;
	else
		_Level = 2;
	if (_Level == 0)
		_Last++;
	else if (_Level == 1)
		_Second++;
	else
		_First++;

	first = std::to_string(_First);
	second = std::to_string(_Second);
	last = std::to_string(_Last);
	whole = first + second + last;

	SaveAccountNumber(type);

	return whole;
}

void SmAccountNumGenerator::SaveAccountNumber(int type)
{
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SaveAccountNo(type, _First, _Second, _Last);
}
