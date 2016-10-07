#include <iostream>
#include <fstream>
#include <string>

char structure(char s) {
  switch(s) {
    case 'H': return 0;
    case 'E': return 1;
    case 'T': return 2;
    default: return 3;
  }
}

int main(int argc, char ** argv) {
  std::ifstream seqin (argv[1]);
  std::ofstream seqout (argv[2], std::ios::binary);
  if (seqin.is_open() && seqout.is_open()) {
    std::cout << "Opened files.\n";
  }
  else return 0;

  int numseq = 0;
  bool onseq = false;

  for (std::string line; std::getline(seqin, line);) {
    if (line[line.size()-1] == 'r') { // next line marks new sequence
      onseq = true;
      ++numseq;
    }
    else if (line[line.size()-1] == 'e' && onseq) {
      onseq = false; // now we're looking at the next amino acid structure.
      seqout << '\n';
    }
    else if (onseq) {
      for (int i = 0; i < line.size(); ++i) { // loop over the line.
        if (line[i] == ' ' || line[i] == '\n') continue;
        seqout << structure(line[i]);
      }
    }
  }

  std::cout << "Found " << numseq << " sequences.\n";

  return 0;
}