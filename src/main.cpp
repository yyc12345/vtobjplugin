#include "stdafx.hpp"
#include "VirtoolsMenu.hpp"
#include "ConfigManager.hpp"

int GetVirtoolsPluginInfoCount() {
	return 1;
}

PluginInfo* GetVirtoolsPluginInfo(int index) {
	switch (index) {
		case 0:
			return vtobjplugin::VirtoolsMenu::GetPluginInfo();
		default:
			return nullptr;
	}
}

class CVtObjPlugin : CWinApp {
public:
	virtual BOOL InitInstance() override {
		// register unhandler exception handler
#ifdef VTOBJ_RELEASE
		YYCC::ExceptionHelper::Register(vtobjplugin::VirtoolsMenu::UnhandledExceptionCallback);
#endif

		// load config from file
		auto& config_manager = vtobjplugin::ConfigManager::GetSingleton();
		config_manager.m_CoreManager.Load();

		// init plugin info
		vtobjplugin::VirtoolsMenu::InitializePluginInfo();

		return CWinApp::InitInstance();
	}
	virtual int ExitInstance() override {
		// save config to file
		auto& config_manager = vtobjplugin::ConfigManager::GetSingleton();
		config_manager.m_CoreManager.Save();

		// unregister unhandler exception handler
#ifdef VTOBJ_RELEASE
		YYCC::ExceptionHelper::Unregister();
#endif

		return CWinApp::ExitInstance();
	}
};
CVtObjPlugin theApp;
