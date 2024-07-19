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
			/**
			 * The 3d object list this file included.
			 * Every 3d object item must have a valid associated mesh (not nullptr)
			 * and this mesh must have at least 1 face.
			*/
			std::deque<ObjectPair_t> m_ObjectList;
			/**
			 * The dictionary of materials and its name which is used by the mesh associating with 3d object in object list
			 * Key is CKMaterial pointer, value is material name.
			 * Every material pointer in pair must be valid (not nullptr)
			*/
			std::map<CKMaterial*, YYCC::yycc_u8string> m_MaterialMap;
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
		/**
		 * The dictionary recoding all textures used by all exported files.
		 * Key is CKTexture pointer, value is texture file name including extensions.
		 * Every texture must be valid (not nullptr) in pair.
		 * And this texture must have a valid file name in slot 0.
		*/
		std::map<CKTexture*, YYCC::yycc_u8string> m_TextureMap;

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
		const Utilities::StringLoader& m_StringLoader;
	};

}

