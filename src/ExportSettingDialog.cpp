#include "ExportSettingDialog.hpp"
#include "ConfigManager.hpp"
#include "Utilities.hpp"

namespace vtobjplugin {

	// ExportSettingDialog dialog

	IMPLEMENT_DYNAMIC(ExportSettingDialog, CDialogEx)

		ExportSettingDialog::ExportSettingDialog(CKContext* ctx, CWnd* pParent /*=nullptr*/)
		: CDialogEx(IDD_EXPORT_SETTING_DIALOG, pParent),
		m_Context(ctx),
		m_AssocExportItemListCache(),
		m_ExportModeCache(DataTypes::ExportMode::All),
		m_ExportDirectoryCache() {}

	ExportSettingDialog::~ExportSettingDialog() {}

	void ExportSettingDialog::DoDataExchange(CDataExchange* pDX) {
		CDialogEx::DoDataExchange(pDX);
		// General Settings
		DDX_Control(pDX, IDC_RADIO1, m_ExportMode_Object);
		DDX_Control(pDX, IDC_RADIO2, m_ExportMode_Group);
		DDX_Control(pDX, IDC_RADIO3, m_ExportMode_Selected);
		DDX_Control(pDX, IDC_RADIO4, m_ExportMode_All);
		DDX_Control(pDX, IDC_COMBO1, m_AssociatedExportItem);
		DDX_Control(pDX, IDC_RADIO5, m_FileMode_OneFile);
		DDX_Control(pDX, IDC_RADIO6, m_FileMode_MultiFile);
		DDX_Control(pDX, IDC_EDIT1, m_ExportDirectory);

		// Object Settings
		DDX_Control(pDX, IDC_CHECK1, m_IgnoreTransform);
		DDX_Control(pDX, IDC_CHECK2, m_ConvertToRightHand);
		DDX_Control(pDX, IDC_CHECK3, m_UseNamePrefix);
		DDX_Control(pDX, IDC_RADIO7, m_ObjectSplitMode_Group);
		DDX_Control(pDX, IDC_RADIO8, m_ObjectSplitMode_Object);
		DDX_Control(pDX, IDC_CHECK4, m_EliminateNonAscii);

		// Reposition Settings
		DDX_Control(pDX, IDC_CHECK5, m_Generate3dsMaxScript);
		DDX_Control(pDX, IDC_CHECK6, m_GenerateBlenderScript);

		// Material Settings
		DDX_Control(pDX, IDC_CHECK7, m_ExportMaterial);
		DDX_Control(pDX, IDC_CHECK8, m_ExportTexture);
		DDX_Control(pDX, IDC_CHECK9, m_CopyTexture);
		DDX_Control(pDX, IDC_CHECK10, m_UseCustomTextureFormat);
		DDX_Control(pDX, IDC_EDIT2, m_CustomTextureFormat);

		// Encoding Settings
		DDX_Control(pDX, IDC_RADIO9, m_CompositionEncoding_System);
		DDX_Control(pDX, IDC_RADIO10, m_CompositionEncoding_Custom);
		DDX_Control(pDX, IDC_EDIT3, m_CustomEncoding);
		DDX_Control(pDX, IDC_CHECK11, m_UseUTF8ObjMtl);
		DDX_Control(pDX, IDC_CHECK12, m_UseUTF8Script);
	}


	BEGIN_MESSAGE_MAP(ExportSettingDialog, CDialogEx)
		// ===== Buttons =====
		ON_BN_CLICKED(IDOK, &ExportSettingDialog::OnBtnOkClicked)
		ON_BN_CLICKED(IDCANCEL, &ExportSettingDialog::OnBtnCancelClicked)
		ON_BN_CLICKED(IDC_BUTTON2, &ExportSettingDialog::OnBtnResetClicked)
		ON_BN_CLICKED(IDC_BUTTON1, &ExportSettingDialog::OnBtnBrowseClicked)
		ON_BN_CLICKED(IDC_BUTTON1, &ExportSettingDialog::OnBtnCancelClicked)
		// ===== Export Mode Radio Button =====
		ON_BN_CLICKED(IDC_RADIO1, &ExportSettingDialog::OnBtnExportModeClicked)
		ON_BN_CLICKED(IDC_RADIO2, &ExportSettingDialog::OnBtnExportModeClicked)
		ON_BN_CLICKED(IDC_RADIO3, &ExportSettingDialog::OnBtnExportModeClicked)
		ON_BN_CLICKED(IDC_RADIO4, &ExportSettingDialog::OnBtnExportModeClicked)
		// ===== Disable Hierarchy Related Button =====
		ON_BN_CLICKED(IDC_CHECK1, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // IgnoreTransform
		ON_BN_CLICKED(IDC_CHECK2, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // ConvertToRightHand
		ON_BN_CLICKED(IDC_CHECK7, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // Export Material
		ON_BN_CLICKED(IDC_CHECK8, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // Export Texture
		ON_BN_CLICKED(IDC_CHECK9, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // CopyTexture
		ON_BN_CLICKED(IDC_CHECK10, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // UseCustomTextureFormat
		ON_BN_CLICKED(IDC_RADIO9, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // CompositionEncoding System
		ON_BN_CLICKED(IDC_RADIO10, &ExportSettingDialog::OnBtnDisableHierarchyClicked) // CompositionEncoding Custom
	END_MESSAGE_MAP()


	// ExportSettingDialog message handlers

#pragma region Event Handler

	BOOL ExportSettingDialog::OnInitDialog() {
		CDialogEx::OnInitDialog();

		// Read settings from config manager
		auto& config_manager = vtobjplugin::ConfigManager::GetSingleton();
#define BOOL_SETTING_ASSIGNER(name) RADIOBTN_SETCHECK(this->name, config_manager.name.Get())
		// General Settings
		PushExportMode(config_manager.m_ExportMode.Get());
		PushFileMode(config_manager.m_FileMode.Get());
		m_ExportDirectoryCache = config_manager.m_ExportDirectory.Get();
		// Object Settings
		PushObjectSplitMode(config_manager.m_ObjectSplitMode.Get());
		BOOL_SETTING_ASSIGNER(m_IgnoreTransform);
		BOOL_SETTING_ASSIGNER(m_ConvertToRightHand);
		BOOL_SETTING_ASSIGNER(m_UseNamePrefix);
		BOOL_SETTING_ASSIGNER(m_EliminateNonAscii);
		// Reposition Settings
		BOOL_SETTING_ASSIGNER(m_Generate3dsMaxScript);
		BOOL_SETTING_ASSIGNER(m_GenerateBlenderScript);
		// Material Settings
		BOOL_SETTING_ASSIGNER(m_ExportMaterial);
		BOOL_SETTING_ASSIGNER(m_ExportTexture);
		BOOL_SETTING_ASSIGNER(m_CopyTexture);
		BOOL_SETTING_ASSIGNER(m_UseCustomTextureFormat);
		Utilities::SetCWndText(&m_CustomTextureFormat, config_manager.m_CustomTextureFormat.Get());
		// Encoding Settings
		PushCompositionEncoding(config_manager.m_CompositionEncoding.Get());
		Utilities::SetCWndText(&m_CustomEncoding, YYCC::ParserHelper::ToString(config_manager.m_CustomEncoding.Get()));
		BOOL_SETTING_ASSIGNER(m_UseUTF8ObjMtl);
		BOOL_SETTING_ASSIGNER(m_UseUTF8Script);
#undef BOOL_SETTING_ASSIGNER

		// update essential data
		UpdateAssocExportItems();
		UpdateDisableHierarchy();
		UpdateExportDirectory();

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void ExportSettingDialog::OnBtnOkClicked() {
		// ===== Validate input first =====
		// selected associated export item
		CK_ID associated_export_item = (CK_ID)0;
		{
			switch (PullExportMode()) {
				case DataTypes::ExportMode::Object:
				case DataTypes::ExportMode::Group:
				{
					// get associated item
					int selected_assoc_item_index = m_AssociatedExportItem.GetCurSel();
					if (selected_assoc_item_index == CB_ERR) {
						MessageBoxW(m_hWnd, L"You must specify an export target!", L"Setting Error", MB_OK + MB_ICONERROR);
						return;
					}
					associated_export_item = m_AssocExportItemListCache[static_cast<size_t>(selected_assoc_item_index)];
					break;
				}
				case DataTypes::ExportMode::Selected:
				case DataTypes::ExportMode::All:
					// these 2 method do not have associated item
					break;
			}
		}
		// export directory
		YYCC::yycc_u8string export_directory;
		{
			export_directory = m_ExportDirectoryCache;
			if (export_directory.empty()) {
				MessageBoxW(m_hWnd, L"Export directory should not be empty!", L"Setting Error", MB_OK + MB_ICONERROR);
				return;
			}
			if (!Utilities::CheckDirectoryExistence(export_directory)) {
				MessageBoxW(m_hWnd, L"Export directory must be existed!", L"Setting Error", MB_OK + MB_ICONERROR);
				return;
			}
		}
		// custom texture format
		YYCC::yycc_u8string custom_texture_format;
		{
			custom_texture_format = Utilities::GetCWndText(&m_CustomTextureFormat);
			// only check custom texture format when it is enabled
			if (RADIOBTN_GETCHECK(m_ExportMaterial) && RADIOBTN_GETCHECK(m_ExportTexture) && RADIOBTN_GETCHECK(m_UseCustomTextureFormat)) {
				if (custom_texture_format.empty()) {
					MessageBoxW(m_hWnd, L"Custom texture format should not be empty!", L"Setting Error", MB_OK + MB_ICONERROR);
					return;
				}
			}
		}
		// custom encoding
		UINT custom_encoding = CP_ACP;
		{
			// only check custom encoding when it is enabled
			if (RADIOBTN_GETCHECK(m_CompositionEncoding_Custom)) {
				YYCC::yycc_u8string custom_encoding_strl;
				Utilities::GetCWndText(&m_CustomEncoding);
				if (custom_encoding_strl.empty() || !YYCC::ParserHelper::TryParse<UINT>(custom_encoding_strl, custom_encoding) || !Utilities::ValidateCodePage(custom_encoding)) {
					MessageBoxW(m_hWnd, L"Custom encoding is invalid!", L"Setting Error", MB_OK + MB_ICONERROR);
					return;
				}
			}
		}

		// ===== Collect input settings =====
		DataTypes::ExportSetting export_setting;
#define BOOL_SETTING_EXTRACTOR(name) export_setting.name = RADIOBTN_GETCHECK(this->name)
		// General Settings
		export_setting.m_ExportMode = PullExportMode();
		export_setting.m_AssociatedExportItem = associated_export_item;
		export_setting.m_FileMode = PullFileMode();
		export_setting.m_ExportDirectory = export_directory;
		// Object Settings
		export_setting.m_ObjectSplitMode = PullObjectSplitMode();
		BOOL_SETTING_EXTRACTOR(m_IgnoreTransform);
		BOOL_SETTING_EXTRACTOR(m_ConvertToRightHand);
		BOOL_SETTING_EXTRACTOR(m_UseNamePrefix);
		BOOL_SETTING_EXTRACTOR(m_EliminateNonAscii);
		// Reposition Settings
		BOOL_SETTING_EXTRACTOR(m_Generate3dsMaxScript);
		BOOL_SETTING_EXTRACTOR(m_GenerateBlenderScript);
		// Material Settings
		BOOL_SETTING_EXTRACTOR(m_ExportMaterial);
		BOOL_SETTING_EXTRACTOR(m_ExportTexture);
		BOOL_SETTING_EXTRACTOR(m_CopyTexture);
		BOOL_SETTING_EXTRACTOR(m_UseCustomTextureFormat);
		export_setting.m_CustomTextureFormat = custom_texture_format;
		// Encoding Settings
		export_setting.m_CompositionEncoding = PullCompositionEncoding();
		export_setting.m_CustomEncoding = custom_encoding;
		BOOL_SETTING_EXTRACTOR(m_UseUTF8ObjMtl);
		BOOL_SETTING_EXTRACTOR(m_UseUTF8Script);
#undef BOOL_SETTING_EXTRACTOR

		// check done. sync settings to config manager
		auto& config_manager = vtobjplugin::ConfigManager::GetSingleton();
#define SETTING_ASSIGNER(name) config_manager.name.Set(export_setting.name)
		SETTING_ASSIGNER(m_ExportMode);
		SETTING_ASSIGNER(m_FileMode);
		SETTING_ASSIGNER(m_ExportDirectory);
		// Object Settings
		SETTING_ASSIGNER(m_ObjectSplitMode);
		SETTING_ASSIGNER(m_IgnoreTransform);
		SETTING_ASSIGNER(m_ConvertToRightHand);
		SETTING_ASSIGNER(m_UseNamePrefix);
		SETTING_ASSIGNER(m_EliminateNonAscii);
		// Reposition Settings
		SETTING_ASSIGNER(m_Generate3dsMaxScript);
		SETTING_ASSIGNER(m_GenerateBlenderScript);
		// Material Settings
		SETTING_ASSIGNER(m_ExportMaterial);
		SETTING_ASSIGNER(m_ExportTexture);
		SETTING_ASSIGNER(m_CopyTexture);
		SETTING_ASSIGNER(m_UseCustomTextureFormat);
		SETTING_ASSIGNER(m_CustomTextureFormat);
		// Encoding Settings
		SETTING_ASSIGNER(m_CompositionEncoding);
		SETTING_ASSIGNER(m_CustomEncoding);
		SETTING_ASSIGNER(m_UseUTF8ObjMtl);
		SETTING_ASSIGNER(m_UseUTF8Script);
#undef SETTING_ASSIGNER

		CDialogEx::OnOK();
	}

	void ExportSettingDialog::OnBtnCancelClicked() {
		CDialogEx::OnCancel();
	}

	void ExportSettingDialog::OnBtnResetClicked() {
		// create a default ExportSettings instance.
		static const DataTypes::ExportSetting c_DefaultExportSetting;

		// Assign values
#define BOOL_SETTING_ASSIGNER(name) RADIOBTN_SETCHECK(this->name, c_DefaultExportSetting.name)
		// General Settings
		PushExportMode(c_DefaultExportSetting.m_ExportMode);
		UpdateAssocExportItems();
		PushFileMode(c_DefaultExportSetting.m_FileMode);
		m_ExportDirectoryCache = c_DefaultExportSetting.m_ExportDirectory;
		// Object Settings
		PushObjectSplitMode(c_DefaultExportSetting.m_ObjectSplitMode);
		BOOL_SETTING_ASSIGNER(m_IgnoreTransform);
		BOOL_SETTING_ASSIGNER(m_ConvertToRightHand);
		BOOL_SETTING_ASSIGNER(m_UseNamePrefix);
		BOOL_SETTING_ASSIGNER(m_EliminateNonAscii);
		// Reposition Settings
		BOOL_SETTING_ASSIGNER(m_Generate3dsMaxScript);
		BOOL_SETTING_ASSIGNER(m_GenerateBlenderScript);
		// Material Settings
		BOOL_SETTING_ASSIGNER(m_ExportMaterial);
		BOOL_SETTING_ASSIGNER(m_ExportTexture);
		BOOL_SETTING_ASSIGNER(m_CopyTexture);
		BOOL_SETTING_ASSIGNER(m_UseCustomTextureFormat);
		Utilities::SetCWndText(&m_CustomTextureFormat, c_DefaultExportSetting.m_CustomTextureFormat);
		// Encoding Settings
		PushCompositionEncoding(c_DefaultExportSetting.m_CompositionEncoding);
		Utilities::SetCWndText(&m_CustomEncoding, YYCC::ParserHelper::ToString(c_DefaultExportSetting.m_CustomEncoding));
		BOOL_SETTING_ASSIGNER(m_UseUTF8ObjMtl);
		BOOL_SETTING_ASSIGNER(m_UseUTF8Script);
#undef BOOL_SETTING_ASSIGNER

		// update essential data
		UpdateAssocExportItems();
		UpdateDisableHierarchy();
		UpdateExportDirectory();

	}

	void ExportSettingDialog::OnBtnBrowseClicked() {
		// configure dialog properties
		YYCC::DialogHelper::FileDialog dialog_settings;
		dialog_settings.SetOwner(this->m_hWnd);

		// open directory picker and assign it if user click ok.
		YYCC::yycc_u8string picked_directory;
		if (YYCC::DialogHelper::OpenFolderDialog(dialog_settings, picked_directory)) {
			m_ExportDirectoryCache = picked_directory;
			UpdateExportDirectory();
		}
	}

	void ExportSettingDialog::OnBtnExportModeClicked() {
		// fetch current mode
		// only update associated item list when mode changed
		auto export_mode = PullExportMode();
		if (export_mode == m_ExportModeCache) return;

		// set to cache and update list
		m_ExportModeCache = export_mode;
		this->UpdateAssocExportItems();

		// when switching export mode, 
		// we also need update disable hierarchy
		this->UpdateDisableHierarchy();
	}

	void ExportSettingDialog::OnBtnDisableHierarchyClicked() {
		this->UpdateDisableHierarchy();
	}

#pragma endregion

#pragma region Pull Push Functions

	DataTypes::ExportMode ExportSettingDialog::PullExportMode() {
		if (RADIOBTN_GETCHECK(m_ExportMode_Object)) {
			return DataTypes::ExportMode::Object;
		} else if (RADIOBTN_GETCHECK(m_ExportMode_Group)) {
			return DataTypes::ExportMode::Group;
		} else if (RADIOBTN_GETCHECK(m_ExportMode_Selected)) {
			return DataTypes::ExportMode::Selected;
		} else if (RADIOBTN_GETCHECK(m_ExportMode_All)) {
			return DataTypes::ExportMode::All;
		} else {
			throw std::runtime_error("invalid export mode.");
		}
	}

	void ExportSettingDialog::PushExportMode(DataTypes::ExportMode export_mode) {
		RADIOBTN_SETCHECK(m_ExportMode_Object, false);
		RADIOBTN_SETCHECK(m_ExportMode_Group, false);
		RADIOBTN_SETCHECK(m_ExportMode_Selected, false);
		RADIOBTN_SETCHECK(m_ExportMode_All, false);

		switch (export_mode) {
			case DataTypes::ExportMode::Object:
				RADIOBTN_SETCHECK(m_ExportMode_Object, true);
				break;
			case DataTypes::ExportMode::Group:
				RADIOBTN_SETCHECK(m_ExportMode_Group, true);
				break;
			case DataTypes::ExportMode::Selected:
				RADIOBTN_SETCHECK(m_ExportMode_Selected, true);
				break;
			case DataTypes::ExportMode::All:
				RADIOBTN_SETCHECK(m_ExportMode_All, true);
				break;
			default:
				throw std::runtime_error("invalid export mode.");
		}
	}

	DataTypes::FileMode ExportSettingDialog::PullFileMode() {
		if (RADIOBTN_GETCHECK(m_FileMode_OneFile)) {
			return DataTypes::FileMode::OneFile;
		} else if (RADIOBTN_GETCHECK(m_FileMode_MultiFile)) {
			return DataTypes::FileMode::MultiFile;
		} else {
			throw std::runtime_error("invalid file mode.");
		}
	}

	void ExportSettingDialog::PushFileMode(DataTypes::FileMode file_mode) {
		RADIOBTN_SETCHECK(m_FileMode_OneFile, false);
		RADIOBTN_SETCHECK(m_FileMode_MultiFile, false);

		switch (file_mode) {
			case DataTypes::FileMode::OneFile:
				RADIOBTN_SETCHECK(m_FileMode_OneFile, true);
				break;
			case DataTypes::FileMode::MultiFile:
				RADIOBTN_SETCHECK(m_FileMode_MultiFile, true);
				break;
			default:
				throw std::runtime_error("invalid file mode.");
		}
	}

	DataTypes::ObjectSplitMode ExportSettingDialog::PullObjectSplitMode() {
		if (RADIOBTN_GETCHECK(m_ObjectSplitMode_Group)) {
			return DataTypes::ObjectSplitMode::Group;
		} else if (RADIOBTN_GETCHECK(m_ObjectSplitMode_Object)) {
			return DataTypes::ObjectSplitMode::Object;
		} else {
			throw std::runtime_error("invalid object split mode.");
		}
	}

	void ExportSettingDialog::PushObjectSplitMode(DataTypes::ObjectSplitMode object_split_mode) {
		RADIOBTN_SETCHECK(m_ObjectSplitMode_Group, false);
		RADIOBTN_SETCHECK(m_ObjectSplitMode_Object, false);

		switch (object_split_mode) {
			case DataTypes::ObjectSplitMode::Group:
				RADIOBTN_SETCHECK(m_ObjectSplitMode_Group, true);
				break;
			case DataTypes::ObjectSplitMode::Object:
				RADIOBTN_SETCHECK(m_ObjectSplitMode_Object, true);
				break;
			default:
				throw std::runtime_error("invalid object split mode.");
		}
	}

	DataTypes::CompositionEncoding ExportSettingDialog::PullCompositionEncoding() {
		if (RADIOBTN_GETCHECK(m_CompositionEncoding_System)) {
			return DataTypes::CompositionEncoding::System;
		} else if (RADIOBTN_GETCHECK(m_CompositionEncoding_Custom)) {
			return DataTypes::CompositionEncoding::Custom;
		} else {
			throw std::runtime_error("invalid composition encoding.");
		}
	}

	void ExportSettingDialog::PushCompositionEncoding(DataTypes::CompositionEncoding composition_encoding) {
		RADIOBTN_SETCHECK(m_CompositionEncoding_System, false);
		RADIOBTN_SETCHECK(m_CompositionEncoding_Custom, false);

		switch (composition_encoding) {
			case DataTypes::CompositionEncoding::System:
				RADIOBTN_SETCHECK(m_CompositionEncoding_System, true);
				break;
			case DataTypes::CompositionEncoding::Custom:
				RADIOBTN_SETCHECK(m_CompositionEncoding_Custom, true);
				break;
			default:
				throw std::runtime_error("invalid composition encoding.");
		}
	}

#pragma endregion

#pragma region Update Functions

	void ExportSettingDialog::UpdateAssocExportItems() {
		// clear UI and cache items
		m_AssocExportItemListCache.clear();
		m_AssociatedExportItem.ResetContent();

		// load items from CKContext
		XObjectPointerArray obj_array;
		switch (PullExportMode()) {
			case vtobjplugin::DataTypes::ExportMode::Object:
				obj_array = m_Context->GetObjectListByType(CKCID_3DENTITY, TRUE);
				break;
			case vtobjplugin::DataTypes::ExportMode::Group:
				obj_array = m_Context->GetObjectListByType(CKCID_GROUP, FALSE);
				break;
			case vtobjplugin::DataTypes::ExportMode::Selected:
			case vtobjplugin::DataTypes::ExportMode::All:
				// these 2 mode do not have list
				return;
		}

		// iterate object list to insert data into UI and cache
		// get the length of object array
		int obj_count = obj_array.Size();
		// reserve cache space first
		m_AssocExportItemListCache.reserve(static_cast<size_t>(obj_count));
		// iterate list
		for (int i = 0; i < obj_count; ++i) {
			// push into cache
			m_AssocExportItemListCache.emplace_back(obj_array[i]->GetID());
			// push into UI
			if (obj_array[i]->GetName() != NULL)
				// if it has name, use it directly
				m_AssociatedExportItem.AddString(obj_array[i]->GetName());
			else {
				// if it is no name, generate a fallback name
				YYCC::yycc_u8string u8_fallback_name(YYCC::StringHelper::Printf(YYCC_U8("[Unnamed Object] (CKID: %u)"), obj_array[i]->GetID()));
				m_AssociatedExportItem.AddString(YYCC::EncodingHelper::CharToChar(
					YYCC::EncodingHelper::ToOrdinary(u8_fallback_name), CP_UTF8, CP_ACP
				).c_str());
			}
		}
	}

	void ExportSettingDialog::UpdateDisableHierarchy() {
		// ===== General Settings =====
		// Associated item box only should be shown in object mode or group mode
		m_AssociatedExportItem.EnableWindow(
			RADIOBTN_GETCHECK(m_ExportMode_Object) || RADIOBTN_GETCHECK(m_ExportMode_Group)
		);

		// ===== Reposition Settings =====
		// Reposition settings only available when Ignore Transform and Convert to Right Hand enabled.
		BOOL enable_reposition = RADIOBTN_GETCHECK(m_IgnoreTransform) && RADIOBTN_GETCHECK(m_ConvertToRightHand);
		m_Generate3dsMaxScript.EnableWindow(enable_reposition);
		m_GenerateBlenderScript.EnableWindow(enable_reposition);

		// ===== Material Settings =====
		// Only checking Export Material, all other settings are available
		BOOL enable_material = RADIOBTN_GETCHECK(m_ExportMaterial);
		m_ExportTexture.EnableWindow(enable_material);
		// Only checking Export Texture, all texture settings are available.
		BOOL enable_texture = enable_material && RADIOBTN_GETCHECK(m_ExportTexture);
		m_CopyTexture.EnableWindow(enable_texture);
		m_UseCustomTextureFormat.EnableWindow(enable_texture);
		// Only checking Use Custom Texture Format, Custom Texture Format is available.
		BOOL enable_custom_texture_format = enable_texture && RADIOBTN_GETCHECK(m_UseCustomTextureFormat);
		m_CustomTextureFormat.EnableWindow(enable_custom_texture_format);

		// ===== Encoding Settings =====
		// Custom Encoding depends on checking Custom in Composition Encoding
		m_CustomEncoding.EnableWindow(RADIOBTN_GETCHECK(m_CompositionEncoding_Custom));
	}

	void ExportSettingDialog::UpdateExportDirectory() {
		// update text
		Utilities::SetCWndText(&m_ExportDirectory, m_ExportDirectoryCache);
	}

#pragma endregion

}


