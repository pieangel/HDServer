#include "pch.h"
#include "afxwin.h"
#include "SmSymbolReader.h"
#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "../Market/SmMarket.h"
#include "../Market/SmMarketManager.h"
#include "../Market/SmProduct.h"
#include "SmSymbol.h"
#include "../Service/SmTimeSeriesServiceManager.h"
#include "SmSymbolManager.h"
#include "../Database/SmTimeSeriesDBManager.h"
//namespace fs = std::filesystem;
#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

SmSymbolReader::SmSymbolReader()
{
}


SmSymbolReader::~SmSymbolReader()
{
}

std::string SmSymbolReader::GetWorkingDir()
{
	fs::path cwp = fs::current_path();
	return cwp.string();
}


void SmSymbolReader::ReadSymbolFromFile(int index, std::string fullPath)
{
	switch (index)
	{
	case 0:
		ReadMarketFile(fullPath);
		break;
	case 1:
		ReadPmFile(fullPath);
		break;
	case 2:
		ReadJmFile(fullPath);
		break;
	default:
		break;
	}
}

void SmSymbolReader::ReadMarketFile()
{
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\MRKT.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		std::string exchange = line.substr(20, 5);
		std::string pmCode = line.substr(25, 3);
		std::string enName = line.substr(28, 50);
		std::string name = line.substr(78, 50);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exchange.c_str(), pmCode.c_str(), name.c_str(), enName.c_str());
		//TRACE(msg);
	}
}

void SmSymbolReader::ReadMarketFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market_type = line.substr(0, 20);
		std::string exchange = line.substr(20, 5);
		std::string pmCode = line.substr(25, 3);
		std::string enName = line.substr(28, 50);
		std::string name = line.substr(78, 50);
		
		boost::trim_right(market_type);
		boost::trim_right(exchange);
		boost::trim_right(pmCode);
		boost::trim_right(enName);
		boost::trim_right(name);
		
		std::shared_ptr<SmMarket> market = marketMgr->AddMarket(market_type);
		std::shared_ptr<SmProduct> cat = market->AddProduct(pmCode);
		cat->MarketName(market_type);
		cat->Exchange(exchange);
		cat->Name(enName);
		cat->NameKr(name);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market_type.c_str(), exchange.c_str(), pmCode.c_str(), name.c_str(), enName.c_str());
		//TRACE(msg);
	}
}

void SmSymbolReader::ReadPmFile()
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\PMCODE.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		// 거래소 인덱스 코드
		std::string exIndexCode = line.substr(20, 4);
		// 거래소 코드 
		std::string exChangeCode = line.substr(24, 5);
		// 품목 코드
		std::string pmCode = line.substr(29, 5);
		// 시장 구분을 숫자로 함
		std::string pmGubun = line.substr(34, 3);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exIndexCode.c_str(), exChangeCode.c_str(), pmCode.c_str(), pmGubun.c_str());
		//TRACE(msg);
		boost::trim_right(market);
		boost::trim_right(exIndexCode);
		boost::trim_right(exChangeCode);
		boost::trim_right(pmCode);
		boost::trim_right(pmGubun);
		std::shared_ptr<SmProduct> cat = marketMgr->FindProduct(market, pmCode);
		if (cat) {
			cat->ExchangeCode(exChangeCode);
			cat->ExchangeIndex(exIndexCode);
			cat->MarketCode(pmGubun);
		}
	}
}



void SmSymbolReader::ReadPmFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		std::string exIndexCode = line.substr(20, 4);
		std::string exChangeCode = line.substr(24, 5);
		std::string pmCode = line.substr(29, 5);
		std::string pmGubun = line.substr(34, 3);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exIndexCode.c_str(), exChangeCode.c_str(), pmCode.c_str(), pmGubun.c_str());
		//TRACE(msg);

		boost::trim_right(market);
		boost::trim_right(exIndexCode);
		boost::trim_right(exChangeCode);
		boost::trim_right(pmCode);
		boost::trim_right(pmGubun);
		std::shared_ptr<SmProduct> cat = marketMgr->FindProduct(market, pmCode);
		if (cat) {
			cat->ExchangeCode(exChangeCode);
			cat->ExchangeIndex(exIndexCode);
			cat->MarketCode(pmGubun);
			marketMgr->AddCategoryMarket(pmCode, market);
		}
	}
}

void SmSymbolReader::ReadJmFile()
{
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\JMCODE.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string Series = line.substr(0, 32);
		/* 종목코드                             */

		std::string ExchCd = line.substr(32, 5);
		/* 거래소                               */

		std::string IndexCode = line.substr(37, 4);
		/* 품목 인덱스코드                      */

		std::string MrktCd = line.substr(41, 5);
		/* 품목코드                             */

		std::string ExchNo = line.substr(46, 5);
		/* 거래소 번호                          */

		std::string Pdesz = line.substr(51, 5);
		/* 소수점 정보                          */

		std::string Rdesz = line.substr(56, 5);
		/* 소수점 정보2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* 계약크기                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* 거래승수                             */

		std::string DispDigit = line.substr(141, 10);
		/* 진법                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full 종목명                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full 종목명한글                      */

		std::string NearSeq = line.substr(215, 1);
		/* 최근월물, 주요종목표시               */

		std::string StatTp = line.substr(216, 1);
		/* 거래가능여부                         */

		std::string LockDt = line.substr(217, 8);
		/* 신규거래제한일                       */

		std::string TradFrDt = line.substr(225, 8);
		/* 최초거래일                           */

		std::string TradToDt = line.substr(233, 8);
		/* 최종거래일                           */

		std::string ExprDt = line.substr(241, 8);
		/* 만기일, 최종결제일                   */

		std::string RemnCnt = line.substr(249, 4);
		/* 잔존일수                             */

		std::string HogaMthd = line.substr(253, 30);
		/* 호가방식                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* 상하한폭비율                         */

		std::string BaseP = line.substr(289, 20);
		/* 기준가                               */

		std::string MaxP = line.substr(309, 20);
		/* 상한가                               */

		std::string MinP = line.substr(329, 20);
		/* 하한가                               */

		std::string TrstMgn = line.substr(349, 20);
		/* 신규주문증거금                       */

		std::string MntMgn = line.substr(369, 20);
		/* 유지증거금                           */

		std::string CrcCd = line.substr(389, 3);
		/* 결제통화코드                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* 매수이자                             */

		std::string SellInt = line.substr(438, 20);
		/* 매도이자                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* 진법자리수                           */

		std::string decimalchiper = line.substr(474, 5);
		/* 소수점 정보(KTB기준)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* 전일거래량                           */



		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		//TRACE(msg);
	}
}

void SmSymbolReader::ReadJmFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string Series = line.substr(0, 32);
		/* 종목코드                             */

		std::string ExchCd = line.substr(32, 5);
		/* 거래소                               */

		std::string IndexCode = line.substr(37, 4);
		/* 품목 인덱스코드                      */

		std::string MrktCd = line.substr(41, 5);
		/* 품목코드                             */

		std::string ExchNo = line.substr(46, 5);
		/* 거래소 번호                          */

		std::string Pdesz = line.substr(51, 5);
		/* 소수점 정보                          */

		std::string Rdesz = line.substr(56, 5);
		/* 소수점 정보2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* 계약크기                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* 거래승수                             */

		std::string DispDigit = line.substr(141, 10);
		/* 진법                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full 종목명                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full 종목명한글                      */

		std::string NearSeq = line.substr(215, 1);
		/* 최근월물, 주요종목표시               */

		std::string StatTp = line.substr(216, 1);
		/* 거래가능여부                         */

		std::string LockDt = line.substr(217, 8);
		/* 신규거래제한일                       */

		std::string TradFrDt = line.substr(225, 8);
		/* 최초거래일                           */

		std::string TradToDt = line.substr(233, 8);
		/* 최종거래일                           */

		std::string ExprDt = line.substr(241, 8);
		/* 만기일, 최종결제일                   */

		std::string RemnCnt = line.substr(249, 4);
		/* 잔존일수                             */

		std::string HogaMthd = line.substr(253, 30);
		/* 호가방식                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* 상하한폭비율                         */

		std::string BaseP = line.substr(289, 20);
		/* 기준가                               */

		std::string MaxP = line.substr(309, 20);
		/* 상한가                               */

		std::string MinP = line.substr(329, 20);
		/* 하한가                               */

		std::string TrstMgn = line.substr(349, 20);
		/* 신규주문증거금                       */

		std::string MntMgn = line.substr(369, 20);
		/* 유지증거금                           */

		std::string CrcCd = line.substr(389, 3);
		/* 결제통화코드                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* 매수이자                             */

		std::string SellInt = line.substr(438, 20);
		/* 매도이자                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* 진법자리수                           */

		std::string decimalchiper = line.substr(474, 5);
		/* 소수점 정보(KTB기준)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* 전일거래량                           */


		boost::trim_right(Series);
		boost::trim_right(SeriesNm);
		boost::trim_right(SeriesNmKor);
		boost::trim_right(MrktCd);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		TRACE(msg);

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmProduct> cat = marketMgr->FindProduct(MrktCd);
		if (cat) {
			std::shared_ptr<SmSymbol> sym = cat->AddSymbol(Series);
			sym->Index(std::stoi(IndexCode));
			sym->Name(SeriesNmKor);
			sym->NameEn(SeriesNm);
			symMgr->AddSymbol(sym);
			sym->CategoryCode(cat->Code());
			sym->MarketName(cat->MarketName());
			sym->Decimal(std::stoi(Pdesz));
			sym->Seungsu(std::stoi(MltiPler));
			sym->CtrUnit(std::stod(CtrtSize));
			sym->TickValue(std::stod(TickValue));
			sym->TickSize(std::stod(TickSize));

			SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
			//dbMgr->SaveSymbol(sym);

			double profit = sym->TickSize() * sym->CtrUnit();
			if (Series.compare("CLQ19") == 0) {
				int k = 0;
			}
		}
	}
}

