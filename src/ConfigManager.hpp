#pragma once
#include "stdafx.hpp"
#include "DataTypes.hpp"

namespace vtobjplugin {

	class ConfigManager {
	public:
		static ConfigManager& GetSingleton();
	private:
		static YYCC::yycc_u8string GetConfigFilePath();
		ConfigManager();

		//void GetSettings(DataTypes::ExportSetting& settings);
		//void SetSettings(const DataTypes::ExportSetting& settings);

	public:
		YYCC::ConfigManager::NumberSetting<DataTypes::ExportMode> m_ExportMode;
		YYCC::ConfigManager::NumberSetting<DataTypes::FileMode> m_FileMode;
		YYCC::ConfigManager::StringSetting m_ExportDirectory;

		YYCC::ConfigManager::NumberSetting<bool>
			m_IgnoreTransform,
			m_ConvertToRightHand,
			m_UseNamePrefix,
			m_EliminateNonAscii;
		YYCC::ConfigManager::NumberSetting<DataTypes::ObjectSplitMode> m_ObjectSplitMode;

		YYCC::ConfigManager::NumberSetting<bool>
			m_Generate3dsMaxScript,
			m_GenerateBlenderScript;

		YYCC::ConfigManager::NumberSetting<bool>
			m_ExportMaterial,
			m_ExportTexture,
			m_CopyTexture,
			m_UseCustomTextureFormat;
		YYCC::ConfigManager::StringSetting m_CustomTextureFormat;

		YYCC::ConfigManager::NumberSetting<DataTypes::CompositionEncoding> m_CompositionEncoding;
		YYCC::ConfigManager::NumberSetting<UINT> m_CustomEncoding;
		YYCC::ConfigManager::NumberSetting<bool>
			m_UseUTF8ObjMtl,
			m_UseUTF8Script;

		YYCC::ConfigManager::CoreManager m_CoreManager;
	};

}
