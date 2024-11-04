#pragma once
#include "stdafx.hpp"

namespace vtobjplugin::Utilities {

	class StringLoader {
	public:
		static const StringLoader& GetSingleton();
	private:
		StringLoader();

	public:
		std::wstring LoadStringW(UINT uID, const wchar_t* fallback = L"") const;
		std::string LoadStringA(UINT uID, const char* fallback = "") const;
		YYCC::yycc_u8string LoadStringU8(UINT uID, const YYCC::yycc_char8_t* fallback = YYCC_U8("")) const;
	private:
		bool InternalLoadString(UINT uID, std::wstring& dst) const;
		HINSTANCE m_Instance;
	};

	class VirtoolsUIReporter {
	public:
		VirtoolsUIReporter(CKContext* context);
		~VirtoolsUIReporter();

		void Write(const YYCC::yycc_char8_t* strl) const;
		void Write(CKObject* associated_obj, const YYCC::yycc_char8_t* strl) const;
		void Format(const YYCC::yycc_char8_t* fmt, ...) const;
		void Format(CKObject* associated_obj, const YYCC::yycc_char8_t* fmt, ...) const;

	private:
		void RawWrite(const char* raw_strl) const;
		CKContext* m_Context;
	};

#define RADIOBTN_GETCHECK(instance) (instance.GetCheck() == BST_CHECKED)
#define RADIOBTN_SETCHECK(instance, stmt) (instance.SetCheck((stmt) ? BST_CHECKED : BST_UNCHECKED))

	void SetCWndText(CWnd* ctl, const YYCC::yycc_u8string_view& val);
	YYCC::yycc_u8string GetCWndText(CWnd* ctl);

	bool CheckDirectoryExistence(const YYCC::yycc_u8string_view& path);

	void GetNormalTransformMatrix(const VxMatrix& object_transform, VxMatrix& result);
	bool IsMirrorMatrix(const VxMatrix& mat);

}
