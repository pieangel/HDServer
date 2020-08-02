#pragma once
class SmOrderNumberGenerator
{
private:
	static int _ID;
public:
	static void SetID(int id);
	static int GetID() {
		++_ID;
		// 주문이 정수 최대 번호에 도착하면 1로 설정한다.
		if (_ID > 2147483647)
			_ID = 1;
		return _ID;
	}
	static void LoadOrderNo();
};

