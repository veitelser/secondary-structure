// get an idea of how quickly you cover the set of naturally occurring length-k tags
// as you sample more of them from the PDB.
// nunique file tells you how many unique tags have been found after each protein is sampled.
// nrepeat file: each line represents a unique tag. The number is the number of times
// that unique tag was present in the PDB.

// COMMAND FORMAT: [executable] [sequences] [tag length] [nunique] [nrepeat] [taglib]
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

int main(int argc, char ** argv) {
  if (argc != 6) {std::cout << "Invalid command format.\n"; return 0;}
  ifstream seqfile (argv[1]);
  int k = atoi(argv[2]);
  ofstream nunique (argv[3]);
  ofstream nrepeat (argv[4]);
  ofstream taglib (argv[5]);

  // Set up a hashtable that uses a k-length tag as key and maps to the
  // number of times that tag is encountered in the PDB.
  // This way, you can extract the asymptotic behavior analytically.
  // The number of key-value pairs is the number of unique sequences
  // that were explicitly encountered in the PDB.
  unordered_map<string, int> redundancy;
  int n, i, nsamples=0, j=1, max_count=0;
  string tag;

  cout << "Sequences processed: 0";
  for (string line; getline(seqfile, line); ++j) {
    // loop over the long sequence entries in the PDB. For each,
    // process all of the length-k substrings.
    n = line.size();
    if (n < k) continue;
    for (i=0; i <= n-k; ++i) {
      ++nsamples;
      tag = line.substr(i, k);
      if (redundancy.count(tag) == 0) { redundancy[tag] = 1; }
      else { int &red = redundancy[tag]; ++red; if (red > max_count) max_count = red; }
    }
    nunique << redundancy.size() << endl;
    if (j % 1000 == 0) cout << "\rSequences processed: " << j;
  }

  // redundancy is now set up.
  cout << "\nNumber of unique tags found: " << redundancy.size();
  cout << "\nNumber of samples taken: " << nsamples;
  cout << "\nRedundancy of most common tag: " << max_count << endl;
  // Would like to give the user some statistics on the distribution of
  // the unique tags. Some may be much more important than others.

  for (auto & kv : redundancy) {nrepeat << kv.second << endl; taglib << kv.first << endl; }

  return 0;
}