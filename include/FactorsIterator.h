#ifndef _FACTORS_ITERATOR_H
#define _FACTORS_ITERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>
#include <vector>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "FactorsConfig.h"

using namespace sdsl;
using namespace std;

class FactorsIterator {

private: 
	
	bits_s_type::select_1_type *select1_s;
	bits_b_type::select_1_type *select1_b;
	bits_b_type::select_0_type *select0_b;
	int_vector<> *pi_inv;
	int_vector<> *pi_inv_new;
	int_vector<> *arr_x_new; // OJO BORRAR ESTO
	
	// Texto de la referencia descomprimido
	const char *ref_text;
	
	// FM-Index en lugar del texto
	fm_index_type *fm_index;
	
	// Largo de la coleccion completa comprimida
	unsigned long long full_size;
	
	// Posicion actual en la referencia para el factor actual
	unsigned long long cur_pos;
	
	// Largo y posicion actual en el texto comprimido total 
	// Es decir, su largo y posicion como si fuese un string independiente
	// Estos datos pueden ser usados para simplificar comparaciones de strings
	unsigned long long text_length;
	unsigned long long text_pos;
	string str_f;
	unsigned long long max_length;
	unsigned long long bwt_pos;
	
	// Factor actual, con posiciones para la referencia
	unsigned long long start_f;
	unsigned long long n_factors;
	unsigned long long cur_f;
	unsigned long long cur_f_ini;
	unsigned long long cur_f_fin;
	
	void loadFactor(unsigned long long f, bool reset = false);
	
public: 
	
	FactorsIterator();
	
	FactorsIterator( unsigned long long _start_f, unsigned long long _n_factors, 
			bits_s_type::select_1_type *_select1_s, 
			bits_b_type::select_1_type *_select1_b, 
			bits_b_type::select_0_type *_select0_b, 
			int_vector<> *_pi_inv,
			int_vector<> *_pi_inv_new,
			int_vector<> *_arr_x_new,
			const char *_ref_text,
			fm_index_type *_fm_index,
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

class FactorsIteratorReverse {

private: 
	
	bits_s_type::select_1_type *select1_s;
	bits_b_type::select_1_type *select1_b;
	bits_b_type::select_0_type *select0_b;
	int_vector<> *pi_inv;
	int_vector<> *pi_inv_new;
	int_vector<> *arr_x_new; // OJO BORRAR ESTO
	
	const char *ref_text;
	
	// FM-Index en lugar del texto
	fm_index_type *fm_index;
	
	// Largo de la coleccion completa comprimida
	unsigned long long full_size;
	
	// Posicion actual en la referencia para el factor actual
	unsigned long long cur_pos;
	
	// Largo y posicion actual en el texto comprimido total 
	// Es decir, su largo y posicion como si fuese un string independiente
	// Estos datos pueden ser usados para simplificar comparaciones de strings
	unsigned long long text_length;
	unsigned long long text_pos;
	string str_f;
	unsigned long long max_length;
	unsigned long long bwt_pos;

	
	// Factor actual, con posiciones para la referencia
	unsigned long long start_f;
	unsigned long long n_factors;
	unsigned long long cur_f;
	unsigned long long cur_f_ini;
	unsigned long long cur_f_fin;
	
	void loadFactor(unsigned long long f, bool reset = false);
	
public: 
	
	FactorsIteratorReverse();
	
	FactorsIteratorReverse( unsigned long long _start_f, unsigned long long _n_factors, 
			bits_s_type::select_1_type *_select1_s, 
			bits_b_type::select_1_type *_select1_b, 
			bits_b_type::select_0_type *_select0_b, 
			int_vector<> *_pi_inv,
			int_vector<> *_pi_inv_new,
			int_vector<> *_arr_x_new,
			const char *_ref_text,
			fm_index_type *_fm_index,
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
