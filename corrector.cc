// Functions to work with local classifier output, and correct it based on
// tags sampled from the PDB. Format is [exe] [input ss file] [tags] [# to correct] [mode] [output ss file],
// all three files uncompressed and \n-delimited
// [mode] is either vote or split
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
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

// split the sequence into chunks as long as the tags, then for each
// chunk replace it with the tag that has the smallest Hamming distance to it
string split_corrector(string & seq, vector<string> & tags) {
  string res = seq;
  int k = tags[0].size();
  int n = seq.size();
  int numchunks = n / k;
  for (int j = 0; j < numchunks; ++j) {
    // find the tag with minimum hamming distance away from this chunk
    int best_t = 0; int Ham = Hamming(seq, tags[0], j*k); int Ham2;
    for (int t = 1; t < tags.size(); ++t) {
      Ham2 = Hamming(seq, tags[t], j*k);
      if (Ham2 < Ham) {best_t = t; Ham = Ham2;}
    }
    // now we know the best tag for this chunk. Replace the chars in the chunk with those of the tag.
    for (int i = 0; i < k; ++i) res[j*k + i] = tags[best_t][i];
  }
  return res;
}

int main(int argc, char ** argv) {
  if (argc != 6) {cout<<"Wrong command format.\n"; return 0;}
  ifstream seqin (argv[1]);
  ifstream tagsin (argv[2]);
  int n_to_correct = atoi(argv[3]);
  string mode (argv[4]);
  ofstream seqout (argv[5]);

  vector<string> tags; string line;
  while(getline(tagsin, line)) tags.push_back(line);

  int twentieths_done = 0;
  cout << "[                    ] Progress";

  for (int k = 0; getline(seqin, line) && k < n_to_correct; ++k) {
    if (k > 0) seqout << '\n';
    if (mode=="vote") {seqout << vote_corrector(line, tags); }
    else{ seqout << split_corrector(line, tags); }

    twentieths_done = 20 * k / n_to_correct;
    cout << "\r["; for (int a = 0; a < 20; ++a) {if (a <= twentieths_done) cout << (unsigned char)0xb1; else cout << ' ';}
    cout << "] Progress";
  }
  cout<<"\n";
  return 0;
}