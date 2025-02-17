#ifndef _RELZ_INDEX_HASH_H
#define _RELZ_INDEX_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>
#include <vector>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/inv_perm_support.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "FactorsConfig.h"
#include "FactorsIterator.h"
#include "FactorsFastIteratorComparator.h"
#include "FactorsIteratorCompacted.h"
#include "NanoTimer.h"

#include "BitsUtils.h"
#include "KarpRabin.h"
#include "KarpRabinFactorsSuffixes.h"
#include "HashTrie.h"

using namespace sdsl;
using namespace std;

class RelzIndexHash {

private: 
	
	unsigned long long len_text;
	unsigned long long n_factors;
	
	CompactedText *ref_text;
	
	fm_index_type fm_index;
	
	rmq_type rmq;
	
	bits_s_type bits_s;
	bits_s_type::select_1_type select1_s;
	bits_s_type::select_0_type select0_s;
	
	bits_b_type bits_b;
	bits_b_type::select_1_type select1_b;
	bits_b_type::select_0_type select0_b;
	
	wt_type wt;
	
	int_vector<> pi;
	int_vector<> pi_inv;
	
	int_vector<> arr_x;
	int_vector<> arr_y;
	
	// Prueba de aceleracion de recursive_rmq almacenando los datos de los factores descomprimidos
	static const bool precompute_rmq = false;
	vector<unsigned long long> arr_tu;
	vector<unsigned long long> arr_pu;
	vector<unsigned long long> arr_lu;
	
	// Simplifacion del calculo de posicion de inicio de factor en coleccion 
	vector<unsigned long long> factors_start;
	
	void recursive_rmq(unsigned long long ini, unsigned long long fin, unsigned long long min_pos, unsigned long long occ_ref, vector<unsigned long long> &results);
	
public: 
	RelzIndexHash();
	RelzIndexHash(KarpRabin *_karp_rabin);
	RelzIndexHash(vector<pair<unsigned long long, unsigned long long> > &factors, char *full_text, unsigned long long _len_text, const char *_ref_text, unsigned long long _len_ref, KarpRabin *_karp_rabin);
	~RelzIndexHash();
	
	// void find(const string &pattern, vector<unsigned long long> &results);
	void findTimes(const string &pattern, vector<unsigned long long> &results, bool use_hash = true);
	
	unsigned long long querytime_p1;
	unsigned long long querytime_p2;
	unsigned long long querytime_p3x;
	unsigned long long querytime_p3y;
	unsigned long long querytime_p4;
	
	unsigned long long occs_a;
	unsigned long long occs_b;
	unsigned long long occs_c;
	unsigned long long occs_d;
	
	KarpRabinFactorsSuffixes *getKRFactors(){
		return kr_factors;
	}
	
	KarpRabin *karp_rabin;
	vector<unsigned long long> arr_kr_ref;
	vector<unsigned long long> arr_kr_s;
	KarpRabinFactorsSuffixes *kr_factors;
	
	HashTrie tree_y;
	HashTrie tree_x;
	
	void printSize();
	
	void save(const string &file_base);
	
	void load(const string &file_base, KarpRabin *_karp_rabin);
	
	
	
};







#endif //_RELZ_INDEX_HASH_H
