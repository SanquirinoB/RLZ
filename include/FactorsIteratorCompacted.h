#ifndef _FACTORS_ITERATOR_COMPACTED_H
#define _FACTORS_ITERATOR_COMPACTED_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>
#include <vector>

#include "FactorsConfig.h"
#include "CompactedText.h"

using namespace sdsl;
using namespace std;

class FactorsIteratorCompacted {

private: 
	
	bits_s_type::select_1_type *select1_s;
	bits_b_type::select_1_type *select1_b;
	bits_b_type::select_0_type *select0_b;
	int_vector<> *pi_inv;
	
	CompactedText *ref_text;
	
	// Largo de la coleccion completa comprimida
	unsigned long long full_size;
	
	// Posicion actual en la referencia para el factor actual
	unsigned long long cur_pos;
	
	// Largo y posicion actual en el texto comprimido total 
	// Es decir, su largo y posicion como si fuese un string independiente
	// Estos datos pueden ser usados para simplificar comparaciones de strings
	unsigned long long text_length;
	unsigned long long text_pos;
	unsigned long long max_length;
	
	// Factor actual, con posiciones para la referencia
	unsigned long long start_f;
	unsigned long long n_factors;
	unsigned long long cur_f;
	unsigned long long cur_f_ini;
	unsigned long long cur_f_fin;
	
	void loadFactor(unsigned long long f, bool reset = false);
	
public: 
	
	FactorsIteratorCompacted();
	
	FactorsIteratorCompacted( unsigned long long _start_f, unsigned long long _n_factors, 
			bits_s_type::select_1_type *_select1_s, 
			bits_b_type::select_1_type *_select1_b, 
			bits_b_type::select_0_type *_select0_b, 
			int_vector<> *_pi_inv,
			CompactedText *_ref_text,
			unsigned long long _full_size );
	
	void reset();
	
	char next();
	
	bool hasNext();
	
	unsigned long long length();
	
	unsigned long long position();
	
	void setMaxLength(unsigned long long _max_length){
		max_length = _max_length;
	}
	
};

class FactorsIteratorCompactedReverse {

private: 
	
	bits_s_type::select_1_type *select1_s;
	bits_b_type::select_1_type *select1_b;
	bits_b_type::select_0_type *select0_b;
	int_vector<> *pi_inv;
	
	CompactedText *ref_text;
	
	// Largo de la coleccion completa comprimida
	unsigned long long full_size;
	
	// Posicion actual en la referencia para el factor actual
	unsigned long long cur_pos;
	
	// Largo y posicion actual en el texto comprimido total 
	// Es decir, su largo y posicion como si fuese un string independiente
	// Estos datos pueden ser usados para simplificar comparaciones de strings
	unsigned long long text_length;
	unsigned long long text_pos;
	unsigned long long max_length;
	
	// Factor actual, con posiciones para la referencia
	unsigned long long start_f;
	unsigned long long n_factors;
	unsigned long long cur_f;
	unsigned long long cur_f_ini;
	unsigned long long cur_f_fin;
	
	void loadFactor(unsigned long long f, bool reset = false);
	
public: 
	
	FactorsIteratorCompactedReverse();
	
	FactorsIteratorCompactedReverse( unsigned long long _start_f, unsigned long long _n_factors, 
			bits_s_type::select_1_type *_select1_s, 
			bits_b_type::select_1_type *_select1_b, 
			bits_b_type::select_0_type *_select0_b, 
			int_vector<> *_pi_inv,
			CompactedText *_ref_text,
			unsigned long long _full_size );
	
	void reset();
	
	char next();
	
	bool hasNext();
	
	unsigned long long length();
	
	unsigned long long position();
	
	void setMaxLength(unsigned long long _max_length){
		max_length = _max_length;
	}
	
};


#endif //_FACTORS_ITERATOR_H
