#pragma once
#include "stdafx.hpp"
#include "DataTypes.hpp"
#include "Utilities.hpp"
#include <deque>

namespace vtobjplugin {

	class ExportLayoutWeaver {
	public:
		using ObjectPair_t = std::pair<CK3dEntity*, YYCC::yycc_u8string>;
		struct File_t {
			YYCC::yycc_u8string m_ObjFileName; ///< The name of OBJ file.
			YYCC::yycc_u8string m_MtlFileName; ///< The name of MTL file.
			std::deque<ObjectPair_t> m_ObjectList; ///< The 3d object list this file included.
			std::map<CKMaterial*, YYCC::yycc_u8string> m_MaterialMap; ///< Key is CKMaterial pointer, value is material name.
		};
	public:
		ExportLayoutWeaver(
			PluginInterface* plugin_interface,
			const DataTypes::ExportSetting& export_setting,
			const Utilities::VirtoolsUIReporter& reporter);
		~ExportLayoutWeaver();

		// ===== Core Functions =====
	public:
		void WeaveLayout();
		const std::deque<File_t>& GetFileList() const;
		const std::map<CKTexture*, YYCC::yycc_u8string>& GetTextureMap() const;
	private:
		void CollectObject(std::deque<ObjectPair_t>& ret);
		void DistributeObject(const std::deque<ObjectPair_t>& object_pairs);
	private:
		std::deque<File_t> m_FileList; ///< The export list splitted by individual file
		std::map<CKTexture*, YYCC::yycc_u8string> m_TextureMap; ///< Key is CKTexture pointer, value is texture file name including extensions.

		// ===== Assistant Functions =====
	private:
		YYCC::yycc_u8string GetCKObjectName(CKObject* ckobj);
		void CorrectObjectName(YYCC::yycc_u8string& name);
		void CorrectPathName(YYCC::yycc_u8string& name);
	private:
		PluginInterface* m_PluginInterface;
		CKContext* m_Context;
		const DataTypes::ExportSetting& m_ExportSetting;
		const Utilities::VirtoolsUIReporter& m_Reporter;
	};

}

