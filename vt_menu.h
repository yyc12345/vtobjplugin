#if !defined(_YYCDLL_VT_MENU_H__IMPORTED_)
#define _YYCDLL_VT_MENU_H__IMPORTED_

#include "stdafx.h"

void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface);

void InitMenu();
void RemoveMenu();
void UpdateMenu();
void PluginMenuCallback(int commandID);

#endif