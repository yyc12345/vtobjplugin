#if !defined(_YYCDLL_STRING_HELPER_H__IMPORTED_)
#define _YYCDLL_STRING_HELPER_H__IMPORTED_

#include "stdafx.h"
#include <string>
#include <stringapiset.h>

namespace string_helper {

	void write_utf8bom(FILE* fs);
	BOOL check_cp_validation(UINT cp);
	BOOL stdstring_to_uint(std::string* strl, UINT* number);
	void uint_to_stdstring(std::string* strl, UINT number);
	void cwndtext_to_stdstring(CWnd* ctrl, std::string* strl);
	void stdstring_sprintf(std::string* strl, const char* format, ...);
	void encoding_conv(std::string* orig, std::string* dest, UINT origCP, UINT destCP);

}

#endif