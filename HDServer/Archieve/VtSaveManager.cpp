#include "pch.h"
#include "VtSaveManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <filesystem>
#include "../Config/SmConfigManager.h"
//#include "../Fund/VtFundManager.h"
//#include "../Account/VtAccountManager.h"
//#include "VtOrderDialogManager.h"
#include "../Symbol/SmSymbol.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "HdWindowManager.h"
#include "../Util/VtStringUtil.h"
#include "../File/path.h"
#include "../File/resolver.h"
#include <exception>
//#include "VtSystemGroupManager.h"
//#include "VtStrategyWndManager.h"
//#include "VtOutSystemManager.h"
#include "../Global/SmGlobal.h"
#include "../Util/cryptor.hpp"
#include "../Login/VtLoginManager.h"
#include "../Log/loguru.hpp"
#include "../Market/SmMarketManager.h"
//#include "../Account/VtAccount.h"
//#include "Format/format_string.h"

using namespace std;

VtSaveManager::VtSaveManager()
{
}


VtSaveManager::~VtSaveManager()
{
}

void VtSaveManager::SaveAccountListToXml()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string id = VtLoginManager::GetInstance()->ID;
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

		appPath.append(_T("\\"));
		appPath.append(VtStringUtil::getTimeStr());
		appPath.append(_T(".xml"));
		filesystem::path path1(appPath);
		if (!path1.exists()) {
			return;
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result) {
			return;
		}

		pugi::xml_node application = doc.child("application");

		// 	application.remove_child("login_info");
		// 	pugi::xml_node login_info = application.append_child("login_info");
		// 	VtLoginManager::GetInstance()->SaveToXml(login_info);

		application.remove_child("account_list");
		pugi::xml_node account_list = application.append_child("account_list");
		//VtAccountManager::GetInstance()->SaveToXml(account_list);

		doc.save_file(appPath.c_str());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSaveManager::SaveFundListToXml()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string id = VtLoginManager::GetInstance()->ID;
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

		appPath.append(_T("\\"));
		appPath.append(VtStringUtil::getTimeStr());
		appPath.append(_T(".xml"));
		filesystem::path path1(appPath);
		if (!path1.exists()) {
			return;
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result) {
			return;
		}

		pugi::xml_node application = doc.child("application");

		application.remove_child("fund_list");
		pugi::xml_node fund_list = application.append_child("fund_list");
		//VtFundManager::GetInstance()->SaveToXml(fund_list);

		doc.save_file(appPath.c_str());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSaveManager::WriteSettings()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string id = VtLoginManager::GetInstance()->ID;
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

		appPath.append(_T("\\"));
		appPath.append(VtStringUtil::getTimeStr());
		appPath.append(_T(".xml"));
		filesystem::path path1(appPath);
		if (!path1.exists()) {
			/// [create new xml file]
			// Generate new XML document within memory
			pugi::xml_document doc;
			// Alternatively store as shared pointer if tree shall be used for longer
			// time or multiple client calls:
			// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
			// Generate XML declaration
			auto declarationNode = doc.append_child(pugi::node_declaration);
			declarationNode.append_attribute("version") = "1.0";
			declarationNode.append_attribute("encoding") = "euc-kr";
			declarationNode.append_attribute("standalone") = "yes";
			// A valid XML doc must contain a single root node of any name
			auto root = doc.append_child("application");
			// Save XML tree to file.
			// Remark: second optional param is indent string to be used;
			// default indentation is tab character.
			bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
			//assert(saveSucceeded);
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");

		// 	application.remove_child("login_info");
		// 	pugi::xml_node login_info = application.append_child("login_info");
		// 	VtLoginManager::GetInstance()->SaveToXml(login_info);

		application.remove_child("account_list");
		pugi::xml_node account_list = application.append_child("account_list");
		//VtAccountManager::GetInstance()->SaveToXml(account_list);

		application.remove_child("fund_list");
		pugi::xml_node fund_list = application.append_child("fund_list");
		//VtFundManager::GetInstance()->SaveToXml(fund_list);

		/*
		application.remove_child("external_system_list");
		pugi::xml_node external_system_list = application.append_child("external_system_list");
		VtOutSystemManager::GetInstance()->SaveToXml(external_system_list);

		application.remove_child("system_group_list");
		pugi::xml_node system_group_list = application.append_child("system_group_list");
		VtSystemGroupManager::GetInstance()->SaveToXml(system_group_list);

		application.remove_child("order_window_list");
		pugi::xml_node window_list = application.append_child("order_window_list");
		VtOrderDialogManager::GetInstance()->SaveToXml(window_list);

		application.remove_child("stratege_window_list");
		pugi::xml_node stratege_window_list = application.append_child("stratege_window_list");
		VtStrategyWndManager::GetInstance()->SaveToXml(stratege_window_list);

		application.remove_child("etc_window_list");
		pugi::xml_node etc_window_list = application.append_child("etc_window_list");
		HdWindowManager::GetInstance()->SaveToXml(etc_window_list);
		*/

		doc.save_file(appPath.c_str());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSaveManager::ReadAccountsAndFunds()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string id = SmGlobal::LoginUserID;
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		appPath.append(_T("\\"));
		std::string config_path = appPath;
		appPath.append(_T("*.*"));
		std::map<std::string, std::string> file_list;
		ListContents(file_list, config_path, "*.xml", false);

		if (file_list.size() == 0)
			return;

		std::string file_name = file_list.rbegin()->second;
		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");
		// 	pugi::xml_node login_info = application.child("login_info");
		// 	if (login_info) {
		// 		VtLoginManager::GetInstance()->LoadFromXml(login_info);
		// 	}
		pugi::xml_node account_list = application.child("account_list");
// 		if (account_list) {
// 			VtAccountManager::GetInstance()->LoadFromXml(account_list);
// 		}

		pugi::xml_node fund_list = application.child("fund_list");
// 		if (fund_list) {
// 			VtFundManager::GetInstance()->LoadFromXml(fund_list);
// 		}

		/*
		pugi::xml_node order_window_list = application.child("order_window_list");
		if (order_window_list) {
		VtOrderDialogManager::GetInstance()->LoadFromXml(order_window_list);
		}

		pugi::xml_node stratege_window_list = application.child("stratege_window_list");
		if (stratege_window_list) {
		VtStrategyWndManager::GetInstance()->LoadFromXml(stratege_window_list);
		}

		pugi::xml_node etc_window_list = application.child("etc_window_list");
		if (etc_window_list) {
		HdWindowManager::GetInstance()->LoadFromXml(etc_window_list);
		}
		*/

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

bool VtSaveManager::ListContents(std::map<std::string, std::string>& dest, std::string dir, std::string filter, bool recursively)
{
	try {
		WIN32_FIND_DATAA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;

		// Prepare string
		//if (dir.back() != '\\') dir += "\\";

		// Safety check
		if (dir.length() >= MAX_PATH) {
			LOG_F(INFO, "Cannot open folder %s: path too long", dir.c_str());
			return false;
		}

		// First entry in directory
		hFind = FindFirstFileA((dir + filter).c_str(), &ffd);

		if (hFind == INVALID_HANDLE_VALUE) {
			LOG_F(INFO, "Cannot open folder in folder %s: error accessing first entry.", dir.c_str());
			return false;
		}

		// List files in directory
		do {
			// Ignore . and .. folders, they cause stack overflow
			if (strcmp(ffd.cFileName, ".") == 0) continue;
			if (strcmp(ffd.cFileName, "..") == 0) continue;


			// Is directory?
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Go inside recursively
				//if (recursively)
				//	ListContents(dest, dir + ffd.cFileName, filter, recursively, content_type);
				continue;
			}
			// Add file to our list
			else {

				SYSTEMTIME stLocal;

				// Convert the last-write time to local time.
				FileTimeToSystemTime(&ffd.ftLastWriteTime, &stLocal);
				std::string local_time;
				local_time += std::to_string(stLocal.wYear);
				local_time += std::to_string(stLocal.wMonth);
				local_time += std::to_string(stLocal.wDay);
				CString local_file_time;
				local_file_time.Format("%04d%02d%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);
				dest[(LPCTSTR)local_file_time] = dir + ffd.cFileName;
			}

		} while (FindNextFileA(hFind, &ffd));

		// Get last error
		dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) {
			LOG_F(INFO, "Error reading file list in folder %s.", dir.c_str());
			return false;
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}

	return true;
}

void VtSaveManager::ReadWindows()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string id = VtLoginManager::GetInstance()->ID;
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;
		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		appPath.append(_T("\\"));
		std::string config_path = appPath;
		appPath.append(_T("*.*"));
		std::map<std::string, std::string> file_list;
		ListContents(file_list, config_path, "*.xml", false);

		if (file_list.size() == 0)
			return;

		std::string file_name = file_list.rbegin()->second;
		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");

		/*
		pugi::xml_node system_group_list = application.child("system_group_list");
		if (system_group_list) {
			VtSystemGroupManager::GetInstance()->LoadFromXml(system_group_list);
		}


		pugi::xml_node external_system_list = application.child("external_system_list");
		if (external_system_list) {
			VtOutSystemManager::GetInstance()->LoadFromXml(external_system_list);
		}

		pugi::xml_node order_window_list = application.child("order_window_list");
		if (order_window_list) {
			VtOrderDialogManager::GetInstance()->LoadFromXml(order_window_list);
		}

		pugi::xml_node stratege_window_list = application.child("stratege_window_list");
		if (stratege_window_list) {
			VtStrategyWndManager::GetInstance()->LoadFromXml(stratege_window_list);
		}

		pugi::xml_node etc_window_list = application.child("etc_window_list");
		if (etc_window_list) {
			HdWindowManager::GetInstance()->LoadFromXml(etc_window_list);
		}
		*/

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

std::string VtSaveManager::GetLastestFile(std::string file_name)
{
	FILETIME bestDate = { 0, 0 };
	FILETIME curDate;
	string name = "";
	CFileFind finder;

	finder.FindFile(file_name.c_str());
	while (finder.FindNextFile())
	{
		finder.GetCreationTime(&curDate);

		if (CompareFileTime(&curDate, &bestDate) > 0)
		{
			bestDate = curDate;
			name = finder.GetFileName().GetString();
			AfxMessageBox(name.c_str());
		}
	}
	return name;
}

void VtSaveManager::SaveOrders()
{
	SaveOrders(_T("orderlist.dat"));
}

void VtSaveManager::SaveOrders(std::string fileName)
{
	
}

void VtSaveManager::SaveFundList(std::string fileName)
{

}

void VtSaveManager::SaveFundList()
{
	SaveFundList(_T("fundlist.dat"));
}

void VtSaveManager::LoadFundList(std::string fileName)
{
	
}

void VtSaveManager::SaveAccountList(std::string fileName)
{
	
}

void VtSaveManager::SaveAccountList()
{
	SaveAccountList(_T("accountlist.dat"));
}

void VtSaveManager::LoadAccountList(std::string fileName)
{
	
}

void VtSaveManager::SaveOrderWndList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void VtSaveManager::LoadOrderWndList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void VtSaveManager::SaveDialogList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void VtSaveManager::LoadDialogList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void VtSaveManager::SaveTotal(std::string fileName, CMainFrame* mainFrm)
{
	
}

void VtSaveManager::SaveLoginInfo(std::string fileName, std::string id, std::string pwd, std::string cert, bool save)
{
	
}

void VtSaveManager::LoadLoginInfo(std::string fileName, std::string& id, std::string& pwd, std::string& cert, bool& save)
{
	
}

void VtSaveManager::SaveSystems(std::string fileName)
{
	
}

void VtSaveManager::LoadSystems(std::string fileName)
{
	
}

void VtSaveManager::SaveOutSystems(std::string fileName)
{
	
}

void VtSaveManager::LoadOutSystems(std::string fileName)
{
	
}

void VtSaveManager::SaveOutSignal(std::string fileName)
{

}

void VtSaveManager::LoadOutSignal(std::string fileName)
{

}

void VtSaveManager::CreateUserDirectory()
{
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetApplicationPath();
	appPath.append(_T("\\"));
	appPath.append(SmGlobal::LoginUserID);
	filesystem::path path1(appPath);
	if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
		filesystem::create_directory(path1);
	}
}

bool VtSaveManager::IsAccountFileExist()
{
	return false;
}

void VtSaveManager::SaveLoginInfoToXml(std::string id, std::string pwd, std::string cert, bool save)
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("config.xml"));

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
			return;
		}

		pugi::xml_node application = doc.child("application");
		application.remove_child("login_info");
		pugi::xml_node login_info = application.append_child("login_info");
		login_info.append_attribute("save") = save;

		auto enc_id = cryptor::encrypt(move(id));
		auto enc_pwd = cryptor::encrypt(move(pwd));
		auto enc_cert = cryptor::encrypt(move(cert));

		pugi::xml_node login_info_child = login_info.append_child("login_id");
		login_info_child.append_child(pugi::node_pcdata).set_value(enc_id.c_str());

		login_info_child = login_info.append_child("login_password");
		login_info_child.append_child(pugi::node_pcdata).set_value(enc_pwd.c_str());

		login_info_child = login_info.append_child("login_cert");
		login_info_child.append_child(pugi::node_pcdata).set_value(enc_cert.c_str());

		doc.save_file(appPath.c_str());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

int VtSaveManager::LoadLoginInfoFromXml(std::string& id, std::string& pwd, std::string& cert, bool& save)
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("config.xml"));

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
			return -1;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node login_info = application.child("login_info");
		if (login_info) {
			bool save_value = login_info.attribute("save").as_bool();
			save_value == 1 ? save = true : save = false;
			id = login_info.child_value("login_id");
			pwd = login_info.child_value("login_password");
			cert = login_info.child_value("login_cert");
			return 1;
		}
		else {
			return -1;
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}

	return -1;
}

int VtSaveManager::LoadLoginInfoFromXml()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("config.xml"));

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			// 설정 파일이 없을 때
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
			return -1;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node login_info = application.child("login_info");
		if (login_info) {
			VtLoginManager* loginMgr = VtLoginManager::GetInstance();
			loginMgr->Save = login_info.attribute("save").as_bool();
			std::string id = login_info.child_value("login_id");
			std::string pwd = login_info.child_value("login_password");
			std::string cert = login_info.child_value("login_cert");

			loginMgr->ID = cryptor::decrypt(id);
			loginMgr->Password = cryptor::decrypt(pwd);
			loginMgr->Cert = cryptor::decrypt(cert);
			return 1;
		}
		else {
			// 설정 파일에서 로그인 항목이 없을 때
			return -1;
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}

	return -1;
}

void VtSaveManager::LoadRunInfoFromXml()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("config.xml"));

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			// 설정 파일이 없을 때
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
			return;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node run_info = application.child("run_info");
// 		if (run_info) {
// 			VtGlobal::OpenTime.hour = std::stoi(run_info.child_value("start_hour"));
// 			VtGlobal::OpenTime.min = std::stoi(run_info.child_value("start_min"));
// 			VtGlobal::OpenTime.sec = std::stoi(run_info.child_value("start_sec"));
// 			VtGlobal::CloseTime.hour = std::stoi(run_info.child_value("end_hour"));
// 			VtGlobal::CloseTime.min = std::stoi(run_info.child_value("end_min"));
// 			VtGlobal::CloseTime.sec = std::stoi(run_info.child_value("end_sec"));
// 
// 			VtGlobal::ServerSleepTime = std::stoi(run_info.child_value("server_sleep_time"));
// 			std::string default_abroad_product_code = run_info.child_value("default_abroad_product_code");
// 			SmMarketManager::GetInstance()->DefaultAbroadProductCode(default_abroad_product_code);
// 			std::string default_abroad_market_name = run_info.child_value("default_abroad_market_name");
// 			SmMarketManager::GetInstance()->DefaultAbroadMarketName(default_abroad_market_name);
// 		}
// 
// 		pugi::xml_node file_watch = application.child("file_watch");
// 		if (file_watch) {
// 			VtGlobal::GetInstance()->EnableFileWatch = file_watch.attribute("enable").as_bool();
// 			VtGlobal::GetInstance()->FileWatchPath = file_watch.child_value("file_watch_path");
// 		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSaveManager::GetWindowSymbolList(std::set<std::string>& symbol_list)
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();
		std::string id = VtLoginManager::GetInstance()->ID;
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		appPath.append(_T("\\"));
		std::string config_path = appPath;
		appPath.append(_T("*.*"));
		std::map<std::string, std::string> file_list;
		ListContents(file_list, config_path, "*.xml", false);

		if (file_list.size() == 0)
			return;

		std::string file_name = file_list.rbegin()->second;

		pugi::xml_document doc;

		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");

		pugi::xml_node order_window_list_node = application.child("order_window_list");
		if (!order_window_list_node)
			return;

		for (pugi::xml_node order_window_node = order_window_list_node.child("order_window"); order_window_node; order_window_node = order_window_node.next_sibling("order_window")) {
			pugi::xml_node center_window_list_node = order_window_node.child("center_window_list");
			if (center_window_list_node) {
				for (pugi::xml_node center_window_node = center_window_list_node.child("center_window"); center_window_node; center_window_node = center_window_node.next_sibling("center_window")) {
					std::string symbol_code = center_window_node.child_value("symbol_code");
					symbol_list.insert(symbol_code);
				}
			}
		}


	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSaveManager::SaveAccountPasswords(std::map<std::pair<int, int>, std::pair<VtAccount*, std::string>>& cell_account_map)
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("acnt_pwd.xml"));
		filesystem::path path1(appPath);
		if (!path1.exists()) {
			/// [create new xml file]
			// Generate new XML document within memory
			pugi::xml_document doc;
			// Alternatively store as shared pointer if tree shall be used for longer
			// time or multiple client calls:
			// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
			// Generate XML declaration
			auto declarationNode = doc.append_child(pugi::node_declaration);
			declarationNode.append_attribute("version") = "1.0";
			declarationNode.append_attribute("encoding") = "euc-kr";
			declarationNode.append_attribute("standalone") = "yes";
			// A valid XML doc must contain a single root node of any name
			auto root = doc.append_child("application");
			// Save XML tree to file.
			// Remark: second optional param is indent string to be used;
			// default indentation is tab character.
			bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
			//assert(saveSucceeded);
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");

		application.remove_child("account_list");
		pugi::xml_node account_list = application.append_child("account_list");


// 		for (auto it = cell_account_map.begin(); it != cell_account_map.end(); ++it) {
// 			std::pair<VtAccount*, std::string>& item = it->second;
// 			// 계좌가 있고 활성화 되어 있을 때만 비번을 저장한다.
// 			if (item.first && item.first->Enable()) {
// 				item.first->Password = item.second;
// 
// 				auto enc_AccountNo = cryptor::encrypt(item.first->AccountNo);
// 				auto enc_Password = cryptor::encrypt(item.first->Password);
// 
// 				pugi::xml_node account_node = account_list.append_child("account");
// 
// 				pugi::xml_node child_node = account_node.append_child("account_no");
// 				child_node.append_child(pugi::node_pcdata).set_value(enc_AccountNo.c_str());
// 				child_node = account_node.append_child("account_pwd");
// 				child_node.append_child(pugi::node_pcdata).set_value(enc_Password.c_str());
// 
// 
// 			}
// 		}


		doc.save_file(appPath.c_str());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}


void VtSaveManager::LoadAccountPasswords()
{
	try {
		SmConfigManager* configMgr = SmConfigManager::GetInstance();

		std::string appPath;
		appPath = configMgr->GetApplicationPath();
		appPath.append(_T("\\"));

		appPath.append(_T("acnt_pwd.xml"));


		std::string file_name = appPath;
		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node account_list_node = application.child("account_list");
// 		if (account_list_node) {
// 			for (pugi::xml_node account_node = account_list_node.child("account"); account_node; account_node = account_node.next_sibling("account")) {
// 				std::string dec_AccountNo, dec_Password;
// 				dec_AccountNo = account_node.child_value("account_no");
// 				dec_Password = account_node.child_value("account_pwd");
// 
// 				std::string AccountNo = cryptor::decrypt(dec_AccountNo);
// 				std::string Password = cryptor::decrypt(dec_Password);
// 				VtAccountManager* acntMgr = VtAccountManager::GetInstance();
// 				VtAccount* acnt = acntMgr->FindAccount(AccountNo);
// 				if (acnt) {
// 					// 계좌에 비밀번호를 넣어준다.
// 					acnt->Password = Password;
// 				}
// 				// 비밀번호 맵에 저장한다.
// 				acntMgr->AddPassword(AccountNo, Password);
// 			}
// 		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSaveManager::GetSymbolMasters()
{
	for (auto it = _SymbolVector.begin(); it != _SymbolVector.end(); ++it) {
		SmSymbol* sym = *it;
		//sym->GetSymbolMaster();
	}
}
