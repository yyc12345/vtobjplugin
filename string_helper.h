#if !defined(_YYCDLL_STRING_HELPER_H__IMPORTED_)
#define _YYCDLL_STRING_HELPER_H__IMPORTED_

#include "stdafx.h"
#include <string>
#include <stringapiset.h>

namespace string_helper {

	void stdstring_sprintf(std::string* strl, const char* format, ...);
	void encoding_conv(std::string* orig, std::string* dest, UINT origCP, UINT destCP);

}

#endif