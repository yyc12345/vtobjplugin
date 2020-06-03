#include "vt_menu.h"
#include "export_setting.h"

extern PluginInterface* s_Plugininterface;
extern obj_export* exporter;
CMenu* s_MainMenu = NULL;


void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface) {
	switch (reason) {
		case PluginInfo::CR_LOAD:
		{
			s_Plugininterface = plugininterface;
			InitMenu();
			UpdateMenu();
		}break;
		case PluginInfo::CR_UNLOAD:
		{
			RemoveMenu();
			s_Plugininterface = NULL;
		}break;
		case PluginInfo::CR_NEWCOMPOSITIONNAME:
		{
		}break;
		case PluginInfo::CR_NOTIFICATION:
		{
		}break;
	}
}

void InitMenu() {
	if (!s_Plugininterface)
		return;

	s_MainMenu = s_Plugininterface->AddPluginMenu("Vtobj", 20, NULL, (VoidFunc1Param)PluginMenuCallback);
}

void RemoveMenu() {
	if (!s_Plugininterface || !s_MainMenu)
		return;

	s_Plugininterface->RemovePluginMenu(s_MainMenu);
}

void UpdateMenu() {
	s_Plugininterface->ClearPluginMenu(s_MainMenu);

	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 0, "Export object");
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, -1, NULL, TRUE);
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 1, "Report bug");
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 2, "About vtobjplugin");


	s_Plugininterface->UpdatePluginMenu(s_MainMenu);
}

void PluginMenuCallback(int commandID) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (commandID == 0) {
		CKContext* ctx = s_Plugininterface->GetCKContext();

		ExportSetting* es = new ExportSetting(ctx);
		if (es->DoModal() != IDOK) {
			delete es;
			return;
		}

		exporter->Ready4Export(ctx, es->res_settings);
		switch (es->res_settings->export_mode) {
			case EXPORTMODE_ALL:
				exporter->ExportAllWarpper();
				break;
			case EXPORTMODE_GROUP:
				exporter->ExportGroupWarpper();
				break;
			case EXPORTMODE_OBJECT:
				exporter->ExportObjectWarpper();
				break;
		}

		ctx->OutputToConsole("[vtobjplugin] Export OK!");
		delete es;
	} else if (commandID == 1)
		ShellExecute(NULL, "open", "https://github.com/yyc12345/vtobjplugin/issues", NULL, NULL, SW_SHOWNORMAL);
	else if (commandID == 2)
		AfxMessageBox("vtobjplugin - An OBJ export plugin for Virtools.\nUnder GPL v3 License.\nProject homepage: https://github.com/yyc12345/vtobjplugin", MB_ICONINFORMATION + MB_OK);
	else;

}
