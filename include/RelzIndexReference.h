#ifndef _RELZ_INDEX_REFERENCE_H
#define _RELZ_INDEX_REFERENCE_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include <algorithm>
#include <vector>
#include <unordered_map>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/inv_perm_support.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "FactorsConfig.h"
#include "FactorsIterator.h"
#include "FactorsFastIteratorComparator.h"
#include "NanoTimer.h"
#include "CompactedText.h"
#include "FactorsIteratorCompacted.h"

using namespace sdsl;
using namespace std;


class RelzIndexReference {

private: 
	
	unsigned long long len_text;
	
	CompactedText *ref_text;
//	char *ref_text;
//	unsigned long long len_ref;
	
	unsigned long long n_factors;
	
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
	
	void recursive_rmq(unsigned long long ini, unsigned long long fin, unsigned long long min_pos, unsigned long long occ_ref, vector<unsigned long long> &results);
			
	pair<unsigned long long, unsigned long long> getRangeY(const char *pattern);
	pair<unsigned long long, unsigned long long> getRangeX(const char *pattern);
	
	template <typename ItereatorType>
	bool factorLess(unsigned long long factor, const char *pattern, unsigned long long len, bool equal = false);
	
public: 
	RelzIndexReference();
	RelzIndexReference(vector<pair<unsigned long long, unsigned long long> > &factors, char *full_text, unsigned long long _len_text, const char *_ref_text, unsigned long long _len_ref);
	~RelzIndexReference();
	
	void findTimes(const string &pattern, vector<unsigned long long> &results);
	
	double getSize();
	
	void save(const string &file_base);
	
	void load(const string &file_base);
	
	unsigned long long querytime_p1;
	unsigned long long querytime_p2;
	unsigned long long querytime_p3;
	unsigned long long querytime_p4;
	
	unsigned long long occs_a;
	unsigned long long occs_b;
	unsigned long long occs_c;
	
};








#endif //_RELZ_INDEX_REFERENCE_H
