#pragma once
#include <string>
#include <vector>
#include <memory>
class SmProduct;
class SmMarket
{
public:
	SmMarket();
	~SmMarket();
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	/// <summary>
	/// 품목을 추가한다.
	/// </summary>
	/// <param name="code"></param>
	/// <returns></returns>
	std::shared_ptr<SmProduct> AddProduct(std::string code);
	std::shared_ptr<SmProduct> FindProduct(std::string code);
	std::shared_ptr<SmProduct> FindAddProduct(std::string code);
	std::vector<std::shared_ptr<SmProduct>>& GetCategoryList() {
		return _CategoryList;
	}
	int Index() const { return _Index; }
	void Index(int val) { _Index = val; }
	std::string MarketCode() const { return _MarketCode; }
	void MarketCode(std::string val) { _MarketCode = val; }
private:
	std::string _MarketCode;
	std::string _Name;
	int _Index;
	std::vector<std::shared_ptr<SmProduct>> _CategoryList;
};

