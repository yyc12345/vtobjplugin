#pragma once
#include "stdafx.hpp"
#include "ExportLayoutWeaver.hpp"

namespace vtobjplugin {

	class ObjExporter {
	public:
		ObjExporter(
			const DataTypes::ExportSetting& export_setting, 
			const ExportLayoutWeaver& export_layout_weaver,
			VirtoolsUIReporter ui_reporter) {}
		~ObjExporter() {}

	private:

	};

}
