#pragma once
#include <string>
#include "../Order/SmOrderDefine.h"
struct SmPosition
{
	// 포지션이 만들어진 날짜
	std::string CreatedDate;
	/// <summary>
	/// 포지션이 만들어진 시간
	/// </summary>
	std::string CreatedTime;
	// 심볼 코드
	std::string SymbolCode;
	// 펀드 이름
	std::string FundName;
	// 계좌 번호
	std::string AccountNo;
	// 포지션
	SmPositionType Position = SmPositionType::None; 
	// 포지션의 잔고
	int OpenQty;
	// 수수료는 부과된 횟수만 기록한다. 실제적인 수수료는 개인이 설정한 값에서 결정된다.
	int FeeCount; //		수수료
	double TradePL = 0.0f; //	매매손익
	double	AvgPrice = 0.0f;	/*평균가*/
	double	CurPrice = 0.0f;	/*현재가*/
	double	OpenPL = 0.0f;		/*평가손익*/
	// 포지션을 소유한 사용자 아이디
	std::string UserID;
};

