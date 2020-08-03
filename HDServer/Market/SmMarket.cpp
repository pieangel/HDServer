#include "pch.h"
#include "SmMarket.h"
#include "SmProduct.h"

SmMarket::SmMarket()
{
}


SmMarket::~SmMarket()
{
// 	for (auto it = _CategoryList.begin(); it != _CategoryList.end(); ++it) {
// 		delete* it;
// 	}
}

std::shared_ptr<SmProduct> SmMarket::AddProduct(std::string code)
{
	std::shared_ptr<SmProduct> cat = std::make_shared<SmProduct>();
	cat->Code(code);
	_CategoryList.push_back(cat);
	return cat;
}

std::shared_ptr<SmProduct> SmMarket::FindProduct(std::string code)
{
	for (auto it = _CategoryList.begin(); it != _CategoryList.end(); ++it) {
		std::shared_ptr<SmProduct> cat = *it;
		if (cat->Code().compare(code) == 0) {
			return cat;
		}
	}

	return nullptr;
}
