#if !defined(_YYCDLL_BUFFER_HELPER_H__IMPORTED_)
#define _YYCDLL_BUFFER_HELPER_H__IMPORTED_

#include "stdafx.h"
#define BUFFER_SIZE 65526

namespace buffer_helper {

	extern char* global_buffer;
	extern char* export_buffer;
	extern char* misc_buffer;
	extern char* misc_buffer2;

	BOOL InitBuffer();
	void DisposeBuffer();

}

#endif