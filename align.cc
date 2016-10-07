#include <iostream>
#include <fstream>
#include <string>

unsigned char symbol(unsigned char c) {return c >> 6;}
unsigned char count(unsigned char c) {return c & 0x3f;}

/* sample, entry are COMPRESSED strings.
   This calculates the simple Hamming distance between two strings
   aligned by some offset delta. */
int penalty(std::string & sample, std::string & entry, unsigned int delta) {
	// assume sample does not hang over the entry
	// delta is offset of sample on top of entry
	int j, k; // which character we are looking at in each string
	unsigned char a, b; // remaining number of the current symbol
	int m = sample.length(); int n = entry.length();
	int penalty = 0;
	j = 0; a = count(sample[0]);
	// calculate intial k, b for the entry.
	k = 0; b = count(entry[0]);
	while (delta > 0) {
		if (delta < b) {b -= delta; break;}
		else { delta -= b; ++k; b = count(entry[k]); }
	}
	//std::cout << k << ' ' << int(b) << std::endl;
	// loop over the overlap
	unsigned char dif;
	while (j < m) {
		if (a <= b) {b-=a; dif = a; a=0;}
		else {a-=b; dif = b; b = 0;}
		if (symbol(sample[j]) != symbol(entry[k])) penalty += dif;
		if (a==0) {++j; a = count(sample[j]); }
		if (b==0) {++k; b = count(entry[k]); }
		//std::cout << j << ' '<<int(a)<<' '<<k<<' '<<int(b)<<std::endl;
	}
	return penalty;
}

/* DECOMPRESSED STRINGS of arbitrary characters, say.
   This actually calculates the min cost of a matching
   between characters of two strings. See Kleinberg & Tardos.
   One difference is that I don't penalize unmatched entry letters.
   The idea is that usually the sample will be smaller. */
int minimizer(std::string & sample, std::string & entry, int ** backi, int ** backj) {
	int delta = 1;
	int alpha = 1;
	int m = sample.length(); int n = entry.length();
	// opt[i][j] is optimal score using i characters of sample and j characters of entry.
	int ** opt = new int*[m+1];
	for (int k = 0; k <= m; ++k) opt[k] = new int[n+1];

	for (int i=0; i <= m; ++i) opt[i][0] = i*delta;
	for (int j=0; j <= n; ++j) opt[0][j] = 0;
	for (int i=1; i <= m; ++i) {
		for (int j=1; j <= n; ++j) {
			int align_both = opt[i-1][j-1] + (sample[i-1]==entry[j-1]?0:alpha);
			int skip_i = opt[i-1][j] + delta;
			int skip_j = opt[i][j-1];
			if (align_both <= skip_i && align_both <= skip_j) {
				opt[i][j] = align_both;
				backi[i][j] = i-1; backj[i][j] = j-1;
			} else if (skip_i <= align_both && skip_i <= skip_j) {
				opt[i][j] = skip_i;
				backi[i][j] = i-1; backj[i][j] = j;
			} else {
				opt[i][j] = skip_j;
				backi[i][j] = i; backj[i][j] = j-1;
			}
		}
	}
	int mincost = opt[m][n];
	for (int k=0; k <= m; ++k) delete[] opt[k];
	return mincost;
}

/* Assume opt has been filled out by the minimizer, matching().
 * Correct the sample string and return the result. For each
 * unmatched character, keep it the same. For each matched character,
 * change it to the the result of the match. */
std::string correction(std::string & sample, std::string & entry, int ** backi, int ** backj) {
	int i = sample.length(); int j = entry.length();
	std::string res = sample;
	while (i > 0 && j > 0) {
		if (backi[i][j] == i-1 && backj[i][j] == j-1) {
			res[i-1] = entry[j-1];
		}
		int old_i = i;
		i = backi[i][j]; j = backj[old_i][j];
	}
	return res;
}

int main() {
	std::string sample = "000";
	std::string entry = "00000";
	sample[0] = 0x05; sample[1] = 0x82; sample[2] = 0x42;
	entry[0] = 0x82; entry[1] = 0x02; entry[2] = 0x81; entry[3]=0x03; entry[4]=0x44;
	std::cout << penalty(sample, entry, 2) << std::endl;

	std::string name = "Salvatore";
	std::string thing = "Salamander";
	// best alignment should be Sal vato re
	//                          Salamander
	// which has a cost of 4.
		// matrix score for optimum length
	int m = name.length(); int n = thing.length();
	int ** backi = new int*[m+1]; int ** backj = new int*[m+1];
	for (int k = 0; k <= m; ++k) {backi[k] = new int[n+1]; backj[k]=new int[n+1];}

	std::cout << minimizer(name, thing, backi, backj) << std::endl;
	std::cout << correction(name, thing, backi, backj) << std::endl;

	for (int k = 0; k <= m; ++k) {delete[] backi[k];delete[] backj[k];}

	return 0;
}