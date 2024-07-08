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
		ON_BN_CLICKED(IDOK, &ExportSettingDialog::OnBtnOkClicked)
		ON_BN_CLICKED(IDCANCEL, &ExportSettingDialog::OnBtnCancelClicked)
		ON_BN_CLICKED(IDC_BUTTON2, &ExportSettingDialog::OnBtnResetClicked)
		ON_BN_CLICKED(IDC_BUTTON1, &ExportSettingDialog::OnBtnBrowseClicked)
		ON_BN_CLICKED(IDC_BUTTON1, &ExportSettingDialog::OnBtnCancelClicked)


	END_MESSAGE_MAP()


	// ExportSettingDialog message handlers

	BOOL ExportSettingDialog::OnInitDialog() {
		CDialogEx::OnInitDialog();

		// Read settings from config manager
		auto& config_manager = vtobjplugin::ConfigManager::GetSingleton();
#define BOOL_SETTING_ASSIGNER(name) this->name.SetCheck(config_manager.name.Get() ? 1 : 0)
		// General Settings
		switch (config_manager.m_ExportMode.Get()) {
			case DataTypes::ExportMode::Object:
				m_ExportMode_Object.SetCheck(1);
				break;
			case DataTypes::ExportMode::Group:
				m_ExportMode_Group.SetCheck(1);
				break;
			case DataTypes::ExportMode::Selected:
				m_ExportMode_Selected.SetCheck(1);
				break;
			case DataTypes::ExportMode::All:
				m_ExportMode_All.SetCheck(1);
				break;
			default:
				throw std::runtime_error("invalid export mode.");
		}
		UpdateAssocExportItems();
		switch (config_manager.m_FileMode.Get()) {
			case DataTypes::FileMode::OneFile:
				m_FileMode_OneFile.SetCheck(1);
				break;
			case DataTypes::FileMode::MultiFile:
				m_FileMode_MultiFile.SetCheck(1);
				break;
			default:
				throw std::runtime_error("invalid file mode.");
		}
		UpdateExportDirectory(config_manager.m_ExportDirectory.Get());
		// Object Settings
		switch (config_manager.m_ObjectSplitMode.Get()) {
			case DataTypes::ObjectSplitMode::Group:
				m_ObjectSplitMode_Group.SetCheck(1);
				break;
			case DataTypes::ObjectSplitMode::Object:
				m_ObjectSplitMode_Object.SetCheck(1);
				break;
			default:
				throw std::runtime_error("invalid object split mode.");
		}
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
		switch (config_manager.m_CompositionEncoding.Get()) {
			case DataTypes::CompositionEncoding::System:
				m_CompositionEncoding_System.SetCheck(1);
				break;
			case DataTypes::CompositionEncoding::Custom:
				m_CompositionEncoding_Custom.SetCheck(1);
				break;
			default:
				throw std::runtime_error("invalid composition encoding.");
		}
		Utilities::SetCWndText(&m_CustomEncoding, YYCC::ParserHelper::ToString(config_manager.m_CustomEncoding.Get()));
		BOOL_SETTING_ASSIGNER(m_UseUTF8ObjMtl);
		BOOL_SETTING_ASSIGNER(m_UseUTF8Script);
#undef BOOL_SETTING_ASSIGNER

		// update disable hierarchy
		UpdateDisableHierarchy();

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

	}

	void ExportSettingDialog::OnBtnBrowseClicked() {
		// configure dialog properties
		YYCC::DialogHelper::FileDialog dialog_settings;
		dialog_settings.SetOwner(this->m_hWnd);

		// open directory picker and assign it if user click ok.
		YYCC::yycc_u8string picked_directory;
		if (YYCC::DialogHelper::OpenFolderDialog(dialog_settings, picked_directory)) {
			UpdateExportDirectory(picked_directory);
		}
	}


	void ExportSettingDialog::UpdateAssocExportItems() {
		// get current export mode
		DataTypes::ExportMode export_mode;
		if (m_ExportMode_Object.GetCheck()) {
			export_mode = DataTypes::ExportMode::Object;
		} else if (m_ExportMode_Group.GetCheck()) {
			export_mode = DataTypes::ExportMode::Group;
		} else if (m_ExportMode_Selected.GetCheck()) {
			export_mode = DataTypes::ExportMode::Selected;
		} else if (m_ExportMode_All.GetCheck()) {
			export_mode = DataTypes::ExportMode::All;
		} else {
			throw std::runtime_error("invalid export mode.");
		}
		// comparing with export mode cache.
		// update list only if value changed.
		if (export_mode == m_ExportModeCache) return;
		m_ExportModeCache = export_mode;

		// clear UI and cache items
		m_AssocExportItemListCache.clear();
		m_AssociatedExportItem.ResetContent();

		// load items
		switch (export_mode) {
			case vtobjplugin::DataTypes::ExportMode::Object:
			{
				// load object list
				// fetch object list data
				XObjectPointerArray obj_array = m_Context->GetObjectListByType(CKCID_3DENTITY, TRUE);
				int obj_count = obj_array.Size();
				m_AssocExportItemListCache.reserve(static_cast<size_t>(obj_count));
				for (int i = 0; i < obj_count; ++i) {
					// push into cache
					m_AssocExportItemListCache.emplace_back(obj_array[i]->GetID());
					if (obj_array[i]->GetName() != NULL)
						m_AssociatedExportItem.AddString(obj_array[i]->GetName());
					else {
						YYCC::yycc_u8string u8_unname(YYCC::StringHelper::Printf(YYCC_U8("[unnamed 3d object] (CKID: %u)"), obj_array[i]->GetID()));
						m_AssociatedExportItem.AddString(YYCC::EncodingHelper::CharToChar(
							YYCC::EncodingHelper::ToOrdinary(u8_unname), CP_UTF8, CP_ACP
						).c_str());
					}
				}
				break;
			}
			case vtobjplugin::DataTypes::ExportMode::Group:
			{
				// load group list
				break;
			}
			case vtobjplugin::DataTypes::ExportMode::Selected:
			case vtobjplugin::DataTypes::ExportMode::All:
				break;
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

	void ExportSettingDialog::UpdateExportDirectory(const YYCC::yycc_u8string& export_dir) {
		// update internal value
		m_ExportDirectoryCache = export_dir;
		// update text
		Utilities::SetCWndText(&m_ExportDirectory, m_ExportDirectoryCache);
	}

}


