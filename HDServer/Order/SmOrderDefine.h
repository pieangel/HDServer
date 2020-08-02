#pragma once
#include <string>

enum class SmOrderError {
	None,
	BadAccount,
	BadSymbol,
	BadPriceType,
};

enum class SmPositionType
{
	None = 0,
	Buy,
	Sell,
	ExitBuy,
	ExitSell
};

enum class SmOrderType
{
	None = 0,
	New,
	Modify,
	Cancel
};

enum class SmPriceType
{
	None = 0,
	Price, // 지정가
	Market, // 시장가
	Condition, // 조건
	BestPrice // 최상위 유리가
};

//1:FAS, 2:FOK, 3:FAK
enum class SmFilledCondition
{
	None = 0,
	Fas = 1, //
	Fok = 2, //
	Fak = 3
};

enum class SmOrderState
{
	None,
	/// <summary>
	/// 원장접수
	/// </summary>
	Ledger,
	/// 신규 확인
	ConfirmNew,
	/// <summary>
	/// 정정 확인
	/// </summary>
	ConfirmModify,
	/// <summary>
	/// 취소 확인
	/// </summary>
	ConfirmCancel,
	/// <summary>
	/// 주문접수
	/// </summary>
	Accepted,
	/// <summary>
	/// 체결
	/// </summary>
	Filled,
	/// <summary>
	/// 신규주문거부
	/// </summary>
	RejectNew,
	/// <summary>
	/// 정정거부
	/// </summary>
	RejectModify,
	/// <summary>
	/// 취소 거부
	/// </summary>
	RejectCancel,
	// 청산됨 - 다른 주문에 의해서 혹은 잔고를 청산하는 용도로
	Settled
};

enum class SmOrderCode {
	None = 100,
	NotFountSymbol,
	UnknownPriceType,
	OrderCreated,
	OrderAccepted,
	OrderFilled,
	PositionCalculated
};


struct SmOrderRequest
{
	// 주문을 낸 사용자의 아이디
	std::string UserID;
	std::string AccountNo;
	std::string Password;
	std::string SymbolCode;
	//매매구분01 1: 매수, 2 : 매도
	SmPositionType Position;
	//가격조건011 : 지정가, 2 : 시장가
	SmPriceType PriceType;
	//체결조건011 : FAS, 2 : FOK, 3 : FAK
	SmFilledCondition FillCondition;
	int OrderPrice;
	//주문수량05주문 수량(ex:"1    ")
	int OrderAmount;
	// 정정이나 취소시 원 주문 번호
	int OriOrderNo = -1;
	// 주문요청 아이디
	int RequestID = 0;
	/// <summary>
	/// 주문 타입 : 신규, 정정, 취소
	/// </summary>
	SmOrderType OrderType;
	// Fund Name
	std::string FundName = "";
	// 전략 이름
	std::string StrategyName = "";
	// 부모 계좌 번호
	std::string ParentAccountNo;
	std::string SystemName;
};
