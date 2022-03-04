#if !defined(_YYCDLL_EXPORT_SETTING_H__IMPORTED_)
#define _YYCDLL_EXPORT_SETTING_H__IMPORTED_

#include "stdafx.h"
#include <afxdialogex.h>
#include <vector>
#include "obj_export.h"
// ExportSetting 对话框

class ExportSetting : public CDialogEx {
	DECLARE_DYNAMIC(ExportSetting)

	public:
	ExportSetting(CKContext* ctx, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ExportSetting();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	public:
	//=============general setting
	CButton m_ExportMode_Object;
	CButton m_ExportMode_Group;
	CButton m_ExportMode_All;
	CComboBox m_ExportList;

	CButton m_FileMode_All;
	CButton m_FileMode_Single;

	CEdit m_ExportFolder;

	//=============object setting
	CButton m_OmitTransform;
	CButton m_RightHand;
	CButton m_NamePrefix;
	CButton m_SplitMode_Group;
	CButton m_SplitMode_Object;
	CButton m_EliminateNonAscii;

	//=============reposition setting
	CButton m_Reposition_3dsmax;
	CButton m_Reposition_Blender;

	//=============material setting
	CButton m_ExportMtl;
	CButton m_ExportTexture;
	CButton m_CopyTexture;
	CButton m_CustomTextureFormat;
	CEdit m_TextureFormat;

	//=============encoding setting
	CButton m_EncodingType_System;
	CButton m_EncodingType_Custom;
	CEdit m_CustomEncoding;
	CButton m_Encoding_UTF8Object;
	CButton m_Encoding_UTF8Script;

	virtual BOOL OnInitDialog();
	afx_msg void func_ExportFolderBroswer();
	afx_msg void func_DialogOK();
	afx_msg void func_DialogCancel();
	afx_msg void func_ChangeExportMode();
	afx_msg void func_RefreshUI();

	ExportConfig res_settings;

	private:
	CKContext* context;
	std::vector<CK_ID> comboboxMirror;
	int cache_ExportMode;
	
};
#endif
