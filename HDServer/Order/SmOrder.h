#pragma once
#include "SmOrderDefine.h"
#include <vector>

// 서브계좌 주문은 accountNo는 부모의 계좌번호를 사용하고
// SubAccountNo 에 계좌 번호를 따로 저장해서 사용한다.
// 일반계좌 주문이든 서브계좌 주문이든  accountNo에는 실제 계좌의 번호가 들어있다.
struct SmOrder
{
	// 주문을 낸 사용자의 아이디
	std::string UserID;
	/// <summary>
	/// 주문 요구 고유 번호
	/// 서버에서 주문번호를 주기전에
	/// 클라이언트 측에서 주문을 구별하기 위한 번호
	/// </summary>
	int RequestID = 0;
	/// <summary>
	/// 주문 계좌
	/// </summary>
	std::string AccountNo;
	/// <summary>
	/// 주문 타입 : 신규, 정정, 취소
	/// </summary>
	SmOrderType OrderType;
	/// <summary>
	/// 포지션 타입 : 매도, 매수
	/// </summary>
	SmPositionType Position;
	/// <summary>
	/// 가격 형태 : 지정가, 시장가, 조건, 최상위 유리가
	/// </summary>
	SmPriceType PriceType;
	/// <summary>
	/// 종목 코드
	/// 차트가 동일한 종목 코드를 가질 경우 모두 업데이트 대상이 된다.
	/// </summary>
	std::string SymbolCode;
	/// <summary>
	/// 주문 가격
	/// </summary>
	int OrderPrice = 0;
	/// <summary>
	/// 주문번호
	/// </summary>
	int OrderNo = 0;
	/// <summary>
	/// 주문 수량
	/// </summary>
	int OrderAmount = 0;
	/// <summary>
	/// 원 주문 번호 : 정정이나 취소 때 필요함
	/// </summary>
	int OriOrderNo = 0;
	/// <summary>
	/// 거래 날짜
	/// </summary>
	std::string FilledDate;
	std::string FilledTime;
	// 주문을 넣은 날짜
	std::string OrderDate;
	std::string OrderTime;
	/// <summary>
	/// 체결 수량
	/// </summary>
	int FilledQty = 0;
	/// <summary>
	/// 체결 가격
	/// </summary>
	int FilledPrice = 0;
	/// <summary>
	/// 주문 상태
	/// </summary>
	SmOrderState OrderState;
	/// <summary>
	/// 체결조건 (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)	
	/// </summary>
	SmFilledCondition FilledCondition;
	// 제품의 소수점
	int SymbolDecimal;
	/// <summary>
	/// 체결된 잔고 수량 - 청산이 되면 0이 된다.
	/// 잔고 수량은 매도는 -로 표시하고 매수는 +로 표시한다.
	/// + 3 : 매수 잔고 3, -2 : 매도 잔고 2
	/// </summary>
	int RemainQty = 0;
	// 부모 계좌 번호
	std::string ParentAccountNo;
	// 전략 이름
	std::string StrategyName = "";
	/// <summary>
	/// The name of the underlying fund.
	/// </summary>
	std::string FundName;
	std::string SystemName;
	/// 접수된 가격
	int AcceptedPrice;
	// 이 주문이 청산 시킨 주문 목록
	std::vector<int> SettledOrders;
};
