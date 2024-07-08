#pragma once
#include "stdafx.hpp"
#include "afxdialogex.h"
#include "DataTypes.hpp"

namespace vtobjplugin {

	// ExportSettingDialog dialog

	class ExportSettingDialog : public CDialogEx {
		DECLARE_DYNAMIC(ExportSettingDialog)

	public:
		ExportSettingDialog(CKContext* ctx, CWnd* pParent = nullptr);   // standard constructor
		virtual ~ExportSettingDialog();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_EXPORT_SETTING_DIALOG };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		virtual BOOL OnInitDialog();
		afx_msg void OnBtnOkClicked();
		afx_msg void OnBtnCancelClicked();
		afx_msg void OnBtnResetClicked();
		afx_msg void OnBtnBrowseClicked();

	protected:
		// ===== Controls =====
		// General Settings
		CButton m_ExportMode_Object;
		CButton m_ExportMode_Group;
		CButton m_ExportMode_Selected;
		CButton m_ExportMode_All;
		CComboBox m_AssociatedExportItem;
		CButton m_FileMode_OneFile;
		CButton m_FileMode_MultiFile;
		CEdit m_ExportDirectory;
		// Object Settings
		CButton m_ObjectSplitMode_Group;
		CButton m_ObjectSplitMode_Object;
		CButton m_IgnoreTransform;
		CButton m_ConvertToRightHand;
		CButton m_UseNamePrefix;
		CButton m_EliminateNonAscii;
		// Reposition Settings
		CButton m_Generate3dsMaxScript;
		CButton m_GenerateBlenderScript;
		// Material Settings
		CButton m_ExportMaterial;
		CButton m_ExportTexture;
		CButton m_CopyTexture;
		CButton m_UseCustomTextureFormat;
		CEdit m_CustomTextureFormat;
		// Encoding Settings
		CButton m_CompositionEncoding_System;
		CButton m_CompositionEncoding_Custom;
		CEdit m_CustomEncoding;
		CButton m_UseUTF8ObjMtl;
		CButton m_UseUTF8Script;
		// ===== Members =====
		CKContext* m_Context;
		std::vector<CK_ID> m_AssocExportItemListCache; ///< Storing displayed associated export items list because UI only display their names.
		DataTypes::ExportMode m_ExportModeCache; ///< Storing current selected export mode for the convenience of detecting the change of export mode.
		YYCC::yycc_u8string m_ExportDirectoryCache; ///< Storing picked export directory because UI can not handle worldwide characters.
		// ===== Functions =====
		void UpdateAssocExportItems(); ///< Update associated export item list when export mode changed.
		void UpdateDisableHierarchy(); ///< Update disable status for specific controls according to the selection status of other controls.
		void UpdateExportDirectory(const YYCC::yycc_u8string& export_dir); ///< Update internal export directory variable and control display text.
	};

}
