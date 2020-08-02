#include "SmUserManager.h"
#include "SmUser.h"
#include "../Server/SmWebsocketSession.h"
#include "../Service/SmRealtimeSymbolServiceManager.h"
#include "../Database/SmTimeSeriesDBManager.h"
#include "../Json/json.hpp"
#include "../Database/SmMongoDBManager.h"
#include "../Service/SmServiceDefine.h"
#include "../Global/SmGlobal.h"
#include "../Server/SmSessionManager.h"
using namespace nlohmann;
SmUserManager::SmUserManager()
{

}

SmUserManager::~SmUserManager()
{
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto it = _UserMap.begin(); it != _UserMap.end(); ++it) {
		std::shared_ptr<SmUser> user = it->second;
		ClearAllService(user);
	}
}

std::set<int> SmUserManager::GetUserSocketList(std::string user_id)
{
	auto it = _UserMap.find(user_id);
	if (it != _UserMap.end()) {
		return it->second->GetSocketSet();
	} 
	else {
		std::set<int> empty_set;
		return empty_set;
	}
}

std::shared_ptr<SmUser> SmUserManager::AddUser(std::string id, SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (!socket)
		return nullptr;

	std::shared_ptr<SmUser> user = FindUser(id);
	if (!user) {
		user = std::make_shared<SmUser>();
	}
	user->Id(id);
	user->AddSocket(socket->SessionID());
	_UserMap[id] = user;
	_SocketToUserMap[socket->SessionID()] = user;
	return user;
}

std::shared_ptr<SmUser> SmUserManager::AddUser(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (!socket)
		return nullptr;
	std::shared_ptr<SmUser> user = FindUser(id);
	if (!user) {
		user = std::make_shared<SmUser>();
	}
	user->Id(id);
	user->Password(pwd);
	user->AddSocket(socket->SessionID());
	_UserMap[id] = user;
	_SocketToUserMap[socket->SessionID()] = user;
	return user;
}

std::pair<int, std::string> SmUserManager::CheckUserInfo(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	std::string result_msg = "";
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	auto user_info = mongoMgr->GetUserInfo(id);
	int result = 0;
	if (id.compare(user_info.first) != 0) {
		result_msg = "No user id is there!";
		result = -1;
	} 
	else if (pwd.compare(user_info.second) != 0) {
		result_msg = "Wrong password!";
		result = -2;
	} 
	else {
		result_msg = "Login success!";
	}
	return std::make_pair(result, result_msg);
}

void SmUserManager::AddUserToDatabase(std::string id, std::string pwd)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	dbMgr->AddUserToDatabase(id, pwd);
}

void SmUserManager::RemoveUser(std::string id)
{
	auto it = _UserMap.find(id);
	if (it != _UserMap.end()) {
		_UserMap.erase(it);
	}
}

void SmUserManager::ClearAllService(std::shared_ptr<SmUser> user)
{
	if (!user)
		return;

	SmRealtimeSymbolServiceManager* rtlSymSvcMgr = SmRealtimeSymbolServiceManager::GetInstance();
	rtlSymSvcMgr->UnregisterAllSymbol(user->Id());
}


void SmUserManager::SendLoginResult(std::string user_id, std::string msg)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_login;
	send_object["result_msg"] = msg;
	send_object["result_code"] = 0;
	std::string content = send_object.dump(4);
	SendResultMessage(user_id, content);
}

void SmUserManager::SendLoginResult(std::string user_id, std::string msg, int session_id)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_login;
	send_object["result_msg"] = msg;
	send_object["result_code"] = 0;
	std::string content = send_object.dump(4);

	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(session_id, content);
}

void SmUserManager::SendLogoutResult(std::string user_id)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_logout;
	send_object["result_msg"] = "Logout Success!";
	send_object["result_code"] = 0;
	std::string content = send_object.dump(4);
	SendResultMessage(user_id, content);
}

void SmUserManager::SendBroadcastMessage(std::string message)
{
	

}

std::shared_ptr<SmUser> SmUserManager::FindUser(std::string id)
{
	auto it = _UserMap.find(id);
	if (it != _UserMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmUserManager::RemoveSocketFromUser(SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (!socket)
		return;

	auto it = _SocketToUserMap.find(socket->SessionID());
	if (it != _SocketToUserMap.end()) {
		std::shared_ptr<SmUser> user = it->second;
		user->RemoveSocket(socket->SessionID());
	}
}

void SmUserManager::SendResultMessage(std::string user_id, std::string message)
{
	auto it = _UserMap.find(user_id);
	if (it != _UserMap.end()) {
		std::shared_ptr<SmUser> user = it->second;
		std::set<int> socket_set = user->GetSocketSet();
		// 같은 아이디를 가지고 접속한 모든 소켓에 동시에 메시지를 보낸다.
		for (auto it = socket_set.begin(); it != socket_set.end(); ++it) {
			int session_id = *it;
			std::shared_ptr<SmSessionManager> sessMgr = SmGlobal::GetInstance()->GetSessionManager();
			sessMgr->send(session_id, message);
		}
	}
}

void SmUserManager::Logout(std::string id)
{
	std::shared_ptr<SmUser> user = FindUser(id);
	if (!user)
		return;
}

bool SmUserManager::IsExistUser(std::string id)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string = "select \"id\", \"pwd\" from \"user_info\" where  \"id\" = \'" + id + "\'";

	std::string resp = dbMgr->ExecQuery(query_string);

	std::string result = resp;

	auto json_object = json::parse(resp);
	auto values = json_object["results"][0]["series"];
	if (values.is_null()) {
		return false;
	}

	values = json_object["results"][0]["series"][0]["values"];
	std::string str_id = values[0][1];
	
	if (str_id.compare(id) == 0)
		return true;
	else
		return false;
}

void SmUserManager::OnLogin(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	if (!socket)
		return;

	auto result = CheckUserInfo(id, pwd, socket);
	// 로그인이 성공하면 사용자 등록을 한다.
	// 등록된 사용자만 주문을 할 수 있다.
	if (result.first == 0) {
		AddUser(id, pwd, socket);
	}
	SendLoginResult(id, result.second, socket->SessionID());
}

void SmUserManager::OnLogout(std::string id)
{
	RemoveUser(id);
	SendLogoutResult(id);
}

int SmUserManager::GetSendBufferQueueSize(std::string user_id)
{
	return -1;
}
