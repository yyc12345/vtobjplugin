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
	DDX_Control(pDX, IDC_RADIO6, m_SplitMode_Group);
	DDX_Control(pDX, IDC_RADIO7, m_SplitMode_Object);
	DDX_Control(pDX, IDC_CHECK10, m_EliminateNonAscii);

	//=============reposition setting
	DDX_Control(pDX, IDC_CHECK7, m_Reposition_3dsmax);
	DDX_Control(pDX, IDC_CHECK8, m_Reposition_Blender);

	//=============material setting
	DDX_Control(pDX, IDC_CHECK1, m_ExportMtl);
	DDX_Control(pDX, IDC_CHECK3, m_ExportTexture);
	DDX_Control(pDX, IDC_CHECK6, m_CopyTexture);
	DDX_Control(pDX, IDC_CHECK5, m_CustomTextureFormat);
	DDX_Control(pDX, IDC_EDIT2, m_TextureFormat);

	//=============encoding setting
	DDX_Control(pDX, IDC_RADIO8, m_EncodingType_System);
	DDX_Control(pDX, IDC_RADIO9, m_EncodingType_Custom);
	DDX_Control(pDX, IDC_EDIT3, m_CustomEncoding);
	DDX_Control(pDX, IDC_CHECK11, m_Encoding_UTF8Object);
	DDX_Control(pDX, IDC_CHECK12, m_Encoding_UTF8Script);
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
	ON_BN_CLICKED(IDC_RADIO8, &ExportSetting::func_RefreshUI)
	ON_BN_CLICKED(IDC_RADIO9, &ExportSetting::func_RefreshUI)
END_MESSAGE_MAP()


// ExportSetting 消息处理程序

BOOL ExportSetting::OnInitDialog() {
	CDialogEx::OnInitDialog();

	std::string uint_cache;
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

	if (res_settings.use_group_split_object) {
		m_SplitMode_Object.SetCheck(1);
	} else {
		m_SplitMode_Group.SetCheck(1);
	}
	m_OmitTransform.SetCheck(res_settings.omit_transform);
	m_RightHand.SetCheck(res_settings.right_hand);
	m_NamePrefix.SetCheck(res_settings.name_prefix);
	m_EliminateNonAscii.SetCheck(res_settings.eliminate_non_ascii);

	m_Reposition_3dsmax.SetCheck(res_settings.reposition_3dsmax);
	m_Reposition_Blender.SetCheck(res_settings.reposition_blender);

	m_ExportMtl.SetCheck(res_settings.export_mtl);
	m_ExportTexture.SetCheck(res_settings.export_texture);
	m_CopyTexture.SetCheck(res_settings.copy_texture);
	m_CustomTextureFormat.SetCheck(res_settings.custom_texture_format);
	m_TextureFormat.SetWindowTextA(res_settings.texture_format.c_str());

	if (res_settings.use_custom_encoding) {
		m_EncodingType_Custom.SetCheck(1);
	} else {
		m_EncodingType_System.SetCheck(1);
	}
	::string_helper::uint_to_stdstring(&uint_cache, res_settings.composition_encoding);
	m_CustomEncoding.SetWindowTextA(uint_cache.c_str());
	m_Encoding_UTF8Object.SetCheck(res_settings.use_utf8_obj);
	m_Encoding_UTF8Script.SetCheck(res_settings.use_utf8_script);


	this->func_ChangeExportMode();
	//this->func_RefreshUI(); // func_ChangeExportMode has called this.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


#pragma endregion

#pragma region core func

void ExportSetting::func_ExportFolderBroswer() {
	std::string display_name(MAX_PATH, '\0');

	BROWSEINFOA folderViewer = { 0 };
	folderViewer.hwndOwner = m_hWnd;
	folderViewer.pidlRoot = NULL;
	folderViewer.pszDisplayName = display_name.data();
	folderViewer.lpszTitle = "Pick a folder";
	folderViewer.lpfn = NULL;
	folderViewer.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	PIDLIST_ABSOLUTE data = SHBrowseForFolder(&folderViewer);
	if (data == NULL) return;
	if (SHGetPathFromIDListA(data, display_name.data()))
		m_ExportFolder.SetWindowTextA(display_name.c_str());
	CoTaskMemFree(data);
}

void ExportSetting::func_DialogOK() {

	//general setting
	res_settings.use_group_split_object = m_SplitMode_Group.GetCheck();
	res_settings.omit_transform = m_OmitTransform.GetCheck();
	res_settings.right_hand = m_RightHand.GetCheck();
	res_settings.name_prefix = m_NamePrefix.GetCheck();
	res_settings.eliminate_non_ascii = m_EliminateNonAscii.GetCheck();

	res_settings.reposition_3dsmax = m_Reposition_3dsmax.GetCheck();
	res_settings.reposition_blender = m_Reposition_Blender.GetCheck();

	res_settings.export_mtl = m_ExportMtl.GetCheck();
	res_settings.export_texture = m_ExportTexture.GetCheck();
	res_settings.copy_texture = m_CopyTexture.GetCheck();
	res_settings.custom_texture_format = m_CustomTextureFormat.GetCheck();
	::string_helper::cwndtext_to_stdstring(&m_TextureFormat, &res_settings.texture_format);

	res_settings.use_utf8_obj = m_Encoding_UTF8Object.GetCheck();
	res_settings.use_utf8_script = m_Encoding_UTF8Script.GetCheck();

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
	::string_helper::cwndtext_to_stdstring(&m_ExportFolder, &res_settings.export_folder);
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

	// composition encoding check
	if (m_EncodingType_System.GetCheck() == 1) {
		res_settings.use_custom_encoding = FALSE;
	} else {
		res_settings.use_custom_encoding = TRUE;

		std::string encoding_cp;
		::string_helper::cwndtext_to_stdstring(&m_CustomEncoding, &encoding_cp);
		if (encoding_cp.empty() ||
			!(::string_helper::stdstring_to_uint(&encoding_cp, &res_settings.composition_encoding)) ||
			!(::string_helper::check_cp_validation(res_settings.composition_encoding))) {
			MessageBoxA("Custom encoding is invalid.", "Setting error", MB_OK + MB_ICONERROR);
			return;
		}
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
	std::string noname_obj;
	int count = 0;
	if (m_ExportMode_Object.GetCheck() == 1) {
		XObjectPointerArray objArray = context->GetObjectListByType(CKCID_3DENTITY, TRUE);
		count = objArray.Size();
		for (int i = 0; i < count; i++) {
			comboboxMirror.push_back(objArray[i]->GetID());
			if (objArray[i]->GetName() != NULL)
				m_ExportList.AddString(objArray[i]->GetName());
			else {
				::string_helper::stdstring_sprintf(&noname_obj, "[unnamed 3d object] (CKID: %d)", objArray[i]->GetID());
				m_ExportList.AddString(noname_obj.c_str());
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
				::string_helper::stdstring_sprintf(&noname_obj, "[unnamed group] (CKID: %d)", idList[i]);
				m_ExportList.AddString(noname_obj.c_str());
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

	//encoding field
	if (m_EncodingType_System.GetCheck()) m_CustomEncoding.EnableWindow(FALSE);
	else m_CustomEncoding.EnableWindow(TRUE);

	// export mode for combobox
	m_ExportList.EnableWindow(!m_ExportMode_All.GetCheck());
}

#pragma endregion
