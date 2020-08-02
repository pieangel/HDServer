#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include "../Json/json.hpp"
#include "SmServiceDefine.h"
#include "../Chart/SmChartDefine.h"
#include <mutex>
class SmWebsocketSession;
// ��� �����ʹ� �޸𸮿����� ������.
// ������ �ε��� �� �����ͺ��̽����� ��� �����͸� �о� �� ������ �����Ѵ�.
// Ŭ���̾�Ʈ���� ������ �޸𸮿� ����� �ڷ� ���������� ������ �ϸ�
// ������ ���̽��� �������� �ʴ´�.
class SmProtocolManager : public TemplateSingleton<SmProtocolManager>
{
public:
	SmProtocolManager();
	~SmProtocolManager();

	void OnMessage(std::string message, SmWebsocketSession* socket);
	void SendIndicatorResult(int result, SmIndicatorRequest ind_req);
	void SendCorrelationResult(double result, SmCorrelationRequest ind_req);
private:
	// ������� ��û
	void OnReqCorrelation(nlohmann::json& obj);
	// ��ǥ ��û
	void OnReqIndicator(nlohmann::json& obj);
	// ���� ��� ��û
	void OnReqTradeList(nlohmann::json& obj);
	// ��Ʈ������ ��û
	void OnReqChartData(std::string message);
	// �޽��� �Ľ�
	void ParseMessage(std::string message, SmWebsocketSession* socket);
	// ��� ����
	void SendResult(std::string user_id, int result_code, std::string result_msg);
	// ��� ����
	void SendResult(int session_id, int result_code, std::string result_msg);
	// �α��� ��û
	void OnReqLogin(nlohmann::json& obj, SmWebsocketSession* socket);
	// �α׾ƿ� ��û
	void OnLogout(nlohmann::json& obj);
	// �ɺ� ��� ��û
	void OnRegisterSymbol(nlohmann::json& obj);
	// �ɺ� ��� ���� ��û
	void OnUnregisterSymbol(nlohmann::json& obj);
	// ��Ʈ �ֱ� ������ ��û
	void OnRegisterChartCycleData(nlohmann::json& obj);
	// ��Ʈ �ֱ� ������ ��û ����
	void OnUnregisterChartCycleData(nlohmann::json& obj);
	// �ֹ� ��û
	void OnOrder(nlohmann::json& obj);
	// ��Ʈ ������ ��û
	void OnReqChartData(nlohmann::json& obj);
	// �ü� ������ ��û
	void OnReqSiseData(nlohmann::json& obj);
	// ȣ�� ������ ��û
	void OnReqHogaData(nlohmann::json& obj);
	// �ɺ� ������ ��û
	void OnReqSymbolMaster(nlohmann::json& obj);
	// ��� �ɺ� ������ ��û
	void OnReqSymbolMasterAll(nlohmann::json& obj);
	// ��� �ü� ������ ��û
	void OnReqSiseDataAll(nlohmann::json& obj);
	// �ֱٿ��� �ü� ������ ��û
	void OnReqRecentSiseDataAll(nlohmann::json& obj);
	// �ֱٿ��� ��� �ü� ������ ��û
	void OnReqRegisterRecentRealtimeSiseAll(nlohmann::json& obj);
	// ���� ��� ��û
	void OnReqMarketList(nlohmann::json& obj);
	// ī�װ��� �ɺ� ��� ��û
	void OnReqSymbolListByCategory(nlohmann::json& obj);
	// ��Ʈ ������ ������ ��û
	void OnReqChartDataResend(nlohmann::json& obj);
	// �ü��� ������Ʈ �Ѵ�. ��ο��� ������.
	// �ü��� ������Ʈ �Ҷ� ��Ʈ �����ʹ� ������Ʈ ���� �ʴ´�. 
	// ��Ʈ �����ʹ� ���������� ��ȴ�. �� Ŭ���̾�Ʈ���� ���� ��Ʈ�����͸� �����ϰ� �־�� �ϸ�
	// ������Ʈ�� ȭ������θ� �����ؾ� �Ѵ�.
	void OnReqUpdateQuote(nlohmann::json& obj);
	// ȣ���� ������Ʈ �Ѵ�. ��ο��� ������.
	void OnReqUpdateHoga(nlohmann::json& obj);
	// ��Ʈ������ ������Ʈ ��û
	void OnReqUpdateChartData(nlohmann::json& obj);
	void OnReqRegisterUser(nlohmann::json& obj);
	void OnReqUnregisterUser(nlohmann::json& obj);
	void OnReqChartDataOneByOne(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnReqAccountList(nlohmann::json& obj);
	// ����Ȯ�� �ֹ����� ������.
	void OnReqAcceptedList(nlohmann::json& obj);
	// ü��� �ֹ����� ������.
	void OnReqFilledList(nlohmann::json& obj);
	// ��û�� �ֹ����� ������.
	void OnReqOrderList(nlohmann::json& obj);
	// ��û�� �����ǵ��� ������.
	void OnReqPositionList(nlohmann::json& obj);
	void SendResult(std::string user_id, SmProtocol protocol, int result_code, std::string result_msg);
	void SendResult(int session_id, SmProtocol protocol, int result_code, std::string result_msg);
	// ��û�� ����ڿ��Ը� ������.
	void OnReqResendChartDataOneByOne(nlohmann::json& obj);
	// ��ο��� ������.
	void OnReqCycleDataResendOneByOne(nlohmann::json& obj);
	void OnReqResetAccount(nlohmann::json& obj);
	void OnReqRecentHogaDataAll(nlohmann::json& obj);
};

