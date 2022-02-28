#include "string_helper.h"
#include <cstdarg>
#include <stdexcept>

namespace string_helper {

	void stdstring_sprintf(std::string* strl, const char* format, ...) {
		va_list argptr;
		va_start(argptr, format);
		int count = _vsnprintf(NULL, 0, format, argptr);
		count++;
		va_end(argptr);

		strl->clear();
		strl->resize(count);
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
		wscache.clear();
		wscache.reserve(count);
		write_result = MultiByteToWideChar(origCP, 0, orig->c_str(), -1, wscache.data(), wcount);
		if (write_result <= 0) throw new std::length_error("Invalid write_result in MultiByteToWideChar.");

		//converter to CHAR
		count = WideCharToMultiByte(destCP, 0, wscache.c_str(), -1, NULL, 0, NULL, NULL);
		if (wcount <= 0) throw new std::logic_error("Invalid count in WideCharToMultiByte.");
		dest->clear();
		dest->resize(count);
		write_result = WideCharToMultiByte(destCP, 0, wscache.c_str(), -1, dest->data(), count, NULL, NULL);
		if (write_result <= 0) throw new std::length_error("Invalid write_result in WideCharToMultiByte.");

	}

}
