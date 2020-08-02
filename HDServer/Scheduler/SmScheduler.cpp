#include "SmScheduler.h"
#include "../Scheduler/Scheduler.h"
#include <chrono>
#include <iomanip>
#include "../Service/SmRealtimeSymbolServiceManager.h"
SmScheduler::SmScheduler()
{
	_Scheduler = new Bosma::Scheduler(12);
}

SmScheduler::~SmScheduler()
{
	if (_Scheduler) {
		delete _Scheduler;
		_Scheduler = nullptr;
	}
}

void SmScheduler::RepeatSymbolService()
{
	std::chrono::milliseconds milSec(1000);
	_Scheduler->every(milSec, [this]() { this->SendRealtimeSymbol(); });
}

void SmScheduler::SendRealtimeSymbol()
{
	SmRealtimeSymbolServiceManager* rtlSymServiceMgr = SmRealtimeSymbolServiceManager::GetInstance();
	//rtlSymServiceMgr->BroadcastSise();
	//rtlSymServiceMgr->BroadcastHoga();
}
