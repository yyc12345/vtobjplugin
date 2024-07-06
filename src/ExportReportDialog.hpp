#pragma once
#include "stdafx.hpp"
#include "afxdialogex.h"

namespace vtobjplugin {

	// ExportReportDialog dialog

	class ExportReportDialog : public CDialogEx {
		DECLARE_DYNAMIC(ExportReportDialog)

	public:
		ExportReportDialog(CKContext* ctx, CWnd* pParent = nullptr);   // standard constructor
		virtual ~ExportReportDialog();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_EXPORT_REPORT_DIALOG };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	};

}
