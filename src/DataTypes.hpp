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
		ExportSetting();

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

		// ===== Assistant Functions =====
		// These functions are used by exporter.
		// Because these settings has complex hierarchy relation.
		// It would be good to create some functions to get correct settings 
		// according to the relation

		bool CanGenerate3dsMaxScript() const;
		bool CanGenerateBlenderScript() const;

		bool CanExportMaterial() const;
		bool CanExportTexture() const;
		bool CanCopyTexture() const;
		bool CanUseCustomTextureFormat() const;

		UINT GetCompositionEncoding() const;
	};

}
