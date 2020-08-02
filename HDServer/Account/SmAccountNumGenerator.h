#pragma once
#include <string>
class SmAccountNumGenerator
{
public:
	// type : 0 -> �ؿ� ����, 1 -> ���� ����
	std::string GetNewAccountNumber(int type = 0);
	void SaveAccountNumber(int type = 0);
private:
	int _First = 100;
	int _Second = 1000;
	int _Last = 1000;
	int _Level = 0;
};

