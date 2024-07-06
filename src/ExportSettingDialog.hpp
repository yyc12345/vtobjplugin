#pragma once
#include "stdafx.hpp"
#include "afxdialogex.h"

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
	};

}
