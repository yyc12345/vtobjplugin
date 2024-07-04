#include "stdafx.hpp"
#include "VirtoolsMenu.hpp"

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
		// init plugin info first
		vtobjplugin::VirtoolsMenu::InitializePluginInfo();
		return CWinApp::InitInstance();
	}
	virtual int ExitInstance() override {
		return CWinApp::ExitInstance();
	}
};
CVtObjPlugin theApp;
