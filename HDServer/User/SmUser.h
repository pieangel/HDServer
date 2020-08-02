#pragma once
#include <string>
#include <set>
class SmWebsocketSession;
class SmUser
{
private:
	std::string _Id;
	std::string _Password;
	std::string _Cert;
	SmWebsocketSession* _Socket = nullptr;
	bool _Connected = false;
	std::set<int> _SocketSet;
public:
	std::set<int> GetSocketSet() {
		return _SocketSet;
	}
	void AddSocket(int session_id) {
		_SocketSet.insert(session_id);
	}
	void RemoveSocket(int session_id) {
		auto it = _SocketSet.find(session_id);
		if (it != _SocketSet.end()) {
			_SocketSet.erase(it);
		}
	}
	//void Reset();
	std::string Id() const { return _Id; }
	void Id(std::string val) { _Id = val; }
	std::string Password() const { return _Password; }
	void Password(std::string val) { _Password = val; }
	std::string Cert() const { return _Cert; }
	void Cert(std::string val) { _Cert = val; }
	bool Connected() const { return _Connected; }
	void Connected(bool val) { _Connected = val; }

};

