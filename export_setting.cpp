#include "stdafx.h"
#include "export_setting.h"
#include "buffer_helper.h"
#include "config_manager.h"

#pragma warning(disable:26812)

IMPLEMENT_DYNAMIC(ExportSetting, CDialogEx)

#pragma region ui

ExportSetting::ExportSetting(CKContext* ctx, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent),
	res_settings(),
	context(ctx),
	comboboxMirror(),
	cache_ExportMode(-1) {
	;
}

ExportSetting::~ExportSetting() {
	;
}

void ExportSetting::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	//=============general setting
	//export mode
	DDX_Control(pDX, IDC_RADIO1, m_ExportMode_Object);
	DDX_Control(pDX, IDC_RADIO2, m_ExportMode_Group);
	DDX_Control(pDX, IDC_RADIO3, m_ExportMode_All);
	DDX_Control(pDX, IDC_COMBO1, m_ExportList);
	//file mode
	DDX_Control(pDX, IDC_RADIO4, m_FileMode_All);
	DDX_Control(pDX, IDC_RADIO5, m_FileMode_Single);
	//export folder
	DDX_Control(pDX, IDC_EDIT1, m_ExportFolder);

	//=============object setting
	DDX_Control(pDX, IDC_CHECK2, m_OmitTransform);
	DDX_Control(pDX, IDC_CHECK4, m_RightHand);
	DDX_Control(pDX, IDC_CHECK9, m_NamePrefix);

	//=============reposition setting
	DDX_Control(pDX, IDC_CHECK7, m_Reposition_3dsmax);
	DDX_Control(pDX, IDC_CHECK8, m_Reposition_Blender);

	//=============material setting
	DDX_Control(pDX, IDC_CHECK1, m_ExportMtl);
	DDX_Control(pDX, IDC_CHECK3, m_ExportTexture);
	DDX_Control(pDX, IDC_CHECK6, m_CopyTexture);
	DDX_Control(pDX, IDC_CHECK5, m_CustomTextureFormat);
	DDX_Control(pDX, IDC_EDIT2, m_TextureFormat);

}


BEGIN_MESSAGE_MAP(ExportSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ExportSetting::func_ExportFolderBroswer)
	ON_BN_CLICKED(IDOK, &ExportSetting::func_DialogOK)
	ON_BN_CLICKED(IDCANCEL, &ExportSetting::func_DialogCancel)

	ON_BN_CLICKED(IDC_RADIO1, &ExportSetting::func_ChangeExportMode)
	ON_BN_CLICKED(IDC_RADIO2, &ExportSetting::func_ChangeExportMode)
	ON_BN_CLICKED(IDC_RADIO3, &ExportSetting::func_ChangeExportMode)

	ON_BN_CLICKED(IDC_CHECK1, &ExportSetting::func_RefreshUI)
	ON_BN_CLICKED(IDC_CHECK3, &ExportSetting::func_RefreshUI)
	ON_BN_CLICKED(IDC_CHECK6, &ExportSetting::func_RefreshUI)
	ON_BN_CLICKED(IDC_CHECK5, &ExportSetting::func_RefreshUI)
	ON_BN_CLICKED(IDC_CHECK2, &ExportSetting::func_RefreshUI)
	ON_BN_CLICKED(IDC_CHECK4, &ExportSetting::func_RefreshUI)
END_MESSAGE_MAP()


// ExportSetting 消息处理程序

BOOL ExportSetting::OnInitDialog() {
	CDialogEx::OnInitDialog();

	config_manager::LoadConfig(&res_settings);
	switch (res_settings.export_mode) {
		case EXPORTMODE_OBJECT:
			m_ExportMode_Object.SetCheck(1);
			break;
		case EXPORTMODE_GROUP:
			m_ExportMode_Group.SetCheck(1);
			break;
		case EXPORTMODE_ALL:
			m_ExportMode_All.SetCheck(1);
			break;
	}
	switch (res_settings.file_mode) {
		case FILEMODE_ONEFILE:
			m_FileMode_All.SetCheck(1);
			break;
		case FILEMODE_MULTIFILE:
			m_FileMode_Single.SetCheck(1);
			break;
	}
	m_ExportFolder.SetWindowTextA(res_settings.export_folder.c_str());

	m_OmitTransform.SetCheck(res_settings.omit_transform);
	m_RightHand.SetCheck(res_settings.right_hand);
	m_NamePrefix.SetCheck(res_settings.name_prefix);

	m_Reposition_3dsmax.SetCheck(res_settings.reposition_3dsmax);
	m_Reposition_Blender.SetCheck(res_settings.reposition_blender);

	m_ExportMtl.SetCheck(res_settings.export_mtl);
	m_ExportTexture.SetCheck(res_settings.export_texture);
	m_CopyTexture.SetCheck(res_settings.copy_texture);
	m_CustomTextureFormat.SetCheck(res_settings.custom_texture_format);
	m_TextureFormat.SetWindowTextA(res_settings.texture_format.c_str());

	this->func_ChangeExportMode();
	//this->func_RefreshUI(); // func_ChangeExportMode has called this.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


#pragma endregion

#pragma region core func

void ExportSetting::func_ExportFolderBroswer() {
	BROWSEINFOA folderViewer = { 0 };
	folderViewer.hwndOwner = m_hWnd;
	folderViewer.pidlRoot = NULL;
	folderViewer.pszDisplayName = buffer_helper::global_buffer;
	folderViewer.lpszTitle = "Pick a folder";
	folderViewer.lpfn = NULL;
	folderViewer.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	PIDLIST_ABSOLUTE data = SHBrowseForFolder(&folderViewer);
	if (data == NULL) return;
	if (SHGetPathFromIDList(data, buffer_helper::global_buffer))
		m_ExportFolder.SetWindowTextA(buffer_helper::global_buffer);
	CoTaskMemFree(data);
}

void ExportSetting::func_DialogOK() {

	//general setting
	res_settings.omit_transform = m_OmitTransform.GetCheck();
	res_settings.right_hand = m_RightHand.GetCheck();
	res_settings.name_prefix = m_NamePrefix.GetCheck();

	res_settings.reposition_3dsmax = m_Reposition_3dsmax.GetCheck();
	res_settings.reposition_blender = m_Reposition_Blender.GetCheck();

	res_settings.export_mtl = m_ExportMtl.GetCheck();
	res_settings.export_texture = m_ExportTexture.GetCheck();
	res_settings.copy_texture = m_CopyTexture.GetCheck();
	res_settings.custom_texture_format = m_CustomTextureFormat.GetCheck();
	m_TextureFormat.GetWindowTextA(buffer_helper::global_buffer, BUFFER_SIZE);
	res_settings.texture_format = buffer_helper::global_buffer;

	//file mode apply
	if (m_FileMode_All.GetCheck()) res_settings.file_mode = FILEMODE_ONEFILE;
	else res_settings.file_mode = FILEMODE_MULTIFILE;

	//export mode check
	if (m_ExportMode_All.GetCheck() == 1)
		res_settings.export_mode = EXPORTMODE_ALL;
	else {
		int gottenIndex = m_ExportList.GetCurSel();
		if (gottenIndex == CB_ERR) {
			MessageBoxA("You should specific a export target.", "Setting error", MB_OK + MB_ICONERROR);
			return;
		}
		res_settings.selected_item = comboboxMirror[gottenIndex];

		if (m_ExportMode_Object.GetCheck() == 1) res_settings.export_mode = EXPORTMODE_OBJECT;
		else res_settings.export_mode = EXPORTMODE_GROUP;
	}

	//export folder check
	m_ExportFolder.GetWindowTextA(buffer_helper::global_buffer, BUFFER_SIZE);
	res_settings.export_folder = buffer_helper::global_buffer;
	if (res_settings.export_folder.empty()) {
		MessageBoxA("Export folder should not be empty.", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(res_settings.export_folder.c_str(), &wfd);
	if (!((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
		MessageBoxA("Export folder should be existed!", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}
	FindClose(hFind);

	//custom texture format check
	if (res_settings.export_mtl && res_settings.export_texture && res_settings.custom_texture_format && res_settings.texture_format.empty()) {
		MessageBoxA("Texture format should not be empty.", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}

	//check pass. save current config and exit
	config_manager::SaveConfig(&res_settings);
	CDialogEx::OnOK();
}

void ExportSetting::func_DialogCancel() {
	CDialogEx::OnCancel();
}

void ExportSetting::func_ChangeExportMode() {
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
			if (objArray[i]->GetName() != NULL) 
				m_ExportList.AddString(objArray[i]->GetName());
			else {
				sprintf(buffer_helper::global_buffer, "[unnamed 3d object] (CKID: %d)", objArray[i]->GetID());
				m_ExportList.AddString(buffer_helper::global_buffer);
			}
		}
	} else if (m_ExportMode_Group.GetCheck() == 1) {
		count = context->GetObjectsCountByClassID(CKCID_GROUP);
		CK_ID* idList = context->GetObjectsListByClassID(CKCID_GROUP);
		//iterate
		for (int i = 0; i < count; i++) {
			comboboxMirror.push_back(idList[i]);
			if (context->GetObjectA(idList[i])->GetName() != NULL)
				m_ExportList.AddString(context->GetObjectA(idList[i])->GetName());
			else {
				sprintf(buffer_helper::global_buffer, "[unnamed group] (CKID: %d)", idList[i]);
				m_ExportList.AddString(buffer_helper::global_buffer);
			}
		}
	}

	func_RefreshUI();
}

void ExportSetting::func_RefreshUI() {
	//material field
	if (m_ExportMtl.GetCheck()) m_ExportTexture.EnableWindow(TRUE);
	else m_ExportTexture.EnableWindow(FALSE);

	if (m_ExportTexture.GetCheck() && m_ExportTexture.IsWindowEnabled()) {
		m_CustomTextureFormat.EnableWindow(TRUE);
		m_CopyTexture.EnableWindow(TRUE);
	} else {
		m_CustomTextureFormat.EnableWindow(FALSE);
		m_CopyTexture.EnableWindow(FALSE);
	}

	if (m_CustomTextureFormat.GetCheck() && m_CustomTextureFormat.IsWindowEnabled()) m_TextureFormat.EnableWindow(TRUE);
	else m_TextureFormat.EnableWindow(FALSE);

	//repostion field
	if (m_OmitTransform.GetCheck() && m_RightHand.GetCheck()) {
		m_Reposition_3dsmax.EnableWindow(TRUE);
		m_Reposition_Blender.EnableWindow(TRUE);
	} else {
		m_Reposition_3dsmax.EnableWindow(FALSE);
		m_Reposition_Blender.EnableWindow(FALSE);
	}

	// export mode for combobox
	m_ExportList.EnableWindow(!m_ExportMode_All.GetCheck());
}

#pragma endregion
