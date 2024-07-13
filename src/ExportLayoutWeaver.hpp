#pragma once
#include "stdafx.hpp"
#include "DataTypes.hpp"
#include <deque>

namespace vtobjplugin {

	class VirtoolsUIReporter {
	public:
		VirtoolsUIReporter(CKContext* context);
		~VirtoolsUIReporter();

		void Write(const YYCC::yycc_char8_t* strl) const;
		void Write(CKObject* associated_obj, const YYCC::yycc_char8_t* strl) const;
		void Format(const YYCC::yycc_char8_t* fmt, ...) const;
		void Format(CKObject* associated_obj, const YYCC::yycc_char8_t* fmt, ...) const;

	private:
		void RawWrite(const char* raw_strl) const;
		CKContext* m_Context;
	};

	class ExportLayoutWeaver {
	public:
		using ObjectPair_t = std::pair<CK3dEntity*, YYCC::yycc_u8string>;
		struct File_t {
			YYCC::yycc_u8string m_FileName; ///< The name of this file. Do not include extension (.obj .mtl).
			std::deque<ObjectPair_t> m_ObjectList;
			std::map<CKMaterial*, YYCC::yycc_u8string> m_MaterialMap; ///< Key is CKMaterial pointer, value is material name.
		};
	public:
		ExportLayoutWeaver(
			PluginInterface* plugin_interface,
			const DataTypes::ExportSetting& export_setting);
		~ExportLayoutWeaver();

		// ===== Core Functions =====
	public:
		const std::deque<File_t>& GetFileList() const;
		const std::map<CKTexture*, YYCC::yycc_u8string>& GetTextureMap() const;
	private:
		void WeaveLayout();
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
		VirtoolsUIReporter m_Reporter;
	};

}

