#include "ExportLayoutWeaver.hpp"
#include <set>
#include <numeric>
#include <stdexcept>

namespace vtobjplugin {

	class UniqueName {
	public:
		UniqueName() : m_NameSet() {}
		~UniqueName() {}

	private:
		std::set<YYCC::yycc_u8string> m_NameSet;
	public:
		/**
		 * @brief Generate unique name from expected name.
		 * @param[in] expected_name The expected name.
		 * @param[out] final_name String container for receiving final unique name.
		 * @return True if final name is equal to expected name, otherwise false.
		*/
		bool GetName(const YYCC::yycc_u8string_view& _expected_name, YYCC::yycc_u8string& final_name) {
			// try first insert
			YYCC::yycc_u8string expected_name(_expected_name);
			final_name = expected_name;
			auto result = m_NameSet.emplace(final_name);
			if (result.second) return true;

			// fail at the first inserting
			// try building unqiue name
			uint64_t i = 0;
			while (true) {
				// check limitation
				if (i == std::numeric_limits<uint64_t>::max()) 
					throw std::runtime_error("unique name exceed the max index.");
				// build unique name
				if (!YYCC::StringHelper::Printf(final_name, YYCC_U8("%s_%03" PRIu64), expected_name.c_str(), i))
					throw std::runtime_error("fail to format unique name.");
				// try inserting
				auto result = m_NameSet.emplace(final_name);
				if (result.second) break;
			}
			// indicate final name is different with expected name
			return false;
		}
	};

#pragma region Export Layout Weaver

	void ExportLayoutWeaver::WeaveLayout() {
	
	
	}

#pragma endregion

}
