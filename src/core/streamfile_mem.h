#pragma once
#include "src/streamfile.h"

STREAMFILE* get_mem_streamfile(char* buffer, size_t buf_size, const char* inputFileExtension);