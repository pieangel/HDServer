#include "pch.h"
#include "SmAccountPositionManager.h"
#include "SmPosition.h"

SmAccountPositionManager::~SmAccountPositionManager()
{
}

std::shared_ptr<SmPosition> SmAccountPositionManager::FindPosition(std::string symbolCode)
{
	auto it = _PositionMap.find(symbolCode);
	if (it != _PositionMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmAccountPositionManager::AddPosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;
	_PositionMap[posi->SymbolCode] = posi;
}
