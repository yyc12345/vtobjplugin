#pragma once
#include "stdafx.hpp"

namespace vtobjplugin::DataTypes {

	enum class ExportMode {
		Object, Group, Selected, All
	};

	enum class FileMode {
		OneFile, MultiFile
	};

	enum class ObjectSplitMode {
		Group, Object
	};

	enum class CompositionEncoding {
		System, Custom
	};

	struct ExportSetting {
		ExportSetting() :
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

		ExportMode m_ExportMode;
		CK_ID m_AssociatedExportItem;
		FileMode m_FileMode;
		YYCC::yycc_u8string m_ExportDirectory;

		bool m_IgnoreTransform;
		bool m_ConvertToRightHand;
		bool m_UseNamePrefix;
		ObjectSplitMode m_ObjectSplitMode;
		bool m_EliminateNonAscii;

		bool m_Generate3dsMaxScript;
		bool m_GenerateBlenderScript;

		bool m_ExportMaterial;
		bool m_ExportTexture;
		bool m_CopyTexture;
		bool m_UseCustomTextureFormat;
		YYCC::yycc_u8string m_CustomTextureFormat;

		CompositionEncoding m_CompositionEncoding;
		UINT m_CustomEncoding;
		bool m_UseUTF8ObjMtl;
		bool m_UseUTF8Script;
	};

}
