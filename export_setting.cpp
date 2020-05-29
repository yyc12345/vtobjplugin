#include "stdafx.h"
#include "export_setting.h"

#pragma warning(disable:26812)

IMPLEMENT_DYNAMIC(ExportSetting, CDialogEx)

#pragma region ui

ExportSetting::ExportSetting(CKContext* ctx, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent),
	res_settings(NULL),
	current_folder(NULL),
	context(ctx),
	comboboxMirror(),
	cache_ExportMode(-1) {

	res_settings = new ExportConfig();

	current_folder = (char*)calloc(65526, sizeof(char));
	assert(current_folder != NULL);
	GetModuleFileName(NULL, current_folder, 65526);
	for (int i = strlen(current_folder); i >= 0; i--)
		if (current_folder[i] == '\\') {
			current_folder[i] = '\0';
			break;
		}
	strcat(current_folder, "\\vtobjplugin.cfg");

}

ExportSetting::~ExportSetting() {
	delete res_settings;
	free(current_folder);
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

	LoadConfig();
	switch (res_settings->export_mode) {
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
	switch (res_settings->file_mode) {
		case FILEMODE_ONEFILE:
			m_FileMode_All.SetCheck(1);
			break;
		case FILEMODE_MULTIFILE:
			m_FileMode_Single.SetCheck(1);
			break;
	}
	m_ExportFolder.SetWindowTextA(res_settings->export_folder);

	m_OmitTransform.SetCheck(res_settings->omit_transform);
	m_RightHand.SetCheck(res_settings->right_hand);

	m_Reposition_3dsmax.SetCheck(res_settings->reposition_3dsmax);
	m_Reposition_Blender.SetCheck(res_settings->reposition_blender);

	m_ExportMtl.SetCheck(res_settings->export_mtl);
	m_ExportTexture.SetCheck(res_settings->export_texture);
	m_CopyTexture.SetCheck(res_settings->copy_texture);
	m_CustomTextureFormat.SetCheck(res_settings->custom_texture_format);
	m_TextureFormat.SetWindowTextA(res_settings->texture_format);

	this->func_RefreshUI();
	this->func_ChangeExportMode();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


#pragma endregion

#pragma region core func

void ExportSetting::func_ExportFolderBroswer() {
	char* buffer = (char*)calloc(65526, sizeof(char));
	assert(buffer != NULL);
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
	free(buffer);
}

void ExportSetting::func_DialogOK() {

	//general setting
	res_settings->omit_transform = m_OmitTransform.GetCheck();
	res_settings->right_hand = m_RightHand.GetCheck();

	res_settings->reposition_3dsmax = m_Reposition_3dsmax.GetCheck();
	res_settings->reposition_blender = m_Reposition_Blender.GetCheck();

	res_settings->export_mtl = m_ExportMtl.GetCheck();
	res_settings->export_texture = m_ExportTexture.GetCheck();
	res_settings->copy_texture = m_CopyTexture.GetCheck();
	res_settings->custom_texture_format = m_CustomTextureFormat.GetCheck();
	m_TextureFormat.GetWindowTextA(res_settings->texture_format, 32);

	//file mode apply
	if (m_FileMode_All.GetCheck()) res_settings->file_mode = FILEMODE_ONEFILE;
	else res_settings->file_mode = FILEMODE_MULTIFILE;

	//export mode check
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

	//export folder check
	m_ExportFolder.GetWindowTextA(res_settings->export_folder, 65526);
	if (res_settings->export_folder[0] == '\0') {
		MessageBoxA("Export folder should not be empty.", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(res_settings->export_folder, &wfd);
	if (!((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
		MessageBoxA("Export folder should be existed!", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}
	FindClose(hFind);

	//custom texture format check
	if (res_settings->export_mtl && res_settings->export_texture && res_settings->custom_texture_format && res_settings->texture_format[0] == '\0') {
		MessageBoxA("Texture format should not be empty.", "Setting error", MB_OK + MB_ICONERROR);
		return;
	}

	//check pass. save current config and exit
	SaveConfig();
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
			m_ExportList.AddString(objArray[i]->GetName());
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
}

void ExportSetting::LoadConfig() {
	FILE* f = fopen(current_folder, "r");
	if (f == NULL) {
		//no config, set default and save it.
		res_settings->export_mode = EXPORTMODE_ALL;
		res_settings->selected_item = 0;
		res_settings->file_mode = FILEMODE_MULTIFILE;
		res_settings->export_folder[0] = '\0';
		res_settings->omit_transform = TRUE;
		res_settings->right_hand = TRUE;
		res_settings->reposition_3dsmax = FALSE;
		res_settings->reposition_blender = FALSE;
		res_settings->export_mtl = TRUE;
		res_settings->export_texture = TRUE;
		res_settings->copy_texture = FALSE;
		res_settings->custom_texture_format = TRUE;
		strcpy(res_settings->texture_format, "bmp");

		SaveConfig();
		return;
	}

	//read config
	res_settings->selected_item = 0;

#define trimLinebreaker(arr) arr[strlen(arr)-1]='\0';
#define readint(target,conv) fgets(intcache, 12, f);trimLinebreaker(intcache);target=(conv)atoi(intcache);

	char intcache[12];
	readint(res_settings->export_mode, ExportMode);
	readint(res_settings->file_mode, FileMode);
	fgets(res_settings->export_folder, 65526, f);  trimLinebreaker(res_settings->export_folder);
	readint(res_settings->omit_transform, BOOL);
	readint(res_settings->right_hand, BOOL);
	readint(res_settings->reposition_3dsmax, BOOL);
	readint(res_settings->reposition_blender, BOOL);
	readint(res_settings->export_mtl, BOOL);
	readint(res_settings->export_texture, BOOL);
	readint(res_settings->copy_texture, BOOL);
	readint(res_settings->custom_texture_format, BOOL);
	fgets(res_settings->texture_format, 32, f);  trimLinebreaker(res_settings->texture_format);

	fclose(f);
}

void ExportSetting::SaveConfig() {
	FILE* f = fopen(current_folder, "w");
	assert(f != NULL);

#define writeint(target) itoa((int)target, intcache, 10);fputs(intcache, f);fputs("\n", f);
#define writestr(target) fputs(target, f);fputs("\n", f);

	char intcache[12];
	writeint(res_settings->export_mode);
	writeint(res_settings->file_mode);
	writestr(res_settings->export_folder);
	writeint(res_settings->omit_transform);
	writeint(res_settings->right_hand);
	writeint(res_settings->reposition_3dsmax);
	writeint(res_settings->reposition_blender);
	writeint(res_settings->export_mtl);
	writeint(res_settings->export_texture);
	writeint(res_settings->copy_texture);
	writeint(res_settings->custom_texture_format);
	writestr(res_settings->texture_format);

	fclose(f);
}

#pragma endregion
