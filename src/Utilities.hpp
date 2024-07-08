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
		YYCC::yycc_u8string LoadStringU8(UINT uID, const YYCC::yycc_char8_t* fallback = YYCC_U8(""));
	private:
		bool InternalLoadString(UINT uID, std::wstring& dst);
		HINSTANCE m_Instance;
	};

	void SetCWndText(CWnd* ctl, const YYCC::yycc_u8string_view& val);
	//YYCC::yycc_u8string GetCWndText(CWnd* ctl);

}
