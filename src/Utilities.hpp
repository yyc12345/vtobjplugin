#pragma once
#include "stdafx.hpp"

namespace vtobjplugin::Utilities {

	class StringLoader {
	public:
		static StringLoader& GetSingleton();
	private:
		StringLoader();

	public:
		std::wstring LoadStringW(UINT uID, const wchar_t* fallback = L"");
		std::string LoadStringA(UINT uID, const char* fallback = "");
	private:
		bool InternalLoadString(UINT uID, std::wstring& dst);
		HINSTANCE m_Instance;
	};

}
