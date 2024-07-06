#include "ExportSettingDialog.hpp"

namespace vtobjplugin {

	// ExportSettingDialog dialog

	IMPLEMENT_DYNAMIC(ExportSettingDialog, CDialogEx)

	ExportSettingDialog::ExportSettingDialog(CKContext* ctx, CWnd* pParent /*=nullptr*/)
		: CDialogEx(IDD_EXPORT_SETTING_DIALOG, pParent) {

	}

	ExportSettingDialog::~ExportSettingDialog() {}

	void ExportSettingDialog::DoDataExchange(CDataExchange* pDX) {
		CDialogEx::DoDataExchange(pDX);
	}


	BEGIN_MESSAGE_MAP(ExportSettingDialog, CDialogEx)
	END_MESSAGE_MAP()


	// ExportSettingDialog message handlers

}
