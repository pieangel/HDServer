#pragma once
#include "../Global/TemplateSingleton.h"
namespace Bosma
{
	class Scheduler;
};
class SmScheduler : public TemplateSingleton<SmScheduler>
{
public:
	SmScheduler();
	~SmScheduler();
	void RepeatSymbolService();
private:
	Bosma::Scheduler* _Scheduler = nullptr;
	void SendRealtimeSymbol();
};

