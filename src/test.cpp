#include "core/convert.h"
#include "core/config.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {
	ifstream myfile;
	myfile.open("E:\\7ztmp\\getVoiceOver.wem", ios::in | ios::binary | ios::ate);
	MyFile source;
	MyFile ans;

	int size = myfile.tellg();
	source.reset(size);
	myfile.seekg(0);
	myfile.read(source.buffer, size);
	source.current = size;

	VgmConfig c = { 0 };


	init_vgmstream_t func;
	convert(source, ans, "wem", 0, &c, &func);

	ofstream output;
	output.open("Z:\\test.wav", ios::binary);
	output.write(ans.buffer, ans.current);
	output.close();

	// -- test2, with generated func

	ifstream myfile2;
	myfile2.open("E:\\7ztmp\\getVoiceOver2.wem", ios::in | ios::binary | ios::ate);
	MyFile source2;
	MyFile ans2;

	int size2 = myfile2.tellg();
	source2.reset(size2);
	myfile2.seekg(0);
	myfile2.read(source2.buffer, size2);
	source2.current = size2;

	ofstream output2;
	output2.open("Z:\\test2.wav", ios::binary);
	convert(source2, ans2, "wem", func, &c, 0);
	output2.write(ans2.buffer, ans2.current);
	output2.close();

	myfile2.close();
}