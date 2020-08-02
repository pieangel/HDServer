#include "pch.h"
#include "afxwin.h"
#include "SmUtfUtil.h"
#include <codecvt>
#include <locale>

std::string SmUtfUtil::AnsiToUtf8(char* ansi)
{
	WCHAR unicode[1500];
	char utf8[1500];

	memset(unicode, 0, sizeof(unicode));
	memset(utf8, 0, sizeof(utf8));

	::MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode, sizeof(unicode));
	::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, sizeof(utf8), NULL, NULL);

	return std::string(utf8);
}

std::string SmUtfUtil::Utf8ToAnsi(char* utf8)
{
	WCHAR* unicode = NULL;
	char* ansi = NULL;

	unicode = (WCHAR*)calloc(1, strlen(utf8) * 2);
	::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, strlen(utf8) * 2);

	//ansi = (char*)calloc(1, _tcslen(unicode)*2);
	ansi = (char*)calloc(1, wcslen(unicode) * 2);  /* 테스트한 프로젝트의 문자 집합 속성이 MultiByte 이므로 _tcslen()은 strlen()으로 정의된다. strlen()은 char*을 인자로 받으므로 컴파일 오류가 생긴다. */
	//::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, _tcslen(unicode)*2, NULL, NULL); 
	::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, wcslen(unicode) * 2, NULL, NULL);

	return std::string(ansi);
}

std::string SmUtfUtil::Utf8ToAnsi(std::string utf8)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
	std::wstring cv = utf8conv.from_bytes(utf8);
	std::string value = SmUtfUtil::unicode2ansi(cv);
	return value;
}

std::wstring SmUtfUtil::utf82ws(std::string& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
	return utf8conv.from_bytes(input);
}

std::string SmUtfUtil::ws2utf8(std::wstring& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
	return utf8conv.to_bytes(input);
}

std::string SmUtfUtil::unicode2ansi(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}


std::string SmUtfUtil::utf8_encode(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring SmUtfUtil::utf8_decode(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

// Convert an ANSI string to a wide Unicode String
std::wstring SmUtfUtil::ansi2unicode(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}
