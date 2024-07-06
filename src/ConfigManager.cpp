#include "ConfigManager.hpp"
#include "VirtoolsMenu.hpp"
#include <filesystem>

namespace vtobjplugin {

	ConfigManager& ConfigManager::GetSingleton() {
		static ConfigManager g_ConfigManager;
		return g_ConfigManager;
	}

	YYCC::yycc_u8string ConfigManager::GetConfigFilePath() {
		std::filesystem::path vt_path(
			VirtoolsMenu::GetPluginInterface()->GetVirtoolsDirectory()
		);
		vt_path /= YYCC::FsPathPatch::FromUTF8Path(YYCC_U8("vtobjplugin.cfg"));
		return YYCC::FsPathPatch::ToUTF8Path(vt_path);
	}

	ConfigManager::ConfigManager() :
		m_ExportMode(YYCC_U8("export-mode"), DataTypes::ExportMode::All),
		m_FileMode(YYCC_U8("file-mode"), DataTypes::FileMode::OneFile),
		m_ExportDirectory(YYCC_U8("export-directory"), YYCC_U8("")),

		m_IgnoreTransform(YYCC_U8("ignore-transform"), true),
		m_ConvertToRightHand(YYCC_U8("convert-to-right-hand"), true),
		m_UseNamePrefix(YYCC_U8("use-name-prefix"), false),
		m_UseGroupSplittor(YYCC_U8("sue-group-splittor"), true),
		m_EliminateNonAscii(YYCC_U8("eliminate-non-ascii"), false),

		m_Generate3dsMaxScript(YYCC_U8("generate-3ds-max-script"), false),
		m_GenerateBlenderScript(YYCC_U8("generate-blender-script"), false),

		m_ExportMaterial(YYCC_U8("export-material"), true),
		m_ExportTexture(YYCC_U8("export-texture"), true),
		m_CopyTexture(YYCC_U8("copy-texture"), false),
		m_UseCustomTextureFormat(YYCC_U8("use-custom-texture-format"), true),
		m_CustomTextureFormat(YYCC_U8("custom-texture-format"), YYCC_U8("bmp")),

		m_UseCustomEncoding(YYCC_U8("use-custom-encoding"), false),
		m_CustomEncoding(YYCC_U8("custom-encoding"), CP_ACP),
		m_UseUTF8ObjMtl(YYCC_U8("use-utf8-obj-mtl"), false),
		m_UseUTF8Script(YYCC_U8("use-utf8-script"), false),

		m_CoreManager(ConfigManager::GetConfigFilePath().c_str(), UINT64_C(150), {
			&m_ExportMode, &m_FileMode, &m_ExportDirectory,
			&m_IgnoreTransform, &m_ConvertToRightHand, &m_UseNamePrefix, &m_UseGroupSplittor, &m_EliminateNonAscii,
			&m_Generate3dsMaxScript, &m_GenerateBlenderScript,
			&m_ExportMaterial, &m_ExportTexture, &m_CopyTexture, &m_UseCustomTextureFormat, &m_CustomTextureFormat,
			&m_UseCustomEncoding, &m_CustomEncoding, &m_UseUTF8ObjMtl, &m_UseUTF8Script
		}) {}

}