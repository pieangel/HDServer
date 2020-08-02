#pragma once
#include <string>
struct SmTrade {
	std::string date_time;
	std::string date;
	std::string time;
	std::string account_no;
	std::string symbol_code;
	int liq_count; // ������ �߻���Ų �ֹ��� ���� Ȥ�� �ܰ��� ����
	double liq_value; // ������ �߻���Ų ����
	double current_profit_loss; // ���� �߻��� ����
	double trade_profit_loss; // ���� ���� ���� ����
	double total_profit_loss; // ���¿� ���� ���� ����
	double total_account_amount; // ���� ������ �Ѿ�
};