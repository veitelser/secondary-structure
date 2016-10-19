#include <iostream>
#include <fstream>

// [exe] [predicted sequences] [correct sequences]
// as 0,1,2,3,\n byte-files (H, E, T, N, delimeter)
int main(int argc, char ** argv) {
  if (argc != 3) {std::cout << "Incorrect command format.\n"; return 0;}
  std::ifstream p_file (argv[1]);
  std::ifstream c_file (argv[2]);

  std::string predicted; std::string known;
  int matches = 0; int positions = 0;
  while (getline(p_file, predicted) && getline(c_file, known)) {
    for (int k = 0; k < predicted.size() && k < known.size(); ++k) {
      ++positions;
      if (predicted[k] == known[k]) ++matches;
    }
  }

  // now we can get percentage of structure annotations correct.
  std::cout << "Similarity: " << 100.0 * matches / positions << "%.\n";
  return 0;
}