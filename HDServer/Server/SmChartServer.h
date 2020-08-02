#pragma once
#include <thread>
#include <vector>
#include "ServerDefine.h"
class SmChartServer
{
public:
	void Init();
	void Start();
	void Stop();

	SmChartServer() : _server_thread(&SmChartServer::ThreadMain, this) {}
	~SmChartServer() {
		_server_thread.join();
		if (_context) {
			delete _context;
			_context = nullptr;
		}
	}
private:
	std::thread _server_thread;
	void ThreadMain();

	net::io_context* _context = nullptr;
};

