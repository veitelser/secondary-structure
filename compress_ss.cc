#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char ** argv) {
	std::ifstream seqin (argv[1]);
	std::ofstream seqout (argv[2], std::ios::binary);
	if (seqin.is_open() && seqout.is_open()) {
		std::cout << "Opened files.\n";
	}
	else {
		std::cout << "Failed to open.\n";
		return 0;
	}

	int numseq = 0;
	bool onseq = false;
	int k;

	for (std::string line; std::getline(seqin, line);) {
		if (line.size() == 0) continue;
		k = 0;
		unsigned char * buf = new unsigned char[line.size()];
		buf[0] = (line[0] << 6) + 1;
		for (int i = 1; i < line.size(); ++i) {
			if (buf[k] >> 6 == line[i] && (buf[k] & 0x3f) < 63) {
				buf[k]++;
			}
			else { // need to start a new bin
				buf[++k]=(line[i]<<6)+1;
			}
		} // end loop over characters in line
		seqout.write((char *)buf, k+1); seqout << char(0);
		++numseq;
		delete[] buf;
	}

	std::cout << "Compressed " << numseq << " sequences.\n";

	return 0;
}