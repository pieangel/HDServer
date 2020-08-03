#pragma once
#include <map>
#include <set>
#include <functional>
#include "../Global/TemplateSingleton.h"

#define WM_QUOTE_CHANGED  WM_USER + 3001
#define WM_HOGA_CHANGED  WM_USER + 3002
#define WM_ORDER_CHANGED  WM_USER + 3003
#define WM_ACCOUNT_CHANGED  WM_USER + 3004

class SmSymbol;
struct VtOrder;
class SmChartData;
class VtAccount;
class SmCallbackManager : public TemplateSingleton<SmCallbackManager>
{
public:
	SmCallbackManager();
	~SmCallbackManager();
	
	void SubscribeQuoteCallback(long id, std::function <void(SmSymbol* symbol)> callback) {
		_QuoteMap[id] = callback;
	}
	void UnsubscribeQuoteCallback(long id);
	void OnQuoteEvent(SmSymbol* symbol);
	void OnHogaEvent(SmSymbol* symbol);
	
	void SubscribeHogaCallback(long id, std::function <void(SmSymbol* symbol)> callback) {
		_HogaMap[id] = callback;
	}
	void UnsubscribeHogaCallback(long id);

	void SubscribeOrderCallback(long id, std::function <void( VtOrder* order)> callback) {
		_OrderMap[id] = callback;
	}
	void UnsubscribeOrderCallback(long id);

	void OnOrderEvent( VtOrder* order);

	void SubscribeMasterCallback(long id, std::function <void(SmSymbol* symbol)> callback) {
		_MasterMap[id] = callback;
	}
	void UnsubscribeMasterCallback(long id);
	void OnMasterEvent(SmSymbol* symbol);

	void SubscribeChartCallback(long id, std::function <void(SmChartData* chart_data)> callback) {
		_ChartMap[id] = callback;
	}
	void UnsubscribeChartCallback(long id);
	void OnChartEvent(SmChartData* chart_data);

	void SubscribeAccountCallback(long id, std::function <void(VtAccount* account)> callback) {
		_AccountMap[id] = callback;
	}
	void UnsubscribeAccountCallback(long id);
	void OnAccountEvent(VtAccount* account);

	void SubscribeQuoteWndCallback(HWND id) {
		_WndQuoteSet.insert(id);
	}
	void UnsubscribeQuoteWndCallback(HWND id) {
		auto it = _WndQuoteSet.find(id);
		if (it != _WndQuoteSet.end()) {
			_WndQuoteSet.erase(it);
		}
	}

	void SubscribeHogaWndCallback(HWND id) {
		_WndHogaSet.insert(id);
	}
	void UnsubscribeHogaWndCallback(HWND id) {
		auto it = _WndHogaSet.find(id);
		if (it != _WndHogaSet.end()) {
			_WndHogaSet.erase(it);
		}
	}
	
	void OnWndQuoteEvent(SmSymbol* symbol);

	
	void OnWndHogaEvent(SmSymbol* symbol);

	void SubscribeOrderWndCallback(HWND id) {
		_WndOrderSet.insert(id);
	}
	void UnsubscribeOrderWndCallback(HWND id) {
		auto it = _WndOrderSet.find(id);
		if (it != _WndOrderSet.end()) {
			_WndOrderSet.erase(it);
		}
	}

	void OnWndOrderEvent(VtOrder* order);

	void SubscribeAccountWndCallback(HWND id) {
		_WndAccountSet.insert(id);
	}
	void UnsubscribeAccountWndCallback(HWND id) {
		auto it = _WndAccountSet.find(id);
		if (it != _WndAccountSet.end()) {
			_WndAccountSet.erase(it);
		}
	}

	void OnWndAccountEvent(VtAccount* account);
	
private:
	std::map<long, std::function<void(SmSymbol* symbol)>> _QuoteMap;
	std::map<long, std::function<void(SmSymbol* symbol)>> _HogaMap;
	std::map<int, std::function<void( VtOrder* order)>> _OrderMap;
	std::map<int, std::function<void(SmSymbol* symbol)>> _MasterMap;
	std::map<int, std::function<void(SmChartData* chart_data)>> _ChartMap;
	std::map<int, std::function<void(VtAccount* account)>> _AccountMap;
	std::set<HWND> _WndQuoteSet;
	std::set<HWND> _WndHogaSet;
	// 주문 이벤트를 받을 창들의 핸들 집합
	std::set<HWND> _WndOrderSet;
	// 계좌 이벤트를 받을 창들의 핸들 집합
	std::set<HWND> _WndAccountSet;
};

