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
	full_size = 0;
}

CompactedFactorsFastIteratorReverseComparator::CompactedFactorsFastIteratorReverseComparator(){
	// factors_starts = NULL;
	// factors = NULL;
	ref_text = NULL;
	full_size = 0;
}

CompactedFactorsFastIteratorComparator::CompactedFactorsFastIteratorComparator(sd_vector<> *_factors_starts, vector<pair<unsigned long long, unsigned long long>> &_factors, const char *_ref_text, unsigned long long _full_size){
	// factors_starts = _factors_starts;
	rank_factors = sd_vector<>::rank_1_type(_factors_starts);
    select_factors = sd_vector<>::select_1_type(_factors_starts);
	factors = _factors;
	ref_text = _ref_text;
	full_size = _full_size;
}

CompactedFactorsFastIteratorReverseComparator::CompactedFactorsFastIteratorReverseComparator(sd_vector<> *_factors_starts, vector<pair<unsigned long long, unsigned long long>> &_factors, const char *_ref_text, unsigned long long _full_size){
	// factors_starts = _factors_starts;
	rank_factors = sd_vector<>::rank_1_type(_factors_starts);
    select_factors = sd_vector<>::select_1_type(_factors_starts);
	factors = _factors;
	ref_text = _ref_text;
	full_size = _full_size;
}

