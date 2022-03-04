#if !defined(_YYCDLL_CONFIG_MANAGER_H__IMPORTED_)
#define _YYCDLL_CONFIG_MANAGER_H__IMPORTED_

#include "obj_export.h"

namespace config_manager {

	void GetConfigFilePath(std::string* buffer);
	void SaveConfig(ExportConfig* cfg);
	void LoadConfig(ExportConfig* cfg);

}

#endif