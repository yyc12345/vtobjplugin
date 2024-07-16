#include "DataTypes.hpp"

namespace vtobjplugin::DataTypes {

	ExportSetting::ExportSetting() :
		m_ExportMode(ExportMode::All),
		m_AssociatedExportItem((CK_ID)0),
		m_FileMode(FileMode::OneFile),
		m_ExportDirectory(YYCC_U8("")),

		m_IgnoreTransform(true),
		m_ConvertToRightHand(true),
		m_UseNamePrefix(false),
		m_ObjectSplitMode(ObjectSplitMode::Group),
		m_EliminateNonAscii(false),

		m_Generate3dsMaxScript(false),
		m_GenerateBlenderScript(false),

		m_ExportMaterial(true),
		m_ExportTexture(true),
		m_CopyTexture(false),
		m_UseCustomTextureFormat(false),
		m_CustomTextureFormat(YYCC_U8("bmp")),

		m_CompositionEncoding(CompositionEncoding::System),
		m_CustomEncoding(CP_ACP),
		m_UseUTF8ObjMtl(false),
		m_UseUTF8Script(false) {}

	bool ExportSetting::CanGenerate3dsMaxScript() const {
		return m_IgnoreTransform && m_ConvertToRightHand && m_Generate3dsMaxScript;
	}
	
	bool ExportSetting::CanGenerateBlenderScript() const {
		return m_IgnoreTransform && m_ConvertToRightHand && m_GenerateBlenderScript;
	}

	bool ExportSetting::CanExportMaterial() const {
		return m_ExportMaterial;
	}

	bool ExportSetting::CanExportTexture() const {
		return m_ExportMaterial && m_ExportTexture;
	}

	bool ExportSetting::CanCopyTexture() const {
		return m_ExportMaterial && m_ExportTexture && m_CopyTexture;
	}

	bool ExportSetting::CanUseCustomTextureFormat() const {
		return m_ExportMaterial && m_ExportTexture && m_UseCustomTextureFormat;
	}

	UINT ExportSetting::GetCompositionEncoding() const {
		switch (m_CompositionEncoding) {
			case CompositionEncoding::System:
				return CP_ACP;
			case CompositionEncoding::Custom:
				return m_CustomEncoding;
			default:
				throw std::runtime_error("invalid composition encoding.");
		}
	}

}
