#pragma once
#include "config.h"



int convert(MyFile& source, MyFile& dest, const char * inputFileExtension, init_vgmstream_t vgmstream_function, VgmConfig* cfg1,init_vgmstream_t* output_vgmstream_function);

