#include <stdlib.h>
#include "streamfile_mem.h"


// 前面几个字节是正常的streamfile，后面几个字节是不同的内容，函数调用时修改了指针的类型，使得偏移量可以指向后面参数
typedef struct {
    STREAMFILE vt;          /* callbacks */

    uint8_t* buf;           /* data buffer */
    size_t buf_size;        /* max buffer size */
	char* name;
	int name_length;
} MEM_STREAMFILE;

size_t mem_read(MEM_STREAMFILE* sf, uint8_t* dst, offv_t offset, size_t length) {
	if (/*!sf->infile ||*/ !dst || length <= 0 || offset < 0)
		return 0;

	rsize_t end = offset + length;
	if (end > sf->buf_size) {
		length = end - sf->buf_size;
		if (length <= 0) {
			return 0;
		}
	}
	memcpy(dst, sf->buf + offset, length);
	
	return length;
}

size_t mem_get_size(MEM_STREAMFILE* sf) {
	return sf->buf_size;
}


offv_t mem_get_offset(MEM_STREAMFILE* sf) {
	return 114514;
}

void mem_get_name(MEM_STREAMFILE* sf, char* name, size_t name_size) {
	memcpy(name, sf->name, sf->name_length);
}

MEM_STREAMFILE* mem_open(struct MEM_STREAMFILE* sf, const char* const filename, size_t buf_size) {
	// return itself anyway
	return sf;
}

void mem_close(MEM_STREAMFILE* sf) {
	// don't do anything
}



STREAMFILE* get_mem_streamfile(char* buffer, size_t buf_size, const char *inputFileExtension) {
	MEM_STREAMFILE *sf = (MEM_STREAMFILE*)malloc(sizeof(MEM_STREAMFILE));
	sf->vt.read = mem_read;
	sf->vt.close = mem_close;
	sf->vt.get_name = mem_get_name;
	sf->vt.get_offset = mem_get_offset;
	sf->vt.get_size = mem_get_size;
	sf->vt.open = mem_open;

	int l = strlen(inputFileExtension);
	sf->name_length = l + 3;
	sf->name = (char*)malloc(sf->name_length);
	sf->name[0] = 'f';
	sf->name[1] = '.';
	strcpy(sf->name + 2, inputFileExtension);

	sf->buf = buffer;
	sf->buf_size = buf_size;

	return &sf->vt;
}