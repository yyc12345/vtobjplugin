#pragma once
#include "stdafx.hpp"
#include "ExportLayoutWeaver.hpp"
#include "Utilities.hpp"

namespace vtobjplugin {

	class ObjExporter {
	public:
		ObjExporter(
			const DataTypes::ExportSetting& export_setting,
			const ExportLayoutWeaver& export_layout_weaver,
			const Utilities::VirtoolsUIReporter reporter);
		~ObjExporter();

		// ===== Core Function =====
	public:
		void DoExport();
	private:
		void ExportObjFile(const ExportLayoutWeaver::File_t& file);
		void ExportMtlFile(const ExportLayoutWeaver::File_t& file);
		void CopyTextureFile();
		void GenerateScriptFile();

		// ===== Assistant Function =====
	private:
		YYCC::yycc_u8string GetExportFilePath(const YYCC::yycc_u8string_view& filename);
	private:
		const DataTypes::ExportSetting& m_ExportSetting;
		const ExportLayoutWeaver& m_ExportLayoutWeaver;
		const Utilities::VirtoolsUIReporter m_Reporter;
		const Utilities::StringLoader& m_StringLoader;
	};

}
