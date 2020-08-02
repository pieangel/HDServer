#pragma once
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>

struct SmOrder;
struct ByOrderNo
{
	bool operator()(const int& lhs, const int& rhs) const
	{
		return lhs < rhs;
	}
};

class SmOrderManager
{
public:
	SmOrderManager();
	virtual ~SmOrderManager();
	// 체결된 주문에 추가한다. 
	virtual void AddFilledOrder(std::shared_ptr<SmOrder> order);
	// 주문확인 목록에 추가한다.
	virtual void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
protected:
	// 전체 주문 맵 - 주문번호에 따른 오름 차순
	std::map<int, std::shared_ptr<SmOrder>> _OrderMap;
	// 접수된 주문 맵 - 주문 번호에 따른 오름 차순
	std::map<int, std::shared_ptr<SmOrder>> _AcceptedOrderMap;
	// 체결되어 남아 있는 주문 목록
	// 체결되어 청산되면 _SettledMap으로 옮겨 간다.
	// 주문 번호에 따른 오름 차순 - 먼저 들어온 주문이 맨 앞에 있다.
	std::map<int, std::shared_ptr<SmOrder>> _FilledOrderMap;
	// 완전히 청산된 주문 목록
	// 주문 번호에 따른 오름 차순 - 먼저 들어온 주문이 맨 앞에 있다.
	std::map<int, std::shared_ptr<SmOrder>> _SettledMap;
	std::shared_ptr<SmOrder> AddOrder(int orderNo);
	void AddOrder(std::shared_ptr<SmOrder> order);
	std::shared_ptr<SmOrder> FindOrder(int order_no);
	// 주문이 처음 도착했을 때 이벤트 처리
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// 주문 접수확인 때 이벤트 처리
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// 주문 체결 확인 때 이벤트 처리
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// 주문 청산 확인 이벤트 처리
	virtual void OnOrderSettled(std::shared_ptr<SmOrder> order);
};

