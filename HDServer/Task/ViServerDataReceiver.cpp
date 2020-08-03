#include "pch.h"
#include "ViServerDataReceiver.h"
#include <vector>
#include <set>
#include "../Market/SmMarketManager.h"
#include "VtProgressDlg.h"
//#include "../MainFrm.h"
//#include "../ViDataCollectorDlg.h"
#include "../Symbol/SmSymbol.h"
//#include "../Account/VtAccountManager.h"
//#include "../Account/VtAccount.h"
#include "../Market/SmProduct.h"
#include "../HdCtrl/HdClient.h"
#include "../MainFrm.h"
#include "../Symbol/SmRunInfo.h"

int ViServerDataReceiver::_TaskId = 0;

void ViServerDataReceiver::ExecRequest(SmTaskArg& arg)
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

void ViServerDataReceiver::SetTaskState()
{
	if (!_ProgressDlg)
		return;
	_ProgressDlg->TotalCount(_TaskInfo.TotalTaskCount);
	_ProgressDlg->RemainCount(_TaskInfo.RemainTaskCount);
	_ProgressDlg->TaskTitle(_TaskInfo.TaskTitle);
	_ProgressDlg->TaskDetail(_TaskInfo.TaskDetail);
	_ProgressDlg->RefreshProgress();
}

void ViServerDataReceiver::DoNextGroupTask()
{
	// ���� ��û ����� ���� ��� ���ش�.
	_TaskInfo.argMap.clear();
	if (_TaskInfo.TaskType == SmTaskType::HdDmSymbolCodeList) {
		StartGetDmSymbolMaster();
	}
	else if (_TaskInfo.TaskType == SmTaskType::HdDmSymbolMaster) {
		StartGetCSise();
	}
	else if (_TaskInfo.TaskType == SmTaskType::HdSymbolSise) {
		StartGetHoga();
	}
	else if (_TaskInfo.TaskType == SmTaskType::HdSymbolHoga) {
		EndAllTask();
	}
}

void ViServerDataReceiver::InitRequestQ()
{

}

void ViServerDataReceiver::ExecNext()
{
	if (_TaskInfo.argMap.size() == 0)
		return;
	// ���� ������ ��Ҹ� �����´�.
	auto front = _TaskInfo.argMap.begin();
	SmTaskArg& arg = front->second;
	_TaskInfo.TaskDetail = arg.Detail;
	// �۾� ���¸� �����Ѵ�.
	SetTaskState();
	// ��û���� �ʾҴٸ� ��û�� �Ѵ�.
	if (!arg.Requested) {
		ExecRequest(arg);
	}
}

SmTaskArg* ViServerDataReceiver::FindTask(int id)
{
	auto it = _TaskInfo.argMap.find(id);
	if (it != _TaskInfo.argMap.end())
		return &it->second;

	return nullptr;
}

void ViServerDataReceiver::OnTaskComplete(int reqId)
{
	// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
	auto it = _TaskInfo.argMap.find(reqId);
	if (it != _TaskInfo.argMap.end()) {
		_TaskInfo.argMap.erase(it);
	}
	else {
		// �̹� ������ ��û�̳� ���� ��û�� ó������ �ʴ´�.
		return;
	}
	// ���� ������ �ٽ� ������ �ش�.
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	SetTaskState();
	if (_TaskInfo.RemainTaskCount == 0) {
		//((CMainFrame*)AfxGetMainWnd())->StopTimer();
		DoNextGroupTask();
	}
}

void ViServerDataReceiver::OnTaskError(int reqId)
{
	// ������ ���� �ٽ� ��û�� ���ش�.
	auto it = _TaskInfo.argMap.find(reqId);
	if (it != _TaskInfo.argMap.end()) {
		SmTaskArg& arg = it->second;
		arg.Requested = false;
	}
}

void ViServerDataReceiver::MakeDmSymbolMasterRequests()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::vector<std::string>& product_set = SmMarketManager::GetInstance()->GetMainDmFutures();
	std::vector<std::shared_ptr<SmSymbol>> symbol_list;
	for (auto it = product_set.begin(); it != product_set.end(); ++it) {
		std::string product_code = (*it).substr(0, 3);
		std::shared_ptr<SmProduct> product = mrktMgr->FindProduct(product_code);
		if (product) {
			std::vector<std::shared_ptr<SmSymbol>>& sym_vec = product->GetSymbolList();
			for (size_t i = 0; i < sym_vec.size(); ++i) {
				std::shared_ptr<SmSymbol> symbol = sym_vec[i];
				symbol_list.push_back(symbol);
			}
		}
	}

	for (size_t i = 0; i < symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = symbol_list[i];
		SmTaskArg arg;
		arg.Desc = "�������� ���������� �������� ���Դϴ�.";
		arg.Detail = symbol->SymbolCode();
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::HdDmSymbolMaster;
		arg.AddArg("symbol_code", symbol->SymbolCode());
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	_TaskInfo.TaskTitle = "�������� ���������� �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::HdDmSymbolMaster;
}

void ViServerDataReceiver::MakeDmSymbolCodeRequests()
{
	std::vector<std::string>& future_list = SmMarketManager::GetInstance()->GetMainDmFutures();
	for (size_t i = 0; i < future_list.size(); ++i) {
		std::string code = future_list[i];
		SmTaskArg arg;
		arg.Desc = "�������� ����� �������� ���Դϴ�.";
		arg.Detail = code;
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::HdDmSymbolCodeList;
		arg.AddArg("product_code", code);
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	_TaskInfo.TaskTitle = "�������� ����� �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::HdDmSymbolCodeList;
}

void ViServerDataReceiver::MakeSymbolMasterRequests()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::set<std::string>& product_set = mrktMgr->GetAbroadProductSet();
	std::vector<std::shared_ptr<SmSymbol>> symbol_list;
	for (auto it = product_set.begin(); it != product_set.end(); ++it) {
		std::shared_ptr<SmProduct> product = mrktMgr->FindProduct(*it);
		if (product) {
			std::shared_ptr<SmSymbol> symbol = product->GetRecentMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
			symbol = product->GetNextMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
		}
	}

	for (size_t i = 0; i < symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = symbol_list[i];
		SmTaskArg arg;
		arg.Desc = "���������� �������� ���Դϴ�.";
		arg.Detail = symbol->SymbolCode();
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::HdSymbolMaster;
		arg.AddArg("symbol_code", symbol->SymbolCode());
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	_TaskInfo.TaskTitle = "���������� �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::HdSymbolMaster;
}

void ViServerDataReceiver::MakeSymbolSiseRequests()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::set<std::string>& product_set = mrktMgr->GetAbroadProductSet();
	std::vector<std::shared_ptr<SmSymbol>> symbol_list;
	for (auto it = product_set.begin(); it != product_set.end(); ++it) {
		std::shared_ptr<SmProduct> product = mrktMgr->FindProduct(*it);
		if (product) {
			std::shared_ptr<SmSymbol> symbol = product->GetRecentMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
			symbol = product->GetNextMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
		}
	}

	for (size_t i = 0; i < symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = symbol_list[i];
		SmTaskArg arg;
		arg.Desc = "�ü������� �������� ���Դϴ�.";
		arg.Detail = symbol->SymbolCode();
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::HdSymbolSise;
		arg.AddArg("symbol_code", symbol->SymbolCode());
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	_TaskInfo.TaskTitle = "�ü������� �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::HdSymbolSise;

}

void ViServerDataReceiver::MakeSymbolHogaRequests()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::set<std::string>& product_set = mrktMgr->GetAbroadProductSet();
	std::vector<std::shared_ptr<SmSymbol>> symbol_list;
	for (auto it = product_set.begin(); it != product_set.end(); ++it) {
		std::shared_ptr<SmProduct> product = mrktMgr->FindProduct(*it);
		if (product) {
			std::shared_ptr<SmSymbol> symbol = product->GetRecentMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
			symbol = product->GetNextMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
		}
	}

	for (size_t i = 0; i < symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = symbol_list[i];
		SmTaskArg arg;
		arg.Desc = "ȣ�������� �������� ���Դϴ�.";
		arg.Detail = symbol->SymbolCode();
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::HdSymbolHoga;
		arg.AddArg("symbol_code", symbol->SymbolCode());
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	_TaskInfo.TaskTitle = "ȣ�������� �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::HdSymbolHoga;
}

void ViServerDataReceiver::MakeSymbolRequests()
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::vector<SmRunInfo> run_list = marketMgr->GetRunInfoList();

	for (auto it = run_list.begin(); it != run_list.end(); ++it) {
		SmRunInfo& info = *it;

		SmTaskArg arg;
		arg.Detail = info.Name;
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::EbSymbolCode;
		arg.AddArg("product_code", info.Code);

		_TaskInfo.argMap[arg.ArgId] = arg;
	}

	_TaskInfo.TaskTitle = "�ɺ��ڵ带 �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::EbSymbolCode;
}

void ViServerDataReceiver::MakeAccountAssetRequests()
{
	
}

void ViServerDataReceiver::MakeAccoutProfitLoss()
{
	
}

void ViServerDataReceiver::MakeRemainRequests()
{
	
}

void ViServerDataReceiver::MakeAccountOrders()
{
	
}

void ViServerDataReceiver::EndAllTask()
{
	//SmMarketManager::GetInstance()->MakeFavoriteList();
	((CMainFrame*)AfxGetMainWnd())->StopTimer();
	((CMainFrame*)AfxGetMainWnd())->HideProgress();
}

void ViServerDataReceiver::MakeChartDataRequests()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::set<std::string>& product_set = mrktMgr->GetAbroadProductSet();
	std::vector<std::shared_ptr<SmSymbol>> symbol_list;
	for (auto it = product_set.begin(); it != product_set.end(); ++it) {
		std::shared_ptr<SmProduct> product = mrktMgr->FindProduct(*it);
		if (product) {
			std::shared_ptr<SmSymbol> symbol = product->GetRecentMonthSymbol();
			// �ٿ���
			if (symbol) symbol_list.push_back(symbol);
			// ������
			symbol = product->GetNextMonthSymbol();
			if (symbol) symbol_list.push_back(symbol);
		}
	}
	SmTaskArg arg;

	for (size_t i = 0; i < symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = symbol_list[i];
		arg.Desc = "��Ʈ�����͸� �������� ���Դϴ�.";
		arg.Detail = symbol->SymbolCode();
		arg.ArgId = ViServerDataReceiver::GetId();
		arg.TaskType = SmTaskType::HdSymbolChartData;
		arg.AddArg("symbol_code", symbol->SymbolCode());
		arg.AddArg("chart_type", std::to_string((int)SmChartType::TICK));
		arg.AddArg("chart_cycle", std::to_string(300));
		arg.AddArg("count", std::to_string(1500));
		arg.AddArg("next", "0");
		arg.AddArg("abroad", "true");
		_TaskInfo.argMap[arg.ArgId] = arg;
	}

// 	arg.Desc = "��Ʈ�����͸� �������� ���Դϴ�.";
// 	arg.Detail = "NQU20";
// 	arg.ArgId = ViServerDataReceiver::GetId();
// 	arg.TaskType = SmTaskType::HdSymbolChartData;
// 	arg.AddArg("symbol_code", "NQU20");
// 	arg.AddArg("chart_type", std::to_string((int)SmChartType::TICK));
// 	arg.AddArg("chart_cycle", std::to_string(300));
// 	arg.AddArg("count", std::to_string(1500));
// 	arg.AddArg("next", "0");
// 	arg.AddArg("abroad", "true");
// 	_TaskInfo.argMap[arg.ArgId] = arg;

	_TaskInfo.TaskTitle = "��Ʈ�����͸� �������� ���Դϴ�.";
	_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	_TaskInfo.TaskType = SmTaskType::HdSymbolChartData;
}

void ViServerDataReceiver::StartGetSymbolMaster()
{
	MakeSymbolMasterRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
}

void ViServerDataReceiver::StartGetSymbol()
{
	MakeSymbolRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(1100);
}

void ViServerDataReceiver::StartGetCSise()
{
	MakeSymbolSiseRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
}

void ViServerDataReceiver::StartGetHoga()
{
	MakeSymbolHogaRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
}

void ViServerDataReceiver::StartGetAccountAsset()
{
	MakeAccountAssetRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
}

void ViServerDataReceiver::StartGetProductRemain()
{
	MakeRemainRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
}

void ViServerDataReceiver::StartGetAccountOrder()
{
	MakeAccountOrders();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
}

void ViServerDataReceiver::StartGetAccountProfitLoss()
{
	MakeAccoutProfitLoss();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
}

void ViServerDataReceiver::StartGetChartData()
{
	MakeChartDataRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
}

void ViServerDataReceiver::StartGetDmSymbolCode()
{
	MakeDmSymbolCodeRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(500);
}

void ViServerDataReceiver::StartGetDmSymbolMaster()
{
	MakeDmSymbolMasterRequests();
	((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
}
