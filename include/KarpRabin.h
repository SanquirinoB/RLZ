#ifndef _KARP_RABIN_H
#define _KARP_RABIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>

#include "CompactedText.h"

using namespace std;

class KarpRabin{

protected: 
	unsigned long long voc_bits;
	unsigned long long kr_mod;
	unsigned long long table_size;
	unsigned long long *pow_table;
	
	// Version using precomputed table
	unsigned long long ullpow2(unsigned long long bits, unsigned long long y);
	
	// Logarithmic version for bit exponents
	unsigned long long ullpow2_log(unsigned long long bits, unsigned long long y);
	
public: 
	
	KarpRabin();
	KarpRabin(unsigned long long _voc_bits, unsigned long long _kr_mod, unsigned long long _table_size = 1000000);
	virtual ~KarpRabin();
	
	// Evaluate the full hash in str.length() operations
	unsigned long long hash(const string &str);
	
	// Evaluate the full hash in str.length() operations
	unsigned long long hash(const char *str, unsigned long long str_len);
	
	// Evaluate the full hash from start, in len operations
	unsigned long long hash(CompactedText *text, unsigned long long start, unsigned long long len);
	
	// Evaluate the hash of the concatenation in constant time
	unsigned long long concat(unsigned long long kr1, unsigned long long kr2, unsigned long long len2);
	
	// Evaluate the hash of the subtract in constant time
	unsigned long long subtract_prefix(unsigned long long kr12, unsigned long long kr1, unsigned long long len2);
	
	unsigned long long getTableSize(){
		return table_size;
	}
	
	void hashPrefixes(const string &pattern, vector<unsigned long long> &kr_vector);
	
	void hashPrefixesRev(const string &pattern, vector<unsigned long long> &kr_rev_vector);
	
	unsigned long long max_len;
	
};







#endif //_TEXT_FILTER_H





