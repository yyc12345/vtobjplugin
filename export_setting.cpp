#include "stdafx.h"
#include "export_setting.h"

IMPLEMENT_DYNAMIC(ExportSetting, CDialogEx)

ExportSetting::ExportSetting(CKContext* ctx, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent),
	res_settings(NULL),
	context(ctx),
	comboboxMirror(),
	cache_ExportMode(2) {
	res_settings = new ExportConfig();
}

ExportSetting::~ExportSetting() {
	delete res_settings;
}

void ExportSetting::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ExportFolder);
	DDX_Control(pDX, IDC_CHECK2, m_OmitTransform);
	DDX_Control(pDX, IDC_CHECK1, m_ExportMtl);
	DDX_Control(pDX, IDC_CHECK3, m_ExportTexture);
	DDX_Control(pDX, IDC_EDIT2, m_TextureFormat);
	DDX_Control(pDX, IDC_RADIO1, m_ExportMode_Object);
	DDX_Control(pDX, IDC_RADIO2, m_ExportMode_Group);
	DDX_Control(pDX, IDC_RADIO3, m_ExportMode_All);
	DDX_Control(pDX, IDC_COMBO1, m_ExportList);
	DDX_Control(pDX, IDC_CHECK4, m_RightHand);
	DDX_Control(pDX, IDC_CHECK5, m_CustomTextureFormat);
	DDX_Control(pDX, IDC_CHECK6, m_SaveAlpha);
}


BEGIN_MESSAGE_MAP(ExportSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ExportSetting::func_ExportFolderBroswer)
	ON_BN_CLICKED(IDOK, &ExportSetting::func_DialogOK)
	ON_BN_CLICKED(IDCANCEL, &ExportSetting::func_DialogCancel)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &ExportSetting::func_ChangeExportMode)
END_MESSAGE_MAP()


// ExportSetting 消息处理程序

BOOL ExportSetting::OnInitDialog() {
	CDialogEx::OnInitDialog();

	m_OmitTransform.SetCheck(0);
	m_RightHand.SetCheck(1);

	m_ExportMtl.SetCheck(1);
	m_ExportTexture.SetCheck(1);
	m_CustomTextureFormat.SetCheck(1);
	m_TextureFormat.SetWindowTextA("bmp");
	m_SaveAlpha.SetCheck(1);

	m_ExportMode_All.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void ExportSetting::func_ExportFolderBroswer() {
	char buffer[1024];
	BROWSEINFOA folderViewer = { 0 };
	folderViewer.hwndOwner = m_hWnd;
	folderViewer.pidlRoot = NULL;
	folderViewer.pszDisplayName = buffer;
	folderViewer.lpszTitle = "Pick a folder";
	folderViewer.lpfn = NULL;
	folderViewer.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	PIDLIST_ABSOLUTE data = SHBrowseForFolder(&folderViewer);
	if (data == NULL) return;
	if (SHGetPathFromIDList(data, buffer))
		m_ExportFolder.SetWindowTextA(buffer);
	CoTaskMemFree(data);
}


void ExportSetting::func_DialogOK() {
	m_ExportFolder.GetWindowTextA(res_settings->export_folder, 1024);
	m_TextureFormat.GetWindowTextA(res_settings->texture_format, 32);
	res_settings->omit_transform = m_OmitTransform.GetCheck();
	res_settings->export_mtl = m_ExportMtl.GetCheck();
	res_settings->export_texture = m_ExportTexture.GetCheck();
	res_settings->right_hand = m_RightHand.GetCheck();
	res_settings->custom_texture_format = m_CustomTextureFormat.GetCheck();
	res_settings->save_alpha = m_SaveAlpha.GetCheck();

	if (m_ExportMode_All.GetCheck() == 1)
		res_settings->export_mode = EXPORTMODE_ALL;
	else {
		int gottenIndex = m_ExportList.GetCurSel();
		if (gottenIndex == CB_ERR) {
			MessageBoxA("You should specific a export target.", "Setting error", MB_OK + MB_ICONERROR);
			return;
		}
		res_settings->selected_item = comboboxMirror[gottenIndex];

		if (m_ExportMode_Object.GetCheck() == 1) res_settings->export_mode = EXPORTMODE_OBJECT;
		else res_settings->export_mode = EXPORTMODE_GROUP;
	}

	if (res_settings->export_folder[0] == '\0') {
		MessageBoxA("Export folder should not be empty.", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}
	if (res_settings->export_mtl && res_settings->export_texture && res_settings->custom_texture_format && res_settings->texture_format[0] == '\0') {
		MessageBoxA("Texture format should not be empty.", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}

	CDialogEx::OnOK();
}


void ExportSetting::func_DialogCancel() {
	CDialogEx::OnCancel();
}

void ExportSetting::func_ChangeExportMode(UINT nID) {
	//make sure only run when mode changed
	int nowExportMode;
	if (m_ExportMode_Object.GetCheck() == 1) nowExportMode = 0;
	else if (m_ExportMode_Group.GetCheck() == 1) nowExportMode = 1;
	else nowExportMode = 2;

	if (nowExportMode == cache_ExportMode) return;
	else cache_ExportMode = nowExportMode;

	//change mode
	m_ExportList.ResetContent();
	comboboxMirror.clear();
	int count = 0;
	if (m_ExportMode_Object.GetCheck() == 1) {
		XObjectPointerArray objArray = context->GetObjectListByType(CKCID_3DENTITY, TRUE);
		count = objArray.Size();
		for (int i = 0; i < count; i++) {
			comboboxMirror.push_back(objArray[i]->GetID());
			m_ExportList.AddString(objArray[i]->GetName());
			OutputDebugStringA(objArray[i]->GetName());
		}
	} else if (m_ExportMode_Group.GetCheck() == 1) {
		count = context->GetObjectsCountByClassID(CKCID_GROUP);
		CK_ID* idList = context->GetObjectsListByClassID(CKCID_GROUP);
		//iterate
		for (int i = 0; i < count; i++) {
			comboboxMirror.push_back(idList[i]);
			m_ExportList.AddString(context->GetObjectA(idList[i])->GetName());
		}
	} else return;


}
