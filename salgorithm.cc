#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <limits>
#include <cmath>
using namespace std;

typedef vector<vector<int> > intM;
string predict(string &, int, int,vector<string> &, vector<intM> &, double[4][4]);

/* [executable] taglib */
int main(int argc, char ** argv) {
	double conf[4][4] = {
		{0.7, 0.1, 0.1, 0.1},
		{0.1, 0.7, 0.1, 0.1},
		{0.1, 0.1, 0.7, 0.1},
		{0.1, 0.1, 0.1, 0.7}
	};
	double confw[4][4];
	for (int i=0; i < 4; ++i) {
	for (int j=0; j < 4; ++j) {
		confw[i][j] = -log(conf[i][j]);
	}}

	if (argc != 2) return 0;
	ifstream taglib (argv[1]);
	int N = 0;
	vector<string> tagvec;
	unordered_set<string> tagset;
	for (string line; getline(taglib, line); ++N) {
		tagvec.push_back(line);
		tagset.insert(line);
	}

	if (N==0) {cout << "Empty file.\n"; return 0; }
	cout << "Loaded tag library.\n";
	int taglen = tagvec[0].size();

	// compatibility matrices
	vector<intM> compats(taglen-1);
	for (int k = 0; k < taglen-1; ++k) {
		compats[k] = vector<vector<int> >(N);
		for (int row = 0; row < N; ++row) {
			compats[k][row] = vector<int>();
		}
	}

	// set them up by going through the tags
	for (int j = 0; j < N; ++j) {for (int k = 0; k < N; ++k) {
		// loop over pairs
		// consider the number of annotations in the second tag
		for (int numsec = 1; numsec < taglen; ++numsec) {
			// first make that joint substring
			string query =
				tagvec[j].substr(numsec, taglen-numsec) +
				tagvec[k].substr(0, numsec);
			if (tagset.count(query) == 0) break; // it's over for this pair!
			// otherwise, we've found a compatibility!
			compats[numsec-1][j].push_back(k);
		}
	}
	int per = 100*j/(N-1);
	if (per % 5 == 0)
	cout << "\rBuilding graph:\t" << per << "%";
	}
	cout << endl;

	int pairs=0;for(int r=0;r<N;++r) {pairs += compats[taglen-2][r].size();}
	cout << "Pairs: " << pairs << endl;

	string mylout = "1230001010101111100010101110101010101";
	cout << predict(mylout, N, taglen, tagvec, compats, confw) << endl;

	return 0;
}

string predict(string & locout, int N, int taglen,
	vector<string> & tagvec, vector<intM> & compats, double confw[4][4]) {

	int n = locout.size(); // length of sequence we would like to correct
	int rem = n % taglen;
	int nodecols = (n-rem)/taglen + (rem == 0 ? 0 : 1);
	
	vector<vector<int> > backpointers(N);
	vector<vector<double> > dist(N);
	for (int row = 0; row < N; ++row) {
		backpointers[row] = vector<int>(nodecols, 0);
		dist[row] = vector<double>(nodecols,
			numeric_limits<double>::infinity());
	}

	// set up small-step distances to all nodes
	vector<vector<double> > elens(N);
	for (int row = 0; row < N; ++row) {
		elens[row] = vector<double>(nodecols);
		for (int col = 0; col < nodecols; ++col) {
			double len = 0.0;
			for (int k = 0; k < (rem > 0 && col == nodecols-1 ? rem : taglen); ++k) {
				// k counts over the tag string for as long as it's supposed to
				char node_k = tagvec[row].at(k);
				char query_k = locout.at(col*taglen + k);
				len += confw[query_k - '0'][node_k - '0'];
			}
			elens[row][col] = len;
		}
	}

	// set up initial distances in first column
	for (int row = 0; row < N; ++row) {
		dist[row][0] = elens[row][0];
	}

	cout << "Set up edge lengths.\n";

	// Now do the heart of Dijkstra's algorithm
	for (int col = 0; col < nodecols-1; ++col) {
	for (int row = 0; row < N; ++row) {
		// figure out which compatibility matrix we want to use!
	for (int e2 : compats[ (rem > 0 && col == nodecols-2) ? rem-1 : taglen-2 ][row]) {
		// look up edge length
		double e = elens[e2][col+1];
		if (dist[row][col] + e < dist[e2][col+1]) {
			dist[e2][col+1] = dist[row][col] + e;
			backpointers[e2][col+1] = row;
		}
	}
	}
	}

	cout << "Found min distances.\n";

	// Find the closest node in the last column,
	// then trace through backpointers
	int bestrow = 0; double bestdist = dist[bestrow][nodecols-1];
	for (int row = 1; row < N; ++row) {
		if (dist[row][nodecols-1] < bestdist) {
			bestrow = row;
			bestdist = dist[row][nodecols-1];
		}
	}

	string res = tagvec[bestrow];
	if (rem > 0) { res = res.substr(0, rem); }
	int c = nodecols - 1; int r = bestrow;
	while (c > 0) {
		r = backpointers[r][c];
		res = tagvec[r] + res;
		cout << tagvec[r] << endl;
		--c;
	}

	return res;
}