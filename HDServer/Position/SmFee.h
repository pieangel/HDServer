#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
struct SmFee {
	std::string Date;
	std::string Time;
	std::string AccountNo;
	std::string SymbolCode;
	int FeeCount;
};

struct SmAccountFee {
	typedef std::vector<std::shared_ptr<SmFee>> SmFeeVector;
	std::string AccountNo;
	// key : symbol code, value : SmFeeVector
	std::map<std::string, SmFeeVector> SymbolFeeMap;
	void AddFee(std::string symbol_code, std::shared_ptr<SmFee> fee) {
		auto it = SymbolFeeMap.find(symbol_code);
		if (it != SymbolFeeMap.end()) {
			SmFeeVector& fee_vec = it->second;
			fee_vec.push_back(fee);
		}
		else {
			SmFeeVector fee_vec;
			fee_vec.push_back(fee);
			SymbolFeeMap.insert(std::make_pair(symbol_code, fee_vec));
		}
	}
};