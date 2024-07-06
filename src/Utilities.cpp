#include "Utilities.hpp"

namespace vtobjplugin::Utilities {

#pragma region String Loader

	StringLoader& StringLoader::GetSingleton() {
		static StringLoader g_Singleton;
		return g_Singleton;
	}

	StringLoader::StringLoader() :
		m_CharCache(), m_WcharCache(),
		m_Instance(YYCC::WinFctHelper::GetCurrentModule()) {}

	const wchar_t* StringLoader::LoadStringW(UINT uID, const wchar_t* fallback) {
		if (InternalLoad(uID)) return m_WcharCache.c_str();
		else return fallback;
	}

	const char* StringLoader::LoadStringA(UINT uID, const char* fallback) {
		if (!InternalLoad(uID)) return fallback;
		if (!YYCC::EncodingHelper::WcharToChar(m_WcharCache, m_CharCache, CP_ACP)) return fallback;
		return m_CharCache.c_str();
	}

	bool StringLoader::InternalLoad(UINT uID) {
		wchar_t* str_ptr = nullptr;
		int str_length = ::LoadStringW(m_Instance, uID, reinterpret_cast<LPWSTR>(&str_ptr), 0);
		if (str_length == 0) return false;

		m_WcharCache.assign(str_ptr, static_cast<size_t>(str_length));
		return true;
	}

#pragma endregion


}
