#include "ExportLayoutWeaver.hpp"
#include <set>
#include <numeric>
#include <stdexcept>
#include <cstdarg>

namespace vtobjplugin {

	class UniqueName {
	public:
		UniqueName(const Utilities::VirtoolsUIReporter& reporter) :
			m_NameSet(), m_Reporter(reporter),
			m_StringLoader(Utilities::StringLoader::GetSingleton()) {}
		~UniqueName() {}

	private:
		std::set<YYCC::yycc_u8string> m_NameSet;
		const Utilities::VirtoolsUIReporter& m_Reporter;
		const Utilities::StringLoader& m_StringLoader;
	public:
		/**
		 * @brief Generate unique name from expected name.
		 * @param[in] expected_name The expected name.
		 * @return Final unique name.
		*/
		YYCC::yycc_u8string GetName(const YYCC::yycc_u8string_view& _expected_name) {
			// try first insert
			YYCC::yycc_u8string expected_name(_expected_name);
			YYCC::yycc_u8string ret = expected_name;
			auto result = m_NameSet.emplace(ret);
			if (result.second) return ret;

			// fail at the first inserting
			// try building unqiue name
			uint64_t i = 0;
			while (true) {
				// check limitation
				if (i == std::numeric_limits<uint64_t>::max())
					throw std::runtime_error("unique name exceed the max index.");
				// build unique name
				if (!YYCC::StringHelper::Printf(ret, YYCC_U8("%s_%03" PRIu64), expected_name.c_str(), i))
					throw std::runtime_error("fail to format unique name.");
				// try inserting
				auto result = m_NameSet.emplace(ret);
				if (result.second) break;
			}
			// indicate final name is different with expected name
			m_Reporter.Format(m_StringLoader.LoadStringU8(IDS_UNAME_RENAME).c_str(), expected_name.c_str(), ret.c_str());
			return ret;
		}
	};

#pragma region Export Layout Weaver

	ExportLayoutWeaver::ExportLayoutWeaver(
		PluginInterface* plugin_interface,
		const DataTypes::ExportSetting& export_setting,
		const Utilities::VirtoolsUIReporter& reporter) :
		m_PluginInterface(plugin_interface),
		m_Context(plugin_interface->GetCKContext()),
		m_ExportSetting(export_setting),
		m_Reporter(reporter),
		m_StringLoader(Utilities::StringLoader::GetSingleton()) {}

	ExportLayoutWeaver::~ExportLayoutWeaver() {}

	const std::deque<ExportLayoutWeaver::File_t>& ExportLayoutWeaver::GetFileList() const {
		return this->m_FileList;
	}

	const std::map<CKTexture*, YYCC::yycc_u8string>& ExportLayoutWeaver::GetTextureMap() const {
		return this->m_TextureMap;
	}

	void ExportLayoutWeaver::WeaveLayout() {
		// collect object and generate their names
		std::deque<ObjectPair_t> object_pairs;
		CollectObject(object_pairs);
		// distribute these objects into files
		DistributeObject(object_pairs);
	}

	void ExportLayoutWeaver::CollectObject(std::deque<ExportLayoutWeaver::ObjectPair_t>& ret) {// collect all exported 3d object by export type
		switch (m_ExportSetting.m_ExportMode) {
			case DataTypes::ExportMode::All:
			{
				XObjectPointerArray obj_array = m_Context->GetObjectListByType(CKCID_3DENTITY, TRUE);
				int obj_count = obj_array.Size();
				for (int i = 0; i < obj_count; ++i) {
					ret.emplace_back(
						std::make_pair(static_cast<CK3dEntity*>(obj_array[i]), YYCC::yycc_u8string())
					);
				}
				break;
			}
			case DataTypes::ExportMode::Selected:
			{
				CKObjectArray* obj_array = m_PluginInterface->Get3dSelection();
				for (obj_array->Reset(); !obj_array->EndOfList(); obj_array->Next()) {
					// get object pointer
					CKObject* obj = obj_array->GetData(m_Context);
					// check object
					if (obj == nullptr || !CKIsChildClassOf(obj, CKCID_3DENTITY))
						continue;
					// insert
					ret.emplace_back(
						std::make_pair(static_cast<CK3dEntity*>(obj), YYCC::yycc_u8string())
					);
				}
				break;
			}
			case DataTypes::ExportMode::Group:
			{
				// try fetching CKGroup
				CKObject* pending_group = m_Context->GetObjectA(m_ExportSetting.m_AssociatedExportItem);
				if (pending_group == nullptr || !CKIsChildClassOf(pending_group, CKCID_GROUP))
					throw std::runtime_error("unexpected invalid associated export item.");
				CKGroup* group = static_cast<CKGroup*>(pending_group);
				// iterate group
				int obj_count = group->GetObjectCount();
				for (int i = 0; i < obj_count; ++i) {
					// check object member type then insert it.
					CKBeObject* obj = group->GetObjectA(i);
					if (CKIsChildClassOf(obj, CKCID_3DENTITY)) {
						ret.emplace_back(
							std::make_pair(static_cast<CK3dEntity*>(obj), YYCC::yycc_u8string())
						);
					}
				}
				break;
			}
			case DataTypes::ExportMode::Object:
			{
				// try fetching CK3dEntity
				CKObject* pending_3dentity = m_Context->GetObjectA(m_ExportSetting.m_AssociatedExportItem);
				if (pending_3dentity == nullptr || !CKIsChildClassOf(pending_3dentity, CKCID_3DENTITY))
					throw std::runtime_error("unexpected invalid associated export item.");
				// insert
				ret.emplace_back(
					std::make_pair(static_cast<CK3dEntity*>(pending_3dentity), YYCC::yycc_u8string())
				);
				break;
			}
			default:
				throw std::runtime_error("invalid export mode.");
		}

		// give them an unique name respectively
		UniqueName object_name_set(m_Reporter);
		for (auto& pair : ret) {
			// get name
			YYCC::yycc_u8string ckobj_name(GetCKObjectName(pair.first));
			// correct name
			CorrectObjectName(ckobj_name);
			// get unique name from expected name
			pair.second = object_name_set.GetName(ckobj_name);
		}
	}

	void ExportLayoutWeaver::DistributeObject(const std::deque<ExportLayoutWeaver::ObjectPair_t>& object_pairs) {
		// Distribute object into file
		switch (m_ExportSetting.m_FileMode) {
			case DataTypes::FileMode::MultiFile:
			{
				// create file for every object
				for (const auto& object_pair : object_pairs) {
					// create file instance
					File_t file;
					// build file name with extra correction
					YYCC::yycc_u8string filename(object_pair.second);
					CorrectPathName(filename);
					file.m_ObjFileName = filename + YYCC_U8(".obj");
					file.m_MtlFileName = filename + YYCC_U8(".mtl");
					// add object into it.
					file.m_ObjectList.emplace_back(object_pair);
					// insert to list
					m_FileList.emplace_back(std::move(file));
				}
				break;
			}
			case DataTypes::FileMode::OneFile:
			{
				// build single file data
				File_t single_file;
				YYCC::yycc_u8string filename = YYCC_U8("all");
				single_file.m_ObjFileName = filename + YYCC_U8(".obj");
				single_file.m_MtlFileName = filename + YYCC_U8(".mtl");
				single_file.m_ObjectList = object_pairs;
				// insert to list
				m_FileList.emplace_back(std::move(single_file));
				break;
			}
			default:
				throw std::runtime_error("invalid file mode.");
		}

		// Fulfill material data if necessary
		if (m_ExportSetting.CanExportMaterial()) {
			for (auto& file : m_FileList) {
				// material unique name
				UniqueName material_name_set(m_Reporter);

				// iterate object for every file entry
				for (const auto& object_pair : file.m_ObjectList) {
					// get mesh from 3d object and validate it.
					CKMesh* mesh = object_pair.first->GetCurrentMesh();
					if (mesh == nullptr) { // skip no associated mesh
						m_Reporter.Write(object_pair.first, m_StringLoader.LoadStringU8(IDS_EXPLAYOUT_OBJ_NO_MESH).c_str());
						continue;
					}
					if (mesh->GetFaceCount() == 0) { // skip zero face mesh
						m_Reporter.Write(object_pair.first, m_StringLoader.LoadStringU8(IDS_EXPLAYOUT_MESH_NO_FACE).c_str());
						continue;
					}

					// extract used material from this mesh
					// and give them an unique name.
					int material_count = mesh->GetMaterialCount();
					for (int i = 0; i < material_count; ++i) {
						// get material
						CKMaterial* material = mesh->GetMaterial(i);
						if (material == nullptr) continue;
						if (file.m_MaterialMap.find(material) != file.m_MaterialMap.end()) continue;

						// get name
						YYCC::yycc_u8string ckobj_name(GetCKObjectName(material));
						// correct name
						CorrectObjectName(ckobj_name);
						// get unique name from expected name 
						// and insert to material map
						file.m_MaterialMap.emplace(
							std::make_pair(material, material_name_set.GetName(ckobj_name))
						);
					}
				}
			}
		}

		// Fulfill texture data if necessary
		if (m_ExportSetting.CanExportTexture()) {
			// texture file unique file name
			UniqueName texture_name_set(m_Reporter);
			// iterate file list
			for (const auto& file : m_FileList) {
				// iterate material list and extract texture infomations
				for (const auto& pair : file.m_MaterialMap) {
					// get texture and validate it.
					CKTexture* texture = pair.first->GetTexture();
					if (texture == nullptr) continue;
					if (m_TextureMap.find(texture) != m_TextureMap.end()) continue;
					if (texture->GetSlotFileName(0) == nullptr) { // skip if no slot file name
						m_Reporter.Write(texture, m_StringLoader.LoadStringU8(IDS_EXPLAYOUT_TEX_NO_SLOT_FNAME).c_str());
						continue;
					}

					// fetch associated texture path from virtools with given encoding
					std::filesystem::path texture_path(YYCC::FsPathPatch::FromUTF8Path(
						YYCC::EncodingHelper::CharToUTF8(texture->GetSlotFileName(0), m_ExportSetting.GetCompositionEncoding()).c_str()
					));
					// extract stem and extension part from given path
					YYCC::yycc_u8string filename_stem(YYCC::FsPathPatch::ToUTF8Path(texture_path.stem())),
						filename_extension(YYCC::FsPathPatch::ToUTF8Path(texture_path.extension()));

					// correct stem part of given file.
					// because file name also need to be written in mtl file
					// so it also need to be corrected for object
					CorrectObjectName(filename_stem);
					CorrectPathName(filename_stem);

					// get unique name of stem
					YYCC::yycc_u8string filename = texture_name_set.GetName(filename_stem);

					// add extra extensions accoridng whether use custom texture format
					if (m_ExportSetting.CanUseCustomTextureFormat()) {
						// use custom one
						filename += YYCC_U8(".");
						filename += m_ExportSetting.m_CustomTextureFormat;
					} else {
						// add old part
						filename += filename_extension;
					}

					// insert to map
					m_TextureMap.emplace(std::make_pair(texture, std::move(filename)));
				}
			}
		}
	}

	static YYCC::yycc_u8string GenerateCKObjectName(CKObject* ckobj) {
		return YYCC::StringHelper::Printf(YYCC_U8("UnamedObj_%" PRIu32), ckobj->GetID());
	}
	YYCC::yycc_u8string ExportLayoutWeaver::GetCKObjectName(CKObject* ckobj) {
		// setup return value
		YYCC::yycc_u8string ret;

		// if given object do not has name, generate one.
		// otherwise use its original name
		CKSTRING ckobj_name = ckobj->GetName();
		if (ckobj != nullptr) {
			// use current name.
			// if fail to convert encoding, fallback to use generated name.
			if (!YYCC::EncodingHelper::CharToUTF8(ckobj_name, ret, m_ExportSetting.GetCompositionEncoding())) {
				m_Reporter.Write(ckobj, m_StringLoader.LoadStringU8(IDS_EXPLAYOUT_NAME_CONV_FAIL).c_str());
				ret = GenerateCKObjectName(ckobj);
			}
		} else {
			// generate one
			m_Reporter.Write(ckobj, m_StringLoader.LoadStringU8(IDS_EXPLAYOUT_NAME_UNAMED).c_str());
			ret = GenerateCKObjectName(ckobj);
		}

		// add prefix string if necessary
		if (m_ExportSetting.m_UseNamePrefix) {
			ret.insert(0u, YYCC::StringHelper::Printf(YYCC_U8("Obj%" PRIu32 "_"), ckobj->GetID()));
		}

		return ret;
	}

	void ExportLayoutWeaver::CorrectObjectName(YYCC::yycc_u8string& name) {
		for (auto& chr : name) {
			// check eliminate non-ascii
			if (m_ExportSetting.m_EliminateNonAscii && (chr & YYCC_U8_CHAR(0b10000000))) {
				chr = YYCC_U8_CHAR('_');
				continue;
			}

			switch (chr) {
				case YYCC_U8_CHAR('\''): // blender & 3dsmax script ban
				case YYCC_U8_CHAR('"'):
				case YYCC_U8_CHAR('\\'):
				case YYCC_U8_CHAR(' '): // wavefront obj ban
					chr = YYCC_U8_CHAR('_');
					break;
			}
		}
	}

	void ExportLayoutWeaver::CorrectPathName(YYCC::yycc_u8string& name) {
		for (auto& chr : name) {
			// path do not need to remove non-ASCII characters

			switch (chr) {
				case YYCC_U8_CHAR('\\'): //file system ban
				case YYCC_U8_CHAR('/'):
				case YYCC_U8_CHAR(':'):
				case YYCC_U8_CHAR('*'):
				case YYCC_U8_CHAR('?'):
				case YYCC_U8_CHAR('"'):
				case YYCC_U8_CHAR('<'):
				case YYCC_U8_CHAR('>'):
				case YYCC_U8_CHAR('|'):
					chr = YYCC_U8_CHAR('_');
					break;
			}
		}
	}

#pragma endregion

}
