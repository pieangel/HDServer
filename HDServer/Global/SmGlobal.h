#pragma once
#include "../Global/TemplateSingleton.h"
class SmSessionManager;
class SmGlobal : public TemplateSingleton<SmGlobal>
{
public:
	void SetSessionManager(std::shared_ptr<SmSessionManager> session_mgr) {
		session_mgr_ = session_mgr;
	}
	std::shared_ptr<SmSessionManager> GetSessionManager() {
		return session_mgr_;
	}
private:
	std::shared_ptr<SmSessionManager> session_mgr_;
};

