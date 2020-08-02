#pragma once
#include <string>
struct SmTrade {
	std::string date_time;
	std::string date;
	std::string time;
	std::string account_no;
	std::string symbol_code;
	int liq_count; // 수익을 발생시킨 주문의 갯수 혹은 잔고의 갯수
	double liq_value; // 수익을 발생시킨 가격
	double current_profit_loss; // 현재 발생한 수익
	double trade_profit_loss; // 종목에 대한 누적 수익
	double total_profit_loss; // 계좌에 대한 누적 수익
	double total_account_amount; // 현재 계좌의 총액
};