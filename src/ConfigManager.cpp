#include "ConfigManager.hpp"
#include <filesystem>

namespace vtobjplugin {

	static const DataTypes::ExportSetting g_DefaultExportSetting;

	ConfigManager& ConfigManager::GetSingleton() {
		static ConfigManager g_ConfigManager;
		return g_ConfigManager;
	}

	YYCC::yycc_u8string ConfigManager::GetConfigFilePath() {
		// get path to executable virtools
		YYCC::yycc_u8string u8_virtools_path;
		if (!YYCC::WinFctHelper::GetModuleFileName(NULL, u8_virtools_path))
			u8_virtools_path.clear();
		// get its parent folder and append with cfg file name
		std::filesystem::path virtools_path(YYCC::StdPatch::ToStdPath(u8_virtools_path));
		return YYCC::StdPatch::ToUTF8Path(virtools_path.parent_path() / YYCC::StdPatch::ToStdPath(YYCC_U8("vtobjplugin.cfg")));
	}

#define INIT_HELPER(name) name(YYCC_U8( #name ), g_DefaultExportSetting.name)
	ConfigManager::ConfigManager() :
		INIT_HELPER(m_ExportMode),
		INIT_HELPER(m_FileMode),
		INIT_HELPER(m_ExportDirectory),

		INIT_HELPER(m_IgnoreTransform),
		INIT_HELPER(m_ConvertToRightHand),
		INIT_HELPER(m_UseNamePrefix),
		INIT_HELPER(m_ObjectSplitMode),
		INIT_HELPER(m_EliminateNonAscii),

		INIT_HELPER(m_Generate3dsMaxScript),
		INIT_HELPER(m_GenerateBlenderScript),

		INIT_HELPER(m_ExportMaterial),
		INIT_HELPER(m_ExportTexture),
		INIT_HELPER(m_CopyTexture),
		INIT_HELPER(m_UseCustomTextureFormat),
		INIT_HELPER(m_CustomTextureFormat),

		INIT_HELPER(m_CompositionEncoding),
		INIT_HELPER(m_CustomEncoding),
		INIT_HELPER(m_UseUTF8ObjMtl),
		INIT_HELPER(m_UseUTF8Script),

		m_CoreManager(ConfigManager::GetConfigFilePath().c_str(), UINT64_C(150), {
			&m_ExportMode, &m_FileMode, &m_ExportDirectory,
			&m_IgnoreTransform, &m_ConvertToRightHand, &m_UseNamePrefix, &m_ObjectSplitMode, &m_EliminateNonAscii,
			&m_Generate3dsMaxScript, &m_GenerateBlenderScript,
			&m_ExportMaterial, &m_ExportTexture, &m_CopyTexture, &m_UseCustomTextureFormat, &m_CustomTextureFormat,
			&m_CompositionEncoding, &m_CustomEncoding, &m_UseUTF8ObjMtl, &m_UseUTF8Script
		}) {}
#undef INIT_HELPER

//
//	void ConfigManager::GetSettings(DataTypes::ExportSetting& settings) {
//#define COPY_HELPER(name) settings.name = this->name.Get()
//
//		COPY_HELPER(m_ExportMode);
//		COPY_HELPER(m_FileMode);
//		COPY_HELPER(m_ExportDirectory);
//
//		COPY_HELPER(m_IgnoreTransform);
//		COPY_HELPER(m_ConvertToRightHand);
//		COPY_HELPER(m_UseNamePrefix);
//		COPY_HELPER(m_ObjectSplitMode);
//		COPY_HELPER(m_EliminateNonAscii);
//
//		COPY_HELPER(m_Generate3dsMaxScript);
//		COPY_HELPER(m_GenerateBlenderScript);
//
//		COPY_HELPER(m_ExportMaterial);
//		COPY_HELPER(m_ExportTexture);
//		COPY_HELPER(m_CopyTexture);
//		COPY_HELPER(m_UseCustomTextureFormat);
//		COPY_HELPER(m_CustomTextureFormat);
//
//		COPY_HELPER(m_UseCustomEncoding);
//		COPY_HELPER(m_CustomEncoding);
//		COPY_HELPER(m_UseUTF8ObjMtl);
//		COPY_HELPER(m_UseUTF8Script);
//
//#undef COPY_HELPER
//	}
//
//	void ConfigManager::SetSettings(const DataTypes::ExportSetting& settings) {
//#define COPY_HELPER(name) this->name.Set(settings.name)
//
//		COPY_HELPER(m_ExportMode);
//		COPY_HELPER(m_FileMode);
//		COPY_HELPER(m_ExportDirectory);
//
//		COPY_HELPER(m_IgnoreTransform);
//		COPY_HELPER(m_ConvertToRightHand);
//		COPY_HELPER(m_UseNamePrefix);
//		COPY_HELPER(m_ObjectSplitMode);
//		COPY_HELPER(m_EliminateNonAscii);
//
//		COPY_HELPER(m_Generate3dsMaxScript);
//		COPY_HELPER(m_GenerateBlenderScript);
//
//		COPY_HELPER(m_ExportMaterial);
//		COPY_HELPER(m_ExportTexture);
//		COPY_HELPER(m_CopyTexture);
//		COPY_HELPER(m_UseCustomTextureFormat);
//		COPY_HELPER(m_CustomTextureFormat);
//
//		COPY_HELPER(m_UseCustomEncoding);
//		COPY_HELPER(m_CustomEncoding);
//		COPY_HELPER(m_UseUTF8ObjMtl);
//		COPY_HELPER(m_UseUTF8Script);
//
//#undef COPY_HELPER
//	}

}