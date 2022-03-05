#include "string_helper.h"
#include <cstdarg>
#include <stdexcept>

namespace string_helper {

	void write_utf8bom(FILE* fs) {
		fputs("\xef\xbb\xbf", fs);
	}

	BOOL check_cp_validation(UINT cp) {
		CPINFOEXA cpinfo;
		return GetCPInfoExA(cp, 0, &cpinfo);
	}

	BOOL stdstring_to_uint(std::string* strl, UINT* number) {
		return (sscanf(strl->c_str(), "%u", number) > 0);
	}

	void uint_to_stdstring(std::string* strl, UINT number) {
		stdstring_sprintf(strl, "%u", number);
	}

	void cwndtext_to_stdstring(CWnd* ctrl, std::string* strl) {
		int count = ctrl->GetWindowTextLengthA();
		if (count == 0) {
			strl->clear();
			return;
		}

		count++;
		strl->resize(count);
		ctrl->GetWindowTextA(strl->data(), count);
	}

	void stdstring_sprintf(std::string* strl, const char* format, ...) {
		va_list argptr;
		va_start(argptr, format);
		int count = _vsnprintf(NULL, 0, format, argptr);
		count++;
		va_end(argptr);

		strl->resize(count);
		(*strl)[count - 1] = '\0';
		va_start(argptr, format);
		int write_result = _vsnprintf(strl->data(), count, format, argptr);
		va_end(argptr);

		if (write_result < 0 || write_result >= count) throw new std::length_error("Invalid write_result in _vsnprintf.");
	}

	void encoding_conv(std::string* orig, std::string* dest, UINT origCP, UINT destCP) {
		std::wstring wscache;

		int count, wcount, write_result;
		
		// convert to WCHAR
		wcount = MultiByteToWideChar(origCP, 0, orig->c_str(), -1, NULL, 0);
		if (wcount <= 0) throw new std::logic_error("Invalid count in MultiByteToWideChar.");
		wscache.reserve(wcount);
		write_result = MultiByteToWideChar(origCP, 0, orig->c_str(), -1, wscache.data(), wcount);
		if (write_result <= 0) throw new std::length_error("Invalid write_result in MultiByteToWideChar.");

		//converter to CHAR
		count = WideCharToMultiByte(destCP, 0, wscache.c_str(), -1, NULL, 0, NULL, NULL);
		if (wcount <= 0) throw new std::logic_error("Invalid count in WideCharToMultiByte.");
		dest->resize(count);
		write_result = WideCharToMultiByte(destCP, 0, wscache.c_str(), -1, dest->data(), count, NULL, NULL);
		if (write_result <= 0) throw new std::length_error("Invalid write_result in WideCharToMultiByte.");

	}

}
