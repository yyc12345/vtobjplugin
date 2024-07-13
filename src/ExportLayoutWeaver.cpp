#include "ExportLayoutWeaver.hpp"
#include <set>
#include <numeric>
#include <stdexcept>
#include <cstdarg>

namespace vtobjplugin {

#pragma region Virtools UI Reporter

	VirtoolsUIReporter::VirtoolsUIReporter(CKContext* context) :
		m_Context(context) {}

	VirtoolsUIReporter::~VirtoolsUIReporter() {}

	void VirtoolsUIReporter::Write(const YYCC::yycc_char8_t* strl) const {
		RawWrite(YYCC::EncodingHelper::UTF8ToChar(strl, CP_ACP).c_str());
	}

	void VirtoolsUIReporter::Write(CKObject* associated_obj, const YYCC::yycc_char8_t* strl) const {
		std::string buffer;

		// add associated object header
		if (associated_obj != nullptr) {
			// obj marker head
			buffer += "[";

			// object marker name part
			CKSTRING assoc_obj_name = associated_obj->GetName();
			if (assoc_obj_name != nullptr) {
				buffer += assoc_obj_name;
				buffer += " ";
			}

			// object marker id part
			buffer += YYCC::EncodingHelper::UTF8ToChar(
				YYCC::StringHelper::Printf(YYCC_U8("<ID: %>" PRIu32), associated_obj->GetID()),
				CP_ACP
			);

			// obj marker tail
			buffer += "] ";
		}

		// append real output data
		buffer += YYCC::EncodingHelper::UTF8ToChar(strl, CP_ACP);
		RawWrite(buffer.c_str());
	}

	void VirtoolsUIReporter::Format(const YYCC::yycc_char8_t* fmt, ...) const {
		va_list argptr;
		va_start(argptr, fmt);
		this->Write(YYCC::StringHelper::VPrintf(fmt, argptr).c_str());
		va_end(argptr);
	}

	void VirtoolsUIReporter::Format(CKObject* associated_obj, const YYCC::yycc_char8_t* fmt, ...) const {
		va_list argptr;
		va_start(argptr, fmt);
		this->Write(associated_obj, YYCC::StringHelper::VPrintf(fmt, argptr).c_str());
		va_end(argptr);
	}

	void VirtoolsUIReporter::RawWrite(const char* raw_strl) const {
		if (m_Context != nullptr && raw_strl != nullptr) {
			m_Context->OutputToConsole(
				const_cast<CKSTRING>(raw_strl),
				FALSE
			);
		}
	}

#pragma endregion

	class UniqueName {
	public:
		UniqueName(const VirtoolsUIReporter& reporter) :
			m_NameSet(), m_Reporter(reporter) {}
		~UniqueName() {}

	private:
		std::set<YYCC::yycc_u8string> m_NameSet;
		const VirtoolsUIReporter& m_Reporter;
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
			m_Reporter.Format(YYCC_U8("Name \"%s\" was renamed to \"%s\" for unique name."), expected_name.c_str(), ret.c_str());
			return ret;
		}
	};

#pragma region Export Layout Weaver

	ExportLayoutWeaver::ExportLayoutWeaver(
		PluginInterface* plugin_interface,
		const DataTypes::ExportSetting& export_setting) :
		m_PluginInterface(plugin_interface),
		m_Context(plugin_interface->GetCKContext()),
		m_ExportSetting(export_setting),
		m_Reporter(m_Context) {
		WeaveLayout();
	}

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
					file.m_FileName = filename;
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
				single_file.m_FileName = YYCC_U8("all");
				single_file.m_ObjectList = object_pairs;
				// insert to list
				m_FileList.emplace_back(std::move(single_file));
				break;
			}
			default:
				throw std::runtime_error("invalid file mode.");
		}

		// Fulfill material and texture data
		for (auto& file : m_FileList) {
			// material unique name
			UniqueName material_name_set(m_Reporter);

			// iterate object for every file entry
			for (const auto& object_pair : file.m_ObjectList) {
				// get mesh from 3d object
				// if no associated mesh, skip this object.
				CKMesh* mesh = object_pair.first->GetCurrentMesh();
				if (mesh == nullptr) continue;

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
					// insert to material map
					file.m_MaterialMap.emplace(std::make_pair(material, std::move(ckobj_name)));
				}
			}

			// iterate material list and extract texture infomations
			for (const auto& pair : file.m_MaterialMap) {
				// get texture
				CKTexture* texture = pair.first->GetTexture();
				if (texture == nullptr) continue;
				if (m_TextureMap.find(texture) != m_TextureMap.end()) continue;

				// extract file name part
				std::filesystem::path texture_path(texture->GetSlotFileName(0));
				YYCC::yycc_u8string filename(YYCC::FsPathPatch::ToUTF8Path(texture_path.filename()));

				// add extra extensions
				if (m_ExportSetting.m_UseCustomTextureFormat) {
					filename += YYCC_U8(".");
					filename += m_ExportSetting.m_CustomTextureFormat;
				}

				// correct path name
				// because file name also need to be written in mtl file
				// so it also need to be corrected for object
				CorrectObjectName(filename);
				CorrectPathName(filename);

				// insert to map
				m_TextureMap.emplace(std::make_pair(texture, std::move(filename)));
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
			// get encoding
			UINT encoding = m_ExportSetting.m_CompositionEncoding == DataTypes::CompositionEncoding::Custom ? m_ExportSetting.m_CustomEncoding : CP_ACP;
			// if fail to convert encoding, fallback to use generated name.
			if (!YYCC::EncodingHelper::CharToUTF8(ckobj_name, ret, encoding)) {
				m_Reporter.Write(ckobj, YYCC_U8("Fail to convert its name by given encoding. Use generated name instead."));
				ret = GenerateCKObjectName(ckobj);
			}
		} else {
			// generate one
			m_Reporter.Write(ckobj, YYCC_U8("Unamed object. Use generated name."));
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
