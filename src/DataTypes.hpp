#pragma once
#include "stdafx.hpp"

namespace vtobjplugin::DataTypes {

	enum class ExportMode {
		Object, Group, Selected, All
	};

	enum class FileMode {
		OneFile, MultiFile
	};

	struct ExportSetting {
		ExportMode m_ExportMode;
		CK_ID m_AssociatedExportItem;
		FileMode m_FileMode;
		YYCC::yycc_u8string m_ExportDirectory;

		bool m_IgnoreTransform;
		bool m_ConvertToRightHand;
		bool m_UseNamePrefix;
		bool m_UseGroupSplittor;
		bool m_EliminateNonAscii;

		bool m_Generate3dsMaxScript;
		bool m_GenerateBlenderScript;

		bool m_ExportMaterial;
		bool m_ExportTexture;
		bool m_CopyTexture;
		bool m_UseCustomTextureFormat;
		YYCC::yycc_u8string m_CustomTextureFormat;

		bool m_UseCustomEncoding;
		UINT m_CustomEncoding;
		bool m_UseUTF8ObjMtl;
		bool m_UseUTF8Script;
	};

}
