#include "core/convert.h"
#include "core/config.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {
	ifstream myfile;
	myfile.open("Z:\\getVoiceOver.wem", ios::in | ios::binary | ios::ate);
	MyFile source;
	MyFile ans;

	int size = myfile.tellg();
	source.reset(size);
	myfile.seekg(0);
	myfile.read(source.buffer, size);
	source.current = size;

	VgmConfig c = { 0 };

	convert(source, &c, ans, "wem");

	ofstream output;
	output.open("Z:\\test.wav", ios::binary);
	output.write(ans.buffer, ans.current);

	myfile.close();
}