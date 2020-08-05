#include "pch.h"
#include "SmServerRequestManager.h"
#include "../HDCtrl/HdClient.h"
#include "../Task/SmCallbackManager.h"
#include <chrono>
#include "../Market/SmMarketManager.h"
#include "../Symbol/SmSymbol.h"

using namespace std::chrono;

int SmServerRequestManager::_TaskId = 0;

void SmServerRequestManager::StartTimer(int interval)
{
	int wait_seconds = 0;

	auto id = _Timer.add(seconds(wait_seconds), std::bind(&SmServerRequestManager::OnTimer, this), seconds(interval));
	// Add to the request map.
	_CycleDataReqTimerMap[id] = id;
}

void SmServerRequestManager::StopTimer(size_t id)
{

}

void SmServerRequestManager::OnTimer()
{
	ExecNext();
}

SmServerRequestManager::SmServerRequestManager()
{

}

SmServerRequestManager::~SmServerRequestManager()
{

}

void SmServerRequestManager::OnTaskComplete(int reqId)
{
	// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
	auto it = _TaskInfo.argMap.find(reqId);
	if (it == _TaskInfo.argMap.end()) {
		return;
	}
	
	SmTaskArg& arg = it->second;

	switch (arg.TaskType)
	{
	case SmTaskType::JisuFtSymbol:
		break;
	case SmTaskType::JisuOpSymbol:
		break;
	case SmTaskType::MiniFtSymbol:
		break;
	case SmTaskType::MiniOpSymbol:
		break;
	case SmTaskType::CommodityFtSymbol:
		break;
	case SmTaskType::WeeklyOpSymbol:
		break;
	case SmTaskType::EtfSymbol:
		break;
	case SmTaskType::AbroadSymbol:
		break;
	case SmTaskType::EbSymbolCode:
		break;
	case SmTaskType::EbSymbolMaster:
	{
		
	}
		break;
	case SmTaskType::EBAccountAsset: {
		
	}
		break;
	case SmTaskType::EBProductRemain:
		break;
	case SmTaskType::EBAccountOrder:
		break;
	case SmTaskType::EBAccountProfitLoss:
		break;
	default:
		break;
	}
	// ó���� ���� �����ش�.
	_TaskInfo.argMap.erase(it);
	// ���� �ִ� �۾��� �ִٸ� �����Ѵ�.
	ExecNext();
}

void SmServerRequestManager::OnTaskError(int reqId)
{
	auto it = _TaskInfo.argMap.find(reqId);
	if (it == _TaskInfo.argMap.end()) {
		return;
	}

	// ó���� ���� �����ش�.
	_TaskInfo.argMap.erase(it);
	// ���� �ִ� �۾��� �ִٸ� �����Ѵ�.
	ExecNext();
}

void SmServerRequestManager::GetAccountAsset(std::string account_no)
{
// 	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
// 	std::vector<VtAccount*> acnt_list = acntMgr->GetAccountList();
// 	for (auto it = acnt_list.begin(); it != acnt_list.end(); ++it) {
// 		VtAccount* acnt = *it;
// 		if (acnt->AccountNo.compare(account_no) == 0) {
// 			SmTaskArg arg;
// 			arg.Detail = acnt->AccountNo;
// 			arg.ArgId = SmServerRequestManager::GetId();
// 			arg.TaskType = SmTaskType::EBAccountAsset;
// 			arg.AddArg("account_no", acnt->AccountNo);
// 			arg.AddArg("password", acnt->Password);
// 
// 			_TaskInfo.argMap[arg.ArgId] = arg;
// 			break;
// 		}
// 	}
// 
// 	_TaskInfo.TaskTitle = "���� ��Ź���� �������� ���Դϴ�.";
// 	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
// 	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
// 	_TaskInfo.TaskType = SmTaskType::EBAccountAsset;
// 	ExecNext();
	//StartTimer(1);
}

void SmServerRequestManager::GetMiniJango(std::string account_no)
{
// 	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
// 	std::vector<VtAccount*> acnt_list = acntMgr->GetAccountList();
// 	for (auto it = acnt_list.begin(); it != acnt_list.end(); ++it) {
// 		VtAccount* acnt = *it;
// 		if (acnt->AccountNo.compare(account_no) == 0) {
// 			SmTaskArg arg;
// 			arg.Detail = acnt->AccountNo;
// 			arg.ArgId = SmServerRequestManager::GetId();
// 			arg.TaskType = SmTaskType::EBAccountAsset;
// 			arg.AddArg("account_no", acnt->AccountNo);
// 			arg.AddArg("password", acnt->Password);
// 
// 			_TaskInfo.argMap[arg.ArgId] = arg;
// 
// 			arg.Detail = acnt->AccountNo;
// 			arg.ArgId = SmServerRequestManager::GetId();
// 			arg.TaskType = SmTaskType::EBProductRemain;
// 			arg.AddArg("account_no", acnt->AccountNo);
// 			arg.AddArg("password", acnt->Password);
// 
// 			_TaskInfo.argMap[arg.ArgId] = arg;
// 
// 			break;
// 		}
// 	}
// 
// 	_TaskInfo.TaskTitle = "�̴��ܰ� �������� ���Դϴ�.";
// 	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
// 	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
// 	_TaskInfo.TaskType = SmTaskType::EBAccountAsset;
// 
// 	ExecNext();
}

void SmServerRequestManager::GetSymbolMaster(std::string symbol_code)
{
// 	SmTaskArg arg;
// 	arg.Desc = "���簡�� �������� ���Դϴ�.";
// 	arg.Detail = symbol_code;
// 	arg.ArgId = SmServerRequestManager::GetId();
// 	arg.TaskType = SmTaskType::EbSymbolMaster;
// 	arg.AddArg("symbol_code", symbol_code);
// 	_TaskInfo.argMap[arg.ArgId] = arg;
// 
// 	_TaskInfo.TaskTitle = "���簡�� �������� ���Դϴ�.";
// 	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
// 	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
// 	_TaskInfo.TaskType = SmTaskType::EbSymbolMaster;
// 
// 	ExecNext();
}

void SmServerRequestManager::GetRecentChartData()
{
	SmTaskArg arg;

	std::vector<int> cycle_vector;
	cycle_vector.push_back(1);
	cycle_vector.push_back(5);
	cycle_vector.push_back(10);
	cycle_vector.push_back(15);
	cycle_vector.push_back(30);
	cycle_vector.push_back(60);
	std::vector<std::shared_ptr<SmSymbol>> symbol_vector = SmMarketManager::GetInstance()->GetRecentMonthSymbolList();


	for (size_t i = 0; i < symbol_vector.size(); i++) {
		arg.Desc = "��Ʈ�����͸� �������� ���Դϴ�.";
		arg.Detail = symbol_vector[i]->SymbolCode();
		arg.ArgId = SmServerRequestManager::GetId();
		arg.TaskType = SmTaskType::HdSymbolChartData;
		arg.AddArg("symbol_code", symbol_vector[i]->SymbolCode());
		arg.AddArg("chart_type", std::to_string((int)SmChartType::TICK));
		arg.AddArg("chart_cycle", std::to_string(300));
		arg.AddArg("count", std::to_string(1500));
		arg.AddArg("next", "0");
		arg.AddArg("abroad", "true");
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	for (size_t i = 0; i < symbol_vector.size(); i++) {
		arg.Desc = "��Ʈ�����͸� �������� ���Դϴ�.";
		arg.Detail = symbol_vector[i]->SymbolCode();
		arg.ArgId = SmServerRequestManager::GetId();
		arg.TaskType = SmTaskType::HdSymbolChartData;
		arg.AddArg("symbol_code", symbol_vector[i]->SymbolCode());
		arg.AddArg("chart_type", std::to_string((int)SmChartType::TICK));
		arg.AddArg("chart_cycle", std::to_string(120));
		arg.AddArg("count", std::to_string(1500));
		arg.AddArg("next", "0");
		arg.AddArg("abroad", "true");
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	for (size_t i = 0; i < symbol_vector.size(); i++) {
		for (size_t j = 0; j < cycle_vector.size(); j++) {
			arg.Desc = "��Ʈ�����͸� �������� ���Դϴ�.";
			arg.Detail = symbol_vector[i]->SymbolCode();
			arg.ArgId = SmServerRequestManager::GetId();
			arg.TaskType = SmTaskType::HdSymbolChartData;
			arg.AddArg("symbol_code", symbol_vector[i]->SymbolCode());
			arg.AddArg("chart_type", std::to_string((int)SmChartType::MIN));
			arg.AddArg("chart_cycle", std::to_string(cycle_vector[j]));
			arg.AddArg("count", std::to_string(1500));
			arg.AddArg("next", "0");
			arg.AddArg("abroad", "true");
			_TaskInfo.argMap[arg.ArgId] = arg;
		}
	}

	for (size_t i = 0; i < symbol_vector.size(); i++) {
		arg.Desc = "��Ʈ�����͸� �������� ���Դϴ�.";
		arg.Detail = symbol_vector[i]->SymbolCode();
		arg.ArgId = SmServerRequestManager::GetId();
		arg.TaskType = SmTaskType::HdSymbolChartData;
		arg.AddArg("symbol_code", symbol_vector[i]->SymbolCode());
		arg.AddArg("chart_type", std::to_string((int)SmChartType::DAY));
		arg.AddArg("chart_cycle", std::to_string(1));
		arg.AddArg("count", std::to_string(100));
		arg.AddArg("next", "0");
		arg.AddArg("abroad", "true");
		_TaskInfo.argMap[arg.ArgId] = arg;
	}
	ExecNext();
}

void SmServerRequestManager::ExecNext()
{
	if (_TaskInfo.argMap.size() == 0)
		return;
	// ���� ������ ��Ҹ� �����´�.
	auto front = _TaskInfo.argMap.begin();
	SmTaskArg& arg = front->second;
	_TaskInfo.TaskDetail = arg.Detail;
	// ��û���� �ʾҴٸ� ��û�� �Ѵ�.
	if (!arg.Requested) {
		ExecRequest(arg);
	}
}

void SmServerRequestManager::ExecRequest(SmTaskArg& arg)
{
	HdClient* client = HdClient::GetInstance();
	arg.Requested = true;
	switch (arg.TaskType)
	{
	case SmTaskType::HdSymbolMaster:
		client->GetAbSymbolMaster(arg);
		break;
	case SmTaskType::HdSymbolSise:
		client->GetAbSymbolSise(arg);
		break;
	case SmTaskType::HdSymbolHoga:
		client->GetAbSymbolHoga(arg);
		break;
	case SmTaskType::HdAccountAsset:
		client->GetAbAsset(arg);
		break;
	case SmTaskType::HdAccountProfitLoss:
		client->GetAbAccountProfitLoss(arg);
		break;
	case SmTaskType::HdAccountSymbolRemain:
		client->GetAbOutStanding(arg);
		break;
	case SmTaskType::HdAcceptedOrderList:
		client->GetAbAccepted(arg);
		break;
	case SmTaskType::HdSymbolChartData:
		client->GetAbChartData(arg);
		break;
	case SmTaskType::HdDmSymbolCodeList:
		client->GetDmSymbolCodeList(arg);
		break;
	case SmTaskType::HdDmSymbolMaster:
		client->GetDmSymbolMaster(arg);
		break;
	default:
		break;
	}
}
