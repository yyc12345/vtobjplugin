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

	YYCC::yycc_u8string StringLoader::LoadStringU8(UINT uID, const YYCC::yycc_char8_t* fallback) {
		std::wstring wret;
		YYCC::yycc_u8string ret;
		if (!InternalLoadString(uID, wret) || !YYCC::EncodingHelper::WcharToUTF8(wret, ret))
			ret = (fallback == nullptr ? YYCC_U8("") : fallback);
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


	bool ValidateCodePage(UINT code_page) {
		CPINFOEXW cpinfo;
		return GetCPInfoExW(code_page, 0, &cpinfo);
	}

	void SetCWndText(CWnd* ctl, const YYCC::yycc_u8string_view& val) {
		HWND hwnd = ctl->m_hWnd;
		auto wval(YYCC::EncodingHelper::UTF8ToWchar(val));
		SetWindowTextW(hwnd, wval.data());
	}

	YYCC::yycc_u8string GetCWndText(CWnd* ctl) {
		HWND hwnd = ctl->m_hWnd;
		YYCC::yycc_u8string ret;
		std::wstring wret;

		// Ref: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextlengthw
		SetLastError(ERROR_SUCCESS);
		int len = GetWindowTextLengthW(hwnd);
		if (len == 0 && GetLastError() != ERROR_SUCCESS) 
			return ret;

		int copied = GetWindowTextW(hwnd, wret.data(), len);
		if (len == 0 && GetLastError() != ERROR_SUCCESS) 
			return ret;

		ret = YYCC::EncodingHelper::WcharToUTF8(wret);
		return ret;
	}

	bool CheckDirectoryExistence(const YYCC::yycc_u8string_view& path) {
		// convert path
		std::wstring wpath;
		if (!YYCC::EncodingHelper::UTF8ToWchar(path, wpath)) 
			return false;

		// find directory
		WIN32_FIND_DATAW wfd;
		HANDLE hFind = FindFirstFileW(wpath.c_str(), &wfd);
		if (hFind == INVALID_HANDLE_VALUE) return false;
		FindClose(hFind);
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return false;
		return true;
	}

}
