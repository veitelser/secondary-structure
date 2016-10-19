// Sample an uncompressed databank of secondary structure sequences into tags.
// command arguments are [databank] [length of tag] [prob. of sampling a given sequence] [out]
// databank format is 0, 1, 2, 3, \n (A) bytes.
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

int main(int argc, char ** argv) {
  if (argc != 5) { std::cout << "Format: [cmd] [in] [tag length] [pr. sample] [out].\n"; return 0; }
  std::ifstream filein (argv[1]);
  std::ofstream fileout (argv[4]);
  std::istringstream taglen_s ( argv[2] );
  std::istringstream pr_s (argv[3]);
  int taglen; double pr; taglen_s >> taglen; pr_s >> pr;

  int offset; int k; int Ntags = 0;

  srand(time(NULL)); // seed the C random number generator
  for (std::string line; std::getline(filein, line);) { // loop over file lines
    if (rand() % 1000 > 1000 * pr) continue;
    if (line.size() < taglen) continue;
    // randomly sample a tag from the line
    if (Ntags > 0) fileout << '\n';
    offset = rand() % (line.size() - taglen + 1);
    for (k = 0; k < taglen; ++k) fileout << line[offset + k];
    ++Ntags;
  }

  std::cout << "Wrote " << Ntags << " tags to file.\n";
}