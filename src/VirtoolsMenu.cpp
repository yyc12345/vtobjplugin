#pragma once
#include "VirtoolsMenu.hpp"
#include "Utilities.hpp"

namespace vtobjplugin::VirtoolsMenu {

	PluginInterface* g_Plugininterface = nullptr;
	PluginInfo g_PluginInfo;
	CMenu* g_MainMenu = nullptr;

	static void PluginMenuCallback(int command_id) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		auto& string_loader = Utilities::StringLoader::GetSingleton();

		if (command_id == 0) {
			CKContext* ctx = g_Plugininterface->GetCKContext();
			AfxMessageBox("Work in Progress...", MB_ICONINFORMATION + MB_OK);

			ctx->OutputToConsole("[vtobjplugin] Export OK!");
		} else if (command_id == 1) {
			ShellExecuteW(NULL, L"open", L"https://github.com/yyc12345/vtobjplugin/issues", NULL, NULL, SW_SHOWNORMAL);
		} else if (command_id == 2) {
			MessageBoxW(
				nullptr,
				L"vtobjplugin v3.0 - A Wavefront OBJ Exporter for Virtools.\nUnder GPL v3 License.\nProject Home Page: https://github.com/yyc12345/vtobjplugin",
				L"About vtobjplugin",
				MB_ICONINFORMATION + MB_OK
			);
		}

	}

	static void InitMenu() {
		if (!g_Plugininterface)
			return;

		g_MainMenu = g_Plugininterface->AddPluginMenu("Wavefront OBJ", 20, NULL, (VoidFunc1Param)PluginMenuCallback);
	}

	static void RemoveMenu() {
		if (!g_Plugininterface || !g_MainMenu)
			return;
		g_Plugininterface->RemovePluginMenu(g_MainMenu);
	}

	static void UpdateMenu() {
		g_Plugininterface->ClearPluginMenu(g_MainMenu);

		auto& string_loader = Utilities::StringLoader::GetSingleton();
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 0, string_loader.LoadStringA(IDS_VTOBJPLG__MENU__EXPORT_OBJECTS, "Export Objects"));
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, -1, NULL, TRUE);
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 1, string_loader.LoadStringA(IDS_VTOBJPLG__MENU__REPORT_BUG, "Report Bug"));
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 2, string_loader.LoadStringA(IDS_VTOBJPLG__MENU__ABOUT_VTOBJPLUGIN, "About vtobjplugin"));


		g_Plugininterface->UpdatePluginMenu(g_MainMenu);
	}

	static void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface) {
		switch (reason) {
			case PluginInfo::CR_LOAD:
			{
				g_Plugininterface = plugininterface;
				InitMenu();
				UpdateMenu();
				break;
			}
			case PluginInfo::CR_UNLOAD:
			{
				RemoveMenu();
				g_Plugininterface = nullptr;
				break;
			}
			case PluginInfo::CR_NEWCOMPOSITIONNAME:
				break;
			case PluginInfo::CR_NOTIFICATION:
				break;
		}
	}

	void InitializePluginInfo() {
		strcpy(g_PluginInfo.m_Name, "vtobjplugin");
		g_PluginInfo.m_PluginType = PluginInfo::PT_EDITOR;
		g_PluginInfo.m_PluginType = (PluginInfo::PLUGIN_TYPE)(g_PluginInfo.m_PluginType | PluginInfo::PTF_RECEIVENOTIFICATION);
		g_PluginInfo.m_PluginCallback = PluginCallback;
	}

	PluginInfo* GetPluginInfo() {
		return &g_PluginInfo;
	}
	PluginInterface* GetPluginInterface() {
		return g_Plugininterface;
	}


}
