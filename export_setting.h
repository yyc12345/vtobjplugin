#if !defined(_YYCDLL_EXPORT_SETTING_H__IMPORTED_)
#define _YYCDLL_EXPORT_SETTING_H__IMPORTED_

#include "stdafx.h"
#include <afxdialogex.h>
#include <vector>
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
	CEdit m_ExportFolder;
	CButton m_OmitTransform;
	CButton m_ExportMtl;
	CButton m_ExportTexture;
	CEdit m_TextureFormat;
	CButton m_ExportMode_Object;
	CButton m_ExportMode_Group;
	CButton m_ExportMode_All;	
	CComboBox m_ExportList;
	CButton m_RightHand;
	virtual BOOL OnInitDialog();
	afx_msg void func_ExportFolderBroswer();
	afx_msg void func_DialogOK();
	afx_msg void func_DialogCancel();
	afx_msg void func_ChangeExportMode(UINT nID);

	char* res_ExportFolder;
	BOOL res_OmitTransform;
	BOOL res_ExportMtl;
	BOOL res_ExportTexture;
	BOOL res_RightHand;
	char* res_TextureFormat;
	CK_ID res_SelectedItem;
	int res_ExportMode;//0-obj, 1-group, 2-all

	private:
	CKContext* context;
	std::vector<CK_ID> comboboxMirror;
	int cache_ExportMode;
};


#endif
