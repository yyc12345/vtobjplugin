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

#pragma region Virtools UI Reporter

	VirtoolsUIReporter::VirtoolsUIReporter(CKContext* context) :
		m_Context(context) {}

	VirtoolsUIReporter::~VirtoolsUIReporter() {}

	void VirtoolsUIReporter::Write(const YYCC::yycc_char8_t* strl) const {
		RawWrite(YYCC::EncodingHelper::UTF8ToChar(strl, CP_ACP).c_str());
	}

	void VirtoolsUIReporter::Write(CKObject* associated_obj, const YYCC::yycc_char8_t* strl) const {
		std::string buffer;

		// add associated object header
		if (associated_obj != nullptr) {
			// obj marker head
			buffer += "[";

			// object marker name part
			CKSTRING assoc_obj_name = associated_obj->GetName();
			if (assoc_obj_name != nullptr) {
				buffer += assoc_obj_name;
				buffer += " ";
			}

			// object marker id part
			buffer += YYCC::EncodingHelper::UTF8ToChar(
				YYCC::StringHelper::Printf(YYCC_U8("<ID: %>" PRIu32), associated_obj->GetID()),
				CP_ACP
			);

			// obj marker tail
			buffer += "] ";
		}

		// append real output data
		buffer += YYCC::EncodingHelper::UTF8ToChar(strl, CP_ACP);
		RawWrite(buffer.c_str());
	}

	void VirtoolsUIReporter::Format(const YYCC::yycc_char8_t* fmt, ...) const {
		va_list argptr;
		va_start(argptr, fmt);
		this->Write(YYCC::StringHelper::VPrintf(fmt, argptr).c_str());
		va_end(argptr);
	}

	void VirtoolsUIReporter::Format(CKObject* associated_obj, const YYCC::yycc_char8_t* fmt, ...) const {
		va_list argptr;
		va_start(argptr, fmt);
		this->Write(associated_obj, YYCC::StringHelper::VPrintf(fmt, argptr).c_str());
		va_end(argptr);
	}

	void VirtoolsUIReporter::RawWrite(const char* raw_strl) const {
		if (m_Context != nullptr && raw_strl != nullptr) {
			m_Context->OutputToConsole(
				const_cast<CKSTRING>(raw_strl),
				FALSE
			);
		}
	}

#pragma endregion

	bool ValidateCodePage(UINT code_page) {
		CPINFOEXW cpinfo;
		return GetCPInfoExW(code_page, 0, &cpinfo);
	}

	void SetCWndText(CWnd* ctl, const YYCC::yycc_u8string_view& val) {
		ctl->SetWindowTextA(YYCC::EncodingHelper::UTF8ToChar(val, CP_ACP).c_str());
	}

	YYCC::yycc_u8string GetCWndText(CWnd* ctl) {
		// Ref: https://learn.microsoft.com/zh-cn/cpp/mfc/reference/cwnd-class?view=msvc-170#getwindowtext
		CString recv;
		ctl->GetWindowTextA(recv);
		return YYCC::EncodingHelper::CharToUTF8((LPCSTR)recv, CP_ACP);
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
