#include "buffer_helper.h"

namespace buffer_helper {

	char* global_buffer;
	char* export_buffer;
	char* misc_buffer;
	char* misc_buffer2;

	BOOL InitBuffer() {
		global_buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
		export_buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
		misc_buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
		misc_buffer2 = (char*)malloc(BUFFER_SIZE * sizeof(char));

		return (global_buffer && export_buffer && misc_buffer && misc_buffer2);
	}
	void DisposeBuffer() {
#define safe_free(a) if(a)free(a);
		safe_free(global_buffer);
		safe_free(export_buffer);
		safe_free(misc_buffer);
		safe_free(misc_buffer2);
#undef safe_free
	}

}