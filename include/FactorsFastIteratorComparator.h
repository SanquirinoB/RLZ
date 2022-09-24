#ifndef _FACTORS_FAST_ITERATOR_COMPARATOR_H
#define _FACTORS_FAST_ITERATOR_COMPARATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>
#include <vector>
#include <sdsl/bit_vectors.hpp>

/*#include "FactorsIterator.h"*/

using namespace std;
using namespace sdsl;

class FactorsFastIteratorComparator : public std::binary_function<unsigned long long, unsigned long long, bool> {
private:
	vector<unsigned long long> *factors_starts;
	char *full_text;
	unsigned long long full_size;

public:
	FactorsFastIteratorComparator();
	
	FactorsFastIteratorComparator(vector<unsigned long long> *_factors_starts, char *_full_text, unsigned long long _full_size);
	
	inline bool operator()(const unsigned long long a, const unsigned long long b){
		unsigned long long start_a = factors_starts->at(a);
		unsigned long long start_b = factors_starts->at(b);
		unsigned long long len_a = full_size - start_a;
		unsigned long long len_b = full_size - start_b;
		for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
			if( full_text[ start_a + i ] < full_text[ start_b + i ] ){
				return true;
			}
			if( full_text[ start_a + i ] > full_text[ start_b + i ] ){
				return false;
			}
		}
		return (len_a < len_b);
	}
};

class FactorsFastIteratorReverseComparator : public std::binary_function<unsigned long long, unsigned long long, bool> {
private:
	vector<unsigned long long> *factors_starts;
	char *full_text;
	unsigned long long full_size;

public:
	FactorsFastIteratorReverseComparator();
	
	FactorsFastIteratorReverseComparator(vector<unsigned long long> *_factors_starts, char *_full_text, unsigned long long _full_size);
	
	inline bool operator()(const unsigned long long a, const unsigned long long b){
		if( a == 0 ){
			return true;
		}
		if( b == 0){
			return false;
		}
/*		cout << "Comp " << a << ", " << b << "\n";*/
		unsigned long long start_a = factors_starts->at(a) - 1;
		unsigned long long start_b = factors_starts->at(b) - 1;
		unsigned long long len_a = factors_starts->at(a) - factors_starts->at(a-1);
		unsigned long long len_b = factors_starts->at(b) - factors_starts->at(b-1);
		for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
			if( full_text[ start_a - i ] < full_text[ start_b - i ] ){
				return true;
			}
			if( full_text[ start_a - i ] > full_text[ start_b - i ] ){
				return false;
			}
		}
		return (len_a < len_b);
	}
};

class CompactedFactorsFastIteratorComparator : public std::binary_function<unsigned long long, unsigned long long, bool>
{
private:
	sd_vector<unsigned long long>::rank_1_type rank_factors;
    sd_vector<unsigned long long>::select_1_type select_factors;
	vector<pair<unsigned long long, unsigned long long>> *factors;
	char *ref_text;
	unsigned long long full_size;

	//	Cache data   									  (pos_text, ref_pos)
	pair<unsigned long long, unsigned long long> last_a = make_pair(0ULL, 0ULL);
	pair<unsigned long long, unsigned long long> last_b = make_pair(0ULL, 0ULL);

public:
	CompactedFactorsFastIteratorComparator();

	CompactedFactorsFastIteratorComparator(sd_vector<unsigned long long> *_factors_starts, vector<pair<unsigned long long, unsigned long long>> *_factors, char *_ref_text, unsigned long long _full_size);

	inline bool operator()(const unsigned long long a, const unsigned long long b){
			unsigned long long start_a = select_factors(a);
			unsigned long long start_b = select_factors(b);

			unsigned long long len_a = full_size - start_a;
			unsigned long long len_b = full_size - start_b;

			// Helper
			unsigned long long ref_pos_a, factor_a, offset_a;
			unsigned long long ref_pos_b, factor_b, offset_b;

			for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
				// Cached part
				if(last_a.first + 1ULL == start_a + i)
				{
					ref_pos_a = last_a.second + 1ULL;
					last_a = make_pair(start_a + i, ref_pos_a);
				} else {
					factor_a = rank_factors(start_a + i);
					offset_a = start_a + i - select_factors(factor_a);
					ref_pos_a = factors->at(factor_a).first + offset_a; 
				}

				if(last_b.first + 1ULL == start_b + i)
				{
					ref_pos_b = last_b.second + 1ULL;
					last_b = make_pair(start_b + i, ref_pos_b);
				} else {
					factor_b = rank_factors(start_b + i);
					offset_b = start_b + i - select_factors(factor_b);
					ref_pos_b = factors->at(factor_b).first + offset_b; 
				}

				if( ref_text[ref_pos_a] < ref_text[ref_pos_b] ){
					return true;
				}
				if( ref_text[ref_pos_a] > ref_text[ref_pos_b] ){
					return false;
				}
			}
			return (len_a < len_b);
		}
};

class CompactedFactorsFastIteratorReverseComparator : public std::binary_function<unsigned long long, unsigned long long, bool>
{
private:
	sd_vector<unsigned long long>::rank_1_type rank_factors;
    sd_vector<unsigned long long>::select_1_type select_factors;
	vector<pair<unsigned long long, unsigned long long>> *factors;
	char *ref_text;
	unsigned long long full_size;

	//	Cache data   									  (pos_text, ref_pos)
	pair<unsigned long long, unsigned long long> last_a = make_pair(0ULL, 0ULL);
	pair<unsigned long long, unsigned long long> last_b = make_pair(0ULL, 0ULL);

public:
	CompactedFactorsFastIteratorReverseComparator();

	CompactedFactorsFastIteratorReverseComparator(sd_vector<unsigned long long> *_factors_starts, vector<pair<unsigned long long, unsigned long long>> *_factors, char *_ref_text, unsigned long long _full_size);

	inline bool operator()(const unsigned long long a, const unsigned long long b){
		if( a == 0 ){
			return true;
		}
		if( b == 0){
			return false;
		}

		unsigned long long start_a = select_factors(a) - 1;
		unsigned long long start_b = select_factors(b) - 1;

		unsigned long long len_a = select_factors(a) - select_factors(a-1);
		unsigned long long len_b = select_factors(b) - select_factors(b-1);

		// Helper
		unsigned long long ref_pos_a, factor_a, offset_a;
		unsigned long long ref_pos_b, factor_b, offset_b;

		for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
			if(last_a.first - 1ULL == start_a - i)
			{
				ref_pos_a = last_a.second - 1ULL;
				last_a = make_pair(start_a - i, ref_pos_a);
			} else {
				// Creo que esto tiene sentido, pues el proceso consta de mapear pos = start_X +- i a donde viva en R
				factor_a = rank_factors(start_a - i);
				offset_a = start_a - i - select_factors(factor_a); // consultar si tiene sentido
				ref_pos_a = factors->at(factor_a).first + offset_a; 
			}
			if( ref_text[ref_pos_a] < ref_text[ref_pos_b] ){
				return true;
			}
			if( ref_text[ref_pos_a] > ref_text[ref_pos_b] ){
				return false;
			}
		}
		return (len_a < len_b);
	}	
};

#endif //_FACTORS_ITERATOR_COMPARATOR_H
