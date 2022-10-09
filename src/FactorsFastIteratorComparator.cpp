#include "FactorsFastIteratorComparator.h"

FactorsFastIteratorComparator::FactorsFastIteratorComparator(){
	factors_starts = NULL;
	full_text = NULL;
	full_size = 0;
}

FactorsFastIteratorComparator::FactorsFastIteratorComparator(vector<unsigned long long> *_factors_starts, char *_full_text, unsigned long long _full_size) {
	factors_starts = _factors_starts;
	full_text = _full_text;
	full_size = _full_size;
}

FactorsFastIteratorReverseComparator::FactorsFastIteratorReverseComparator(){
	factors_starts = NULL;
	full_text = NULL;
	full_size = 0;
}

FactorsFastIteratorReverseComparator::FactorsFastIteratorReverseComparator(vector<unsigned long long> *_factors_starts, char *_full_text, unsigned long long _full_size) {
	factors_starts = _factors_starts;
	full_text = _full_text;
	full_size = _full_size;
}

CompactedFactorsFastIteratorComparator::CompactedFactorsFastIteratorComparator(){
	// factors_starts = NULL;
	// factors = NULL;
	ref_text = NULL;
}

CompactedFactorsFastIteratorReverseComparator::CompactedFactorsFastIteratorReverseComparator(){
	// factors_starts = NULL;
	// factors = NULL;
	ref_text = NULL;
}

CompactedFactorsFastIteratorComparator::CompactedFactorsFastIteratorComparator(vector<pair<unsigned long long, unsigned long long>> &_factors, const char *_ref_text){
	factors = _factors;
	n_factors = _factors.size() - 1;
	ref_text = _ref_text;
}

CompactedFactorsFastIteratorReverseComparator::CompactedFactorsFastIteratorReverseComparator(vector<pair<unsigned long long, unsigned long long>> &_factors, const char *_ref_text){
	factors = _factors;
	ref_text = _ref_text;
}

