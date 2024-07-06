#include "ExportReportDialog.hpp"

namespace vtobjplugin {

	// ExportReportDialog dialog

	IMPLEMENT_DYNAMIC(ExportReportDialog, CDialogEx)

	ExportReportDialog::ExportReportDialog(CKContext* ctx, CWnd* pParent /*=nullptr*/)
		: CDialogEx(IDD_EXPORT_REPORT_DIALOG, pParent) {

	}

	ExportReportDialog::~ExportReportDialog() {}

	void ExportReportDialog::DoDataExchange(CDataExchange* pDX) {
		CDialogEx::DoDataExchange(pDX);
	}


	BEGIN_MESSAGE_MAP(ExportReportDialog, CDialogEx)
	END_MESSAGE_MAP()


	// ExportReportDialog message handlers

}
