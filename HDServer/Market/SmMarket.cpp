#include "pch.h"
#include "SmMarket.h"
#include "SmCategory.h"

SmMarket::SmMarket()
{
}


SmMarket::~SmMarket()
{
	for (auto it = _CategoryList.begin(); it != _CategoryList.end(); ++it) {
		delete* it;
	}
}

SmCategory* SmMarket::AddCategory(std::string code)
{
	SmCategory* cat = new SmCategory();
	cat->Code(code);
	_CategoryList.push_back(cat);
	return cat;
}

SmCategory* SmMarket::FindCategory(std::string code)
{
	for (auto it = _CategoryList.begin(); it != _CategoryList.end(); ++it) {
		SmCategory* cat = *it;
		if (cat->Code().compare(code) == 0) {
			return cat;
		}
	}

	return nullptr;
}
