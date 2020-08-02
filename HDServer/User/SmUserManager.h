#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <map>
#include <memory>
#include <set>
#include "../Global/TemplateSingleton.h"
class SmUser;
class SmWebsocketSession;
class SmUserManager : public TemplateSingleton<SmUserManager>
{
	// This mutex synchronizes all access to sessions_
	std::mutex _mutex;

	// Keep a list of all the connected clients
	std::map<std::string, std::shared_ptr<SmUser>> _UserMap;
	/// <summary>
	/// Session ID, User pointer.
	/// </summary>
	std::map<int, std::shared_ptr<SmUser>> _SocketToUserMap;
public:
	SmUserManager();
	~SmUserManager();
	std::set<int> GetUserSocketList(std::string user_id);
	std::shared_ptr<SmUser> AddUser(std::string id, SmWebsocketSession* socket);
	std::shared_ptr<SmUser> AddUser(std::string id, std::string pwd, SmWebsocketSession* socket);
	std::pair<int, std::string> CheckUserInfo(std::string id, std::string pwd, SmWebsocketSession* socket);
	void SendBroadcastMessage(std::string message);
	std::shared_ptr<SmUser> FindUser(std::string id);
	void RemoveSocketFromUser(SmWebsocketSession* socket);
	void SendResultMessage(std::string user_id, std::string message);
	void Logout(std::string id);
	bool IsExistUser(std::string id);
	void OnLogin(std::string id, std::string pwd, SmWebsocketSession* socket);
	void OnLogout(std::string id);
	int GetSendBufferQueueSize(std::string user_id);
private:
	void AddUserToDatabase(std::string id, std::string pwd);
	void RemoveUser(std::string id);
	void ClearAllService(std::shared_ptr<SmUser> user);
	void SendLoginResult(std::string user_id, std::string msg);
	void SendLoginResult(std::string user_id, std::string msg, int session_id);
	void SendLogoutResult(std::string user_id);
};

