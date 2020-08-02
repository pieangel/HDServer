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
	// ü��� �ֹ��� �߰��Ѵ�. 
	virtual void AddFilledOrder(std::shared_ptr<SmOrder> order);
	// �ֹ�Ȯ�� ��Ͽ� �߰��Ѵ�.
	virtual void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
protected:
	// ��ü �ֹ� �� - �ֹ���ȣ�� ���� ���� ����
	std::map<int, std::shared_ptr<SmOrder>> _OrderMap;
	// ������ �ֹ� �� - �ֹ� ��ȣ�� ���� ���� ����
	std::map<int, std::shared_ptr<SmOrder>> _AcceptedOrderMap;
	// ü��Ǿ� ���� �ִ� �ֹ� ���
	// ü��Ǿ� û��Ǹ� _SettledMap���� �Ű� ����.
	// �ֹ� ��ȣ�� ���� ���� ���� - ���� ���� �ֹ��� �� �տ� �ִ�.
	std::map<int, std::shared_ptr<SmOrder>> _FilledOrderMap;
	// ������ û��� �ֹ� ���
	// �ֹ� ��ȣ�� ���� ���� ���� - ���� ���� �ֹ��� �� �տ� �ִ�.
	std::map<int, std::shared_ptr<SmOrder>> _SettledMap;
	std::shared_ptr<SmOrder> AddOrder(int orderNo);
	void AddOrder(std::shared_ptr<SmOrder> order);
	std::shared_ptr<SmOrder> FindOrder(int order_no);
	// �ֹ��� ó�� �������� �� �̺�Ʈ ó��
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// �ֹ� ����Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// �ֹ� ü�� Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// �ֹ� û�� Ȯ�� �̺�Ʈ ó��
	virtual void OnOrderSettled(std::shared_ptr<SmOrder> order);
};

