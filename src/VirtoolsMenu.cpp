#pragma once
#include "VirtoolsMenu.hpp"
#include "Utilities.hpp"
#include "ExportSettingDialog.hpp"
#include "ExportLayoutWeaver.hpp"
#include "ObjExporter.hpp"
#include <memory>

namespace vtobjplugin::VirtoolsMenu {

	PluginInterface* g_Plugininterface = nullptr;
	PluginInfo g_PluginInfo;
	CMenu* g_MainMenu = nullptr;

	static void PluginMenuCallback(int command_id) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		auto& string_loader = Utilities::StringLoader::GetSingleton();

		switch (command_id) {
			case 0:
			{
				// open export setting dialog
				CKContext* ctx = g_Plugininterface->GetCKContext();
				auto export_setting_dialog = std::make_unique<ExportSettingDialog>(ctx, nullptr);
				if (export_setting_dialog->DoModal() != IDOK) break;
				{
					// create reporter
					Utilities::VirtoolsUIReporter reporter(ctx);
					// build layout
					const DataTypes::ExportSetting export_setting = export_setting_dialog->GetExportSettingResult();
					ExportLayoutWeaver export_layout_weaver(g_Plugininterface, export_setting, reporter);
					export_layout_weaver.WeaveLayout();

					// call final exporter
					ObjExporter obj_exporter(export_setting, export_layout_weaver, reporter);
					obj_exporter.DoExport();
				}
				
				auto success_note = string_loader.LoadStringA(IDS_VTMENU_SUCCESS_NOTE);
				ctx->OutputToInfo(const_cast<CKSTRING>(success_note.c_str()));
				ctx->OutputToConsole(const_cast<CKSTRING>(success_note.c_str()));
				break;
			}
			case 1:
				ShellExecuteW(NULL, L"open", L"https://github.com/yyc12345/vtobjplugin/issues", NULL, NULL, SW_SHOWNORMAL);
				break;
			case 2:
				MessageBoxW(
					nullptr,
					string_loader.LoadStringW(IDS_VTMENU_ABOUT_BODY).c_str(),
					string_loader.LoadStringW(IDS_VTMENU_ABOUT_TITLE).c_str(),
					MB_ICONINFORMATION + MB_OK
				);
				break;
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
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 0, string_loader.LoadStringA(IDS_VTMENU_MENU_EXPORT_OBJECTS).c_str());
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, -1, NULL, TRUE);
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 1, string_loader.LoadStringA(IDS_VTMENU_MENU_REPORT_BUG).c_str());
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 2, string_loader.LoadStringA(IDS_VTMENU_MENU_ABOUT).c_str());


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
