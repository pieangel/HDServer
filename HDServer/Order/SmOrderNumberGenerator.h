#pragma once
class SmOrderNumberGenerator
{
private:
	static int _ID;
public:
	static void SetID(int id);
	static int GetID() {
		++_ID;
		// �ֹ��� ���� �ִ� ��ȣ�� �����ϸ� 1�� �����Ѵ�.
		if (_ID > 2147483647)
			_ID = 1;
		return _ID;
	}
	static void LoadOrderNo();
};

