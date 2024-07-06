#include "Utilities.hpp"

namespace vtobjplugin::Utilities {

#pragma region String Loader

	StringLoader& StringLoader::GetSingleton() {
		static StringLoader g_Singleton;
		return g_Singleton;
	}

	StringLoader::StringLoader() :
		m_Instance(YYCC::WinFctHelper::GetCurrentModule()) {}

	std::wstring StringLoader::LoadStringW(UINT uID, const wchar_t* fallback) {
		std::wstring ret;
		if (!InternalLoadString(uID, ret)) 
			ret = (fallback == nullptr ? L"" : fallback);
		return ret;
	}

	std::string StringLoader::LoadStringA(UINT uID, const char* fallback) {
		std::wstring wret;
		std::string ret;
		if (!InternalLoadString(uID, wret) || !YYCC::EncodingHelper::WcharToChar(wret, ret, CP_ACP)) 
			ret = (fallback == nullptr ? "" : fallback);
		return ret;
	}

	bool StringLoader::InternalLoadString(UINT uID, std::wstring& dst) {
		wchar_t* str_ptr = nullptr;
		int str_length = ::LoadStringW(m_Instance, uID, reinterpret_cast<LPWSTR>(&str_ptr), 0);
		if (str_length == 0) return false;

		dst.assign(str_ptr, static_cast<size_t>(str_length));
		return true;
	}

#pragma endregion


}
