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

	BOOL ExportSettingDialog::OnInitDialog() {
		CDialogEx::OnInitDialog();

		// Read settings from config manager
		auto& config_manager = vtobjplugin::ConfigManager::GetSingleton();
#define BOOL_SETTING_ASSIGNER(name) this->name.SetCheck(config_manager.name.Get() ? BST_CHECKED : BST_UNCHECKED)
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

		CDialogEx::OnOK();
	}

	void ExportSettingDialog::OnBtnCancelClicked() {
		CDialogEx::OnCancel();
	}

	void ExportSettingDialog::OnBtnResetClicked() {
		// create a default ExportSettings instance.
		static const DataTypes::ExportSetting c_DefaultExportSetting;

		// Assign values
#define BOOL_SETTING_ASSIGNER(name) this->name.SetCheck(c_DefaultExportSetting.name ? BST_CHECKED : BST_UNCHECKED)
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
			m_ExportMode_Object.GetCheck() || m_ExportMode_Group.GetCheck()
		);

		// ===== Reposition Settings =====
		// Reposition settings only available when Ignore Transform and Convert to Right Hand enabled.
		BOOL enable_reposition = m_IgnoreTransform.GetCheck() && m_ConvertToRightHand.GetCheck();
		m_Generate3dsMaxScript.EnableWindow(enable_reposition);
		m_GenerateBlenderScript.EnableWindow(enable_reposition);

		// ===== Material Settings =====
		// Only checking Export Material, all other settings are available
		BOOL enable_material = m_ExportMaterial.GetCheck();
		m_ExportTexture.EnableWindow(enable_material);
		// Only checking Export Texture, all texture settings are available.
		BOOL enable_texture = enable_material && m_ExportTexture.GetCheck();
		m_CopyTexture.EnableWindow(enable_texture);
		m_UseCustomTextureFormat.EnableWindow(enable_texture);
		// Only checking Use Custom Texture Format, Custom Texture Format is available.
		BOOL enable_custom_texture_format = enable_texture && m_UseCustomTextureFormat.GetCheck();
		m_CustomTextureFormat.EnableWindow(enable_custom_texture_format);

		// ===== Encoding Settings =====
		// Custom Encoding depends on checking Custom in Composition Encoding
		m_CustomEncoding.EnableWindow(m_CompositionEncoding_Custom.GetCheck());
	}

	void ExportSettingDialog::UpdateExportDirectory() {
		// update text
		Utilities::SetCWndText(&m_ExportDirectory, m_ExportDirectoryCache);
	}

#pragma region Pull Push Functions

	DataTypes::ExportMode ExportSettingDialog::PullExportMode() {
		if (m_ExportMode_Object.GetCheck() == BST_CHECKED) {
			return DataTypes::ExportMode::Object;
		} else if (m_ExportMode_Group.GetCheck() == BST_CHECKED) {
			return DataTypes::ExportMode::Group;
		} else if (m_ExportMode_Selected.GetCheck() == BST_CHECKED) {
			return DataTypes::ExportMode::Selected;
		} else if (m_ExportMode_All.GetCheck() == BST_CHECKED) {
			return DataTypes::ExportMode::All;
		} else {
			throw std::runtime_error("invalid export mode.");
		}
	}

	void ExportSettingDialog::PushExportMode(DataTypes::ExportMode export_mode) {
		m_ExportMode_Object.SetCheck(BST_UNCHECKED);
		m_ExportMode_Group.SetCheck(BST_UNCHECKED);
		m_ExportMode_Selected.SetCheck(BST_UNCHECKED);
		m_ExportMode_All.SetCheck(BST_UNCHECKED);

		switch (export_mode) {
			case DataTypes::ExportMode::Object:
				m_ExportMode_Object.SetCheck(BST_CHECKED);
				break;
			case DataTypes::ExportMode::Group:
				m_ExportMode_Group.SetCheck(BST_CHECKED);
				break;
			case DataTypes::ExportMode::Selected:
				m_ExportMode_Selected.SetCheck(BST_CHECKED);
				break;
			case DataTypes::ExportMode::All:
				m_ExportMode_All.SetCheck(BST_CHECKED);
				break;
			default:
				throw std::runtime_error("invalid export mode.");
		}
	}

	DataTypes::FileMode ExportSettingDialog::PullFileMode() {
		if (m_FileMode_OneFile.GetCheck() == BST_CHECKED) {
			return DataTypes::FileMode::OneFile;
		} else if (m_FileMode_MultiFile.GetCheck() == BST_CHECKED) {
			return DataTypes::FileMode::MultiFile;
		} else {
			throw std::runtime_error("invalid file mode.");
		}
	}

	void ExportSettingDialog::PushFileMode(DataTypes::FileMode file_mode) {
		m_FileMode_OneFile.SetCheck(BST_UNCHECKED);
		m_FileMode_MultiFile.SetCheck(BST_UNCHECKED);

		switch (file_mode) {
			case DataTypes::FileMode::OneFile:
				m_FileMode_OneFile.SetCheck(BST_CHECKED);
				break;
			case DataTypes::FileMode::MultiFile:
				m_FileMode_MultiFile.SetCheck(BST_CHECKED);
				break;
			default:
				throw std::runtime_error("invalid file mode.");
		}
	}

	DataTypes::ObjectSplitMode ExportSettingDialog::PullObjectSplitMode() {
		if (m_ObjectSplitMode_Group.GetCheck() == BST_CHECKED) {
			return DataTypes::ObjectSplitMode::Group;
		} else if (m_ObjectSplitMode_Object.GetCheck() == BST_CHECKED) {
			return DataTypes::ObjectSplitMode::Object;
		} else {
			throw std::runtime_error("invalid object split mode.");
		}
	}

	void ExportSettingDialog::PushObjectSplitMode(DataTypes::ObjectSplitMode object_split_mode) {
		m_ObjectSplitMode_Group.SetCheck(BST_UNCHECKED);
		m_ObjectSplitMode_Object.SetCheck(BST_UNCHECKED);

		switch (object_split_mode) {
			case DataTypes::ObjectSplitMode::Group:
				m_ObjectSplitMode_Group.SetCheck(BST_CHECKED);
				break;
			case DataTypes::ObjectSplitMode::Object:
				m_ObjectSplitMode_Object.SetCheck(BST_CHECKED);
				break;
			default:
				throw std::runtime_error("invalid object split mode.");
		}
	}

	DataTypes::CompositionEncoding ExportSettingDialog::PullCompositionEncoding() {
		if (m_CompositionEncoding_System.GetCheck() == BST_CHECKED) {
			return DataTypes::CompositionEncoding::System;
		} else if (m_CompositionEncoding_Custom.GetCheck() == BST_CHECKED) {
			return DataTypes::CompositionEncoding::Custom;
		} else {
			throw std::runtime_error("invalid composition encoding.");
		}
	}

	void ExportSettingDialog::PushCompositionEncoding(DataTypes::CompositionEncoding composition_encoding) {
		m_CompositionEncoding_System.SetCheck(BST_UNCHECKED);
		m_CompositionEncoding_Custom.SetCheck(BST_UNCHECKED);

		switch (composition_encoding) {
			case DataTypes::CompositionEncoding::System:
				m_CompositionEncoding_System.SetCheck(BST_CHECKED);
				break;
			case DataTypes::CompositionEncoding::Custom:
				m_CompositionEncoding_Custom.SetCheck(BST_CHECKED);
				break;
			default:
				throw std::runtime_error("invalid composition encoding.");
		}
	}

#pragma endregion

}


