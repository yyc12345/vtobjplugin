#pragma once
#include "stdafx.hpp"
#include "DataTypes.hpp"

namespace vtobjplugin {

	using VirtoolsUIReporter = void(*)(const YYCC::yycc_u8string&);

	class ExportLayoutWeaver {
	public:
		ExportLayoutWeaver(
			const DataTypes::ExportSetting& export_setting, 
			VirtoolsUIReporter reporter);
		~ExportLayoutWeaver();

	private:
		void WeaveLayout();
	};

}

