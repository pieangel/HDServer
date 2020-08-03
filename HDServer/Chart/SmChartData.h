#pragma once
#include <string>
#include "SmChartDefine.h"
#include "../Symbol/SmQuoteDefine.h"
#include <vector>
#include <set>
#include <list>
#include <map>
#include <array>
class SmChartData
{
private:
	// ��Ʈ�����͸� �̹� �޾����� ǥ���Ѵ�.
	bool _Received = false;
	std::string _SymbolCode;
	SmChartType _ChartType = SmChartType::MIN;
	int _Cycle = 0;
	// �ֽŵ����Ͱ� �� �տ� �´�. 
	// ���� �ε��� 0�� ���� �ֽ� �������̴�.
	std::list<SmChartDataItem> _DataItemList;
	std::map<std::string, SmChartDataItem> _DataMap;
	// ��Ʈ�� ��û�ϴ� ����� ���̵� ���
	std::set<std::string> _UserList;
	void GetChartDataFromDB();
	void GetChartDataFromServer();
	void GetCyclicDataFromServer();
	size_t _DataQueueSize = ChartDataSize;
	size_t _CycleDataSize = 3;
	// ��ϵ� ����ڵ鿡�� ��Ʈ ���� �����͸� �����ش�.
	void SendCyclicChartDataToUsers();
public:
	std::vector<double> GetClose();
	std::vector<double> GetOpen();
	std::vector<double> GetHigh();
	std::vector<double> GetLow();
	std::vector<double> GetVolume();
	std::map<std::string, SmChartDataItem>& GetDataMap() {
		return _DataMap;
	}
	void AddChartData(SmChartDataItem&& data);
	void AddData(SmChartDataItem& data_item);
// 	std::list<SmChartDataItem>& GetDataItemList() {
// 		return _DataItemList;
// 	}
// 	size_t GetChartDataCount() {
// 		return _DataItemList.size();
// 	}

	std::map<std::string, SmChartDataItem>& GetDataItemList() {
		return _DataMap;
	}

	size_t GetDataCount() {
		return _DataMap.size();
	}
	// ���ð�, ����Ŭ
	std::pair<int, int> GetCycleByTimeDif();
	// ��Ʈ �����Ͱ� ���� ���������� �˸���.
	void OnChartDataUpdated();
	void PushChartDataItemToBack(SmChartDataItem data);
	void PushChartDataItemToFront(SmChartDataItem data);
	void UpdateChartData(SmChartDataItem data);
	// �ǽð� ƽ �����ͷ� ��Ʈ�����͸� ������Ʈ �Ѵ�.
	void UpdateChartData(SmQuoteData tick_data);
	size_t GetUserCount() {
		return _UserList.size();
	}
	void RemoveUser(std::string user_id);
	void AddUser(std::string user_id) {
		if (user_id.length() > 0)
			_UserList.insert(user_id);
	}
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	SmChartType ChartType() const { return _ChartType; }
	void ChartType(SmChartType val) { _ChartType = val; }
	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	// Ÿ�̸Ӱ� �Ҹ��� ȣ�� �Ǵ� �Լ�
	/// <summary>
	/// Ÿ�̸Ӱ� �Ҹ� �� ����� ����� �����Ͽ� ��Ʈ �����͸� �����Ͽ� Ŭ���̾�Ʈ���� �����Ѵ�.
	/// </summary>
	void OnTimer();
	std::string GetDataKey() {
		std::string key = _SymbolCode;
		key.append(":");
		key.append(std::to_string((int)_ChartType));
		key.append(":");
		key.append(std::to_string(_Cycle));
		return key;
	}

	static std::string MakeDataKey(std::string symbol_code, int chart_type, int cycle) {
		std::string key = symbol_code;
		key.append(":");
		key.append(std::to_string(chart_type));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
	int DataQueueSize() const { return _DataQueueSize; }
	void DataQueueSize(int val) { _DataQueueSize = val; }
	size_t CycleDataSize() const { return _CycleDataSize; }
	void CycleDataSize(size_t val) { _CycleDataSize = val; }
	bool Received() const { return _Received; }
	void Received(bool val) { _Received = val; }

	SmChartDataItem* GetChartDataItem(std::string date_time);
};

