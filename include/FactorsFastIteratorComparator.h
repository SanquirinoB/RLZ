#ifndef _FACTORS_FAST_ITERATOR_COMPARATOR_H
#define _FACTORS_FAST_ITERATOR_COMPARATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>
#include <vector>

/*#include "FactorsIterator.h"*/

using namespace std;

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

#endif //_FACTORS_ITERATOR_COMPARATOR_H
