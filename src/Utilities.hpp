#pragma once
#include "stdafx.hpp"

namespace vtobjplugin::Utilities {

	class StringLoader {
	public:
		static StringLoader& GetSingleton();
	private:
		StringLoader();

	public:
		const wchar_t* LoadStringW(UINT uID, const wchar_t* fallback = L"");
		const char* LoadStringA(UINT uID, const char* fallback = "");
	private:
		bool InternalLoad(UINT uID);

		HINSTANCE m_Instance;
		std::string m_CharCache;
		std::wstring m_WcharCache;
	};

}
