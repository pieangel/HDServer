#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>

class SmSymbolReader : public TemplateSingleton<SmSymbolReader>
{
public:
	SmSymbolReader();
	~SmSymbolReader();

public:
	std::string GetWorkingDir();
	void ReadSymbolFromFile(int index, std::string fullPath);
private:
	void ReadMarketFile(std::string fullPath);
	void ReadPmFile(std::string fullPath);
	void ReadJmFile(std::string fullPath);
	void ReadMarketFile();
	void ReadPmFile();
	void ReadJmFile();
};

