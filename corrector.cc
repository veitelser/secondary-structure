// Functions to work with local classifier output, and correct it based on
// tags sampled from the PDB. Format is [exe] [input ss file] [tags] [output ss file],
// all three files uncompressed and \n-delimited
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

int Hamming(string & seq, string & tag, int offset) {
  int H = 0;
  for (int k=0; k < tag.size(); ++k) {
    if (seq[k+offset] != tag[k]) ++H;
  }
  return H;
}

string vote_corrector(string & seq, vector<string> & tags) {
  int N = seq.size();
  vector<bool> voted(N, false);
  vector<vector<double> > votes;
  for (int j=0;j<N;++j) votes.push_back(vector<double>(4, 0.0));
  int offset, Ham, Ham2; double vote;

  for (int t=0; t<tags.size(); ++t) {
    // on a given tag. Line it up as best as possible with the sequence.
    offset = 0; Ham = Hamming(seq, tags[t], offset);
    for (int k=1; k <= N - tags[t].size(); ++k) {
      Ham2 = Hamming(seq, tags[t], k);
      if (Ham2 < Ham) {offset = k; Ham = Ham2;}
    }
    // have now aligned the tag. Cast votes for it.
    vote = exp(-Ham);
    for (int k=0; k < tags[t].size(); ++k) {
      unsigned char type = tags[t][k];
      votes[offset+k][type] += vote; voted[offset+k]=true;
    }
  }
  // we have now gone through all the tags and counted their votes.
  // Now decide which sequence elements to change, if any.
  string res = seq; int max_i; double max_vote;
  for (int j = 0; j < N; ++j) {
    max_i=0; max_vote = votes[j][0];
    if (votes[j][1] > max_vote) {max_i=1; max_vote=votes[j][1];}
    if (votes[j][2] > max_vote) {max_i=2; max_vote=votes[j][2];}
    if (votes[j][3] > max_vote) {max_i=3; max_vote=votes[j][3];}
    if (voted[j]) res[j] = (unsigned char)max_i;
  }
  return res;
}

int main(int argc, char ** argv) {
  if (argc != 4) {cout<<"Wrong command format.\n"; return 0;}
  ifstream seqin (argv[1]);
  ifstream tagsin (argv[2]);
  ofstream seqout (argv[3]);

  vector<string> tags; string line;
  while(getline(tagsin, line)) tags.push_back(line);
  for (int k = 0; getline(seqin, line) && k < 100; ++k) {
    if (k > 0) seqout << '\n';
    seqout << vote_corrector(line, tags);
  }
  return 0;
}