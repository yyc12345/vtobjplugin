#pragma once
#include "stdafx.hpp"

namespace vtobjplugin::VirtoolsMenu {

	void InitializePluginInfo();
	PluginInfo* GetPluginInfo();
	PluginInterface* GetPluginInterface();

	void UnhandledExceptionCallback(const YYCC::yycc_u8string& log_path, const YYCC::yycc_u8string& coredump_path);

}
