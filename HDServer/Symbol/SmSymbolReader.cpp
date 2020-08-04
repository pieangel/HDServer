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
#include "../Config/SmConfigManager.h"
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
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetApplicationPath();
	appPath.append(_T("\\"));
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	std::string file_name = "MRKT.cod";
	TRACE(file_name.c_str());
	std::string file_path = appPath + file_name;
	ReadMarketFile(file_path);
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
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetApplicationPath();
	appPath.append(_T("\\"));
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	std::string file_name = "PMCODE.cod";
	TRACE(file_name.c_str());
	std::string file_path = appPath + file_name;
	ReadPmFile(file_path);
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
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetApplicationPath();
	appPath.append(_T("\\"));
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	std::string file_name = "JMCODE.cod";
	TRACE(file_name.c_str());
	std::string file_path = appPath + file_name;
	ReadJmFile(file_path);
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
		/* �����ڵ�                             */

		std::string ExchCd = line.substr(32, 5);
		/* �ŷ���                               */

		std::string IndexCode = line.substr(37, 4);
		/* ǰ�� �ε����ڵ�                      */

		std::string MrktCd = line.substr(41, 5);
		/* ǰ���ڵ�                             */

		std::string ExchNo = line.substr(46, 5);
		/* �ŷ��� ��ȣ                          */

		std::string Pdesz = line.substr(51, 5);
		/* �Ҽ��� ����                          */

		std::string Rdesz = line.substr(56, 5);
		/* �Ҽ��� ����2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* ���ũ��                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* �ŷ��¼�                             */

		std::string DispDigit = line.substr(141, 10);
		/* ����                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full �����                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full ������ѱ�                      */

		std::string NearSeq = line.substr(215, 1);
		/* �ֱٿ���, �ֿ�����ǥ��               */

		std::string StatTp = line.substr(216, 1);
		/* �ŷ����ɿ���                         */

		std::string LockDt = line.substr(217, 8);
		/* �ű԰ŷ�������                       */

		std::string TradFrDt = line.substr(225, 8);
		/* ���ʰŷ���                           */

		std::string TradToDt = line.substr(233, 8);
		/* �����ŷ���                           */

		std::string ExprDt = line.substr(241, 8);
		/* ������, ����������                   */

		std::string RemnCnt = line.substr(249, 4);
		/* �����ϼ�                             */

		std::string HogaMthd = line.substr(253, 30);
		/* ȣ�����                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* ������������                         */

		std::string BaseP = line.substr(289, 20);
		/* ���ذ�                               */

		std::string MaxP = line.substr(309, 20);
		/* ���Ѱ�                               */

		std::string MinP = line.substr(329, 20);
		/* ���Ѱ�                               */

		std::string TrstMgn = line.substr(349, 20);
		/* �ű��ֹ����ű�                       */

		std::string MntMgn = line.substr(369, 20);
		/* �������ű�                           */

		std::string CrcCd = line.substr(389, 3);
		/* ������ȭ�ڵ�                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* �ż�����                             */

		std::string SellInt = line.substr(438, 20);
		/* �ŵ�����                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* �����ڸ���                           */

		std::string decimalchiper = line.substr(474, 5);
		/* �Ҽ��� ����(KTB����)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* ���ϰŷ���                           */


		boost::trim_right(Series);
		boost::trim_right(SeriesNm);
		boost::trim_right(SeriesNmKor);
		boost::trim_right(MrktCd);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		TRACE(msg);

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmProduct> product = marketMgr->FindProduct(MrktCd);

		bool included = false;
		// �ؿܼ��� ��� �� ����� ��ϸ� �о� �´�.
		std::set<std::string>& abroad_product_set = SmMarketManager::GetInstance()->GetAbroadProductSet();
		auto it = abroad_product_set.find(product->Code());
		if (it != abroad_product_set.end())
			included = true;
		if (product && included) {
			std::shared_ptr<SmSymbol> sym = product->AddSymbol(Series);

			product->AddToYearMonth(sym->SymbolCode(), sym);

			sym->Index(std::stoi(IndexCode));
			sym->Name(SeriesNmKor);
			sym->NameEn(SeriesNm);
			symMgr->AddSymbol(sym);
			sym->CategoryCode(product->Code());
			sym->MarketName(product->MarketName());
			sym->Decimal(std::stoi(Pdesz));
			sym->Seungsu(std::stoi(MltiPler));
			sym->CtrUnit(std::stod(CtrtSize));
			sym->TickValue(std::stod(TickValue));
			sym->TickSize(std::stod(TickSize));

			SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
			//dbMgr->SaveSymbol(sym);

			double profit = sym->TickSize() * sym->CtrUnit();
		}
	}
}

