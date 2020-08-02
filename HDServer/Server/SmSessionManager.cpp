#include "SmSessionManager.h"
#include "SmWebSocketSession.h"
#include "../User/SmUserManager.h"
#include "../Service/SmRealtimeSymbolServiceManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../User/SmUser.h"
#include "../Service/SmTimeSeriesServiceManager.h"

SmSessionManager::
SmSessionManager(std::string doc_root)
	: doc_root_(std::move(doc_root))
{
	
}

void
SmSessionManager::
join(SmWebsocketSession* session)
{
	std::lock_guard<std::mutex> lock(mutex_);
	//sessions_.insert(session);
	int session_id = _SessionIDGen.GetID();
	session->SessionID(session_id);
	_session_map[session_id] = session;
}

void
SmSessionManager::
leave(SmWebsocketSession* session)
{
	std::lock_guard<std::mutex> lock(mutex_);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	// 사용자가 소유한 소켓에서 현재 소켓을 제외해 준다.
	userMgr->RemoveSocketFromUser(session);
	//sessions_.erase(session);
	auto it = _session_map.find(session->SessionID());
	if (it != _session_map.end()) {
		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		// Set the nullptr to the sise socket if this socket is the sise socket.
		tsMgr->ClearSiseSocket(session);
		_session_map.erase(it);
	}
}

// Broadcast a message to all websocket client sessions
void
SmSessionManager::
send(std::string message)
{
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		//v.reserve(sessions_.size());
		v.reserve(_session_map.size());
		//for (auto p : sessions_)
		//	v.emplace_back(p->weak_from_this());
		for (auto it = _session_map.begin(); it != _session_map.end(); ++it) {
			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			// 시세 전용 소켓이 있을 때
			if (tsMgr->GetSiseSocketCount() > 0) {
				// 시세 소켓과 다른 것들에게만 메시지를 보낸다.
				if (it->second->Type() == 0) {
					v.emplace_back(it->second->weak_from_this());
				}
			}
			else { // 시세 소켓이 없을 때
				// 모든 소켓에 메시지를 보낸다.
				v.emplace_back(it->second->weak_from_this());
			}
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock()) {		
			sp->send(ss);
		}

}

void SmSessionManager::send(int session_id, std::string message)
{
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		//v.reserve(sessions_.size());
		v.reserve(1);
		auto it = _session_map.find(session_id);
		if (it != _session_map.end()) {
			v.emplace_back(it->second->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock()) {
			sp->send(ss);
		}
}

SmSessionManager::~SmSessionManager()
{
	CloseAllSocket();
}

void SmSessionManager::AddUser(std::string id, std::string pwd, SmWebsocketSession* sess)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->AddUser(id, pwd, sess);
	SmRealtimeSymbolServiceManager* rtlSymSvcMgr = SmRealtimeSymbolServiceManager::GetInstance();
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol("CLN19");
	rtlSymSvcMgr->Symbol(sym);
	rtlSymSvcMgr->Register(user);
}

void SmSessionManager::DeleteUser(std::string id)
{
	SmRealtimeSymbolServiceManager* rtlSymSvcMgr = SmRealtimeSymbolServiceManager::GetInstance();
	rtlSymSvcMgr->Unregister(id);
}

void SmSessionManager::CloseAllSocket()
{
	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		v.reserve(_session_map.size());
		//for (auto p : sessions_)
		//	v.emplace_back(p->weak_from_this());
		for (auto it = _session_map.begin(); it != _session_map.end(); ++it) {
			v.emplace_back(it->second->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock()) {
			// Close the WebSocket connection
			sp->close_socket();
		}
	/*
	// WebSocket says that to close a connection you have
	// to keep reading messages until you receive a close frame.
	// Beast delivers the close frame as an error from read.
	//
	beast::drain_buffer drain; // Throws everything away efficiently
	for (;;)
	{
		// Keep reading messages...
		ws.read(drain, ec);

		// ...until we get the special error code
		if (ec == beast::websocket::error::closed)
			break;

		// Some other error occurred, report it and exit.
		if (ec)
			return fail("close", ec);
	}
	*/
}
