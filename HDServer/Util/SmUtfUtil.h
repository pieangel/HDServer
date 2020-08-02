#pragma once
#include <string>
class SmUtfUtil
{
public:
	static std::string AnsiToUtf8(char* ansi);
	static std::string Utf8ToAnsi(char* utf8);
	static std::string unicode2ansi(const std::wstring& wstr);
	static std::wstring ansi2unicode(const std::string& str);
	static std::wstring utf8_decode(const std::string& str);
	static std::string utf8_encode(const std::wstring& wstr);
	static std::string Utf8ToAnsi(std::string utf8);
	std::wstring utf82ws(std::string& input);
	std::string ws2utf8(std::wstring& input);
};

