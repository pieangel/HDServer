#pragma once
#include <map>
#include <string>
#include "../pch.h"

enum class SmAppState
{
	// �ʱ� ����
	None = 0, 
	// ���� ���� �ɺ� ����
	JisuFtSymbolReceived,
	// ���� �ɼ� �ɺ� ����
	JisuOpSymbolReceived,
	// ��ǰ ���� �ɺ� ����
	CommodityFtSymbolReceived,
	// �̴� �ڽ��� ���� �ɺ� ����
	MiniFtSymbolReceived,
	// �̴� �ڽ��� ���� �ɼ� �ɺ� ����
	MinitOpSymbolReceived,
	// EFT �ɺ� ����
	ETFSymbolReceived,
	/// <summary>
	/// ��Ŭ�� �ɼ� �ɺ� ����
	/// </summary>
	WeeklyOpSymbolReceived,
	// �ؿ� ���� �ɺ� ��� ����
	AbroadSymbolReceived
};

enum class SmMarketType
{
	None = 0,
	Stock, // �Ϲ� �ֽ�
	Etf, // ETF
	StockFuture, // �ֽ� ����
	DomesticFuture, // ���� ����
	DomesticOption, // ���� �ɼ�
	AbroadFuture, // �ؿ� ����
	AbroadOption // �ؿ� �ɼ�
};


enum class SmTaskType
{
	// �ʱ� ����
	None = 0,
	// ���� ���� �ɺ� 
	JisuFtSymbol,
	// ���� �ɼ� �ɺ� 
	JisuOpSymbol,
	// ��ǰ ���� �ɺ� 
	CommodityFtSymbol,
	// �̴� �ڽ��� ���� �ɺ� 
	MiniFtSymbol,
	// �̴� �ڽ��� ���� �ɼ� �ɺ� 
	MiniOpSymbol,
	// EFT �ɺ� ����
	EtfSymbol,
	/// <summary>
	/// ��Ŭ�� �ɼ� �ɺ� ����
	/// </summary>
	WeeklyOpSymbol,
	// �ؿ� ���� �ɺ� ��� ����
	AbroadSymbol,
	EbSymbolCode, // �ɺ� �ڵ�
	EbSymbolMaster, // �ɺ� ������
	EBAccountAsset, // ��Ź �ܰ�
	EBAccountProfitLoss, // ���º� ����
	EBProductRemain, // ���� �ܰ�� �� ����
	EBAccountOrder, // ���º� �ֹ� ����
	HdSymbolMaster, // �ؿ� ���� ����
	HdSymbolSise, // �ؿ� ���� �ü�
	HdSymbolHoga, // �ؿ� ���� ȣ��
	HdSymbolChartData, // �ؿ� ���� ��Ʈ ������
	HdAccountAsset, // �ؿ� ���� 
	HdAccountProfitLoss, // �ؿ� ���� ����
	HdAccountSymbolRemain, // �ؿ� ���� �ܰ�
	HdAcceptedOrderList, // �ؿ� ��ü�� �ֹ�
	HdDmSymbolCodeList, // ���� �ɺ� �ڵ� ����Ʈ
	HdDmSymbolMaster // ���� �ɺ� ������
};

struct SmTaskArg {
	// ��û ���̵�
	int ArgId = 0;
	// ��û�ߴ��� ����
	bool Requested = false;
	// �޾Ҵ��� ����
	bool Received = false;
	SmTaskType TaskType = SmTaskType::None;
	// �۾� ����
	std::string Desc;
	// �۾� ����
	std::string Detail;
	// �Ű����� ��
	std::map<std::string, std::string> ArgMap;

	void AddArg(std::string key, std::string value) {
		ArgMap[key] = value;
	}

	std::string GetArg(std::string key) {
		std::string value;
		auto it = ArgMap.find(key);
		if (it != ArgMap.end()) {
			value = it->second;
		}

		return value;
	}
};

struct SmTaskInfo {
	SmTaskType TaskType;
	std::string TaskTitle;
	std::string TaskDetail;
	int TotalTaskCount;
	int RemainTaskCount;
	std::map<int, SmTaskArg> argMap;
};


#define WMU_SHOW_TR		( WM_USER + 1000 )
#define WMU_SET_MESSAGE	( WM_USER + 1001 )

enum
{
	DATA_TYPE_STRING = 0,		// ���ڿ�
	DATA_TYPE_LONG,				// ����
	DATA_TYPE_FLOAT,			// �Ǽ�
	DATA_TYPE_FLOAT_DOT,		// �Ǽ�( �Ҽ����� ������ ���� )
	DATA_TYPY_FLOAT_TO_LONG     // �Ǽ����� �Ҽ����� ������ �Ŀ� ������ �ٲ� ��
};
#define DATA_TYPE_DOUBLE	DATA_TYPE_FLOAT
#define DATA_TYPE_DOT		DATA_TYPE_FLOAT_DOT



