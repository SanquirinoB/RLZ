#include "FactorsIterator.h"

// Metodos de FactorsIterator Normal

void FactorsIterator::loadFactor(unsigned long long f, bool reset){
	cur_f = f;
	if( cur_f == (unsigned long long)(-1) || cur_f >= n_factors){
		text_length = 0;
		text_pos = 0;
		return;
	}
	// Convertir el factor posicional creciente a la posicion EN la permutacion
	unsigned long long pu = select1_b->operator()(cur_f + 1);
	unsigned long long lu = select1_b->operator()(cur_f + 2) - pu;	

   cur_pos = 1;
   cur_f_fin = lu;	
	bwt_pos = (*pi_inv_new)[cur_f];
	// valores como string independiente
	if(reset){
		text_length = full_size - pu;
		text_pos = 0;
	}
}

FactorsIterator::FactorsIterator(){
	start_f = 0;
	n_factors = 0;
	select1_s = NULL;
	select1_b = NULL;
	select0_b = NULL;
	pi_inv = NULL;
	ref_text = NULL;
	fm_index = NULL;
	full_size = 0;
	cur_pos = 0;
	cur_f = 0;
	cur_f_ini = 0;
	cur_f_fin = 0;
	text_length = 0;
	text_pos = 0;
	max_length = 100;
	reset();
}

FactorsIterator::FactorsIterator( unsigned long long _start_f, unsigned long long _n_factors, 
		bits_s_type::select_1_type *_select1_s, 
		bits_b_type::select_1_type *_select1_b, 
		bits_b_type::select_0_type *_select0_b,
		int_vector<> *_pi_inv,
		int_vector<> *_pi_inv_new,
		int_vector<> *_arr_x_new,
		const char *_ref_text,
		fm_index_type *_fm_index,
		unsigned long long _full_size ){
	start_f = _start_f;
	n_factors = _n_factors;
	select1_s = _select1_s;
	select1_b = _select1_b;
	select0_b = _select0_b;
	pi_inv = _pi_inv;	
	pi_inv_new = _pi_inv_new;
	arr_x_new = _arr_x_new; // OJO, eliminar esto cuando esté listo
	ref_text = _ref_text;
	fm_index = _fm_index;
	full_size = _full_size;
	cur_pos = 0;
	cur_f = 0;
	cur_f_ini = 0;
	cur_f_fin = 0;
	max_length = 100;
	reset();
}

void FactorsIterator::reset(){
//	cout << "FactorsIterator::reset\n";
	loadFactor( start_f, true );
}

uint64_t FL(fm_index_type *fm_index, uint64_t i)
 {
     if (i == 1) return fm_index->wavelet_tree.select(1, fm_index->comp2char[0]);
     uint32_t c = 0;
     while (c <= fm_index->sigma && fm_index->C[c] <= i) c++;
     c--;
     uint32_t j = i - fm_index->C[c] + 1;
     return fm_index->wavelet_tree.select(j, fm_index->comp2char[c]);
 }


char FactorsIterator::next(){
//	cout << "FactorsIterator::next - cur_pos: " << cur_pos << ", cur_f_fin: " << cur_f_fin << ", cur_f: " << cur_f << " / " << n_factors << "\n";
	char ret = 0;
	if( ref_text != NULL ){
		ret = ref_text[cur_pos];
	}
	else{
      ret = fm_index->bwt[bwt_pos];
      bwt_pos = FL(fm_index, bwt_pos);
	}
	++cur_pos;
	++text_pos;
	if( (cur_pos > cur_f_fin) && (cur_f < n_factors-1) ){
		loadFactor(++cur_f);
	}
	return ret;
}

bool FactorsIterator::hasNext(){
	if( text_pos < text_length ){
		return true;
	}
	return false;
}

unsigned long long FactorsIterator::length(){
	return text_length;
}

unsigned long long FactorsIterator::position(){
	return text_pos;
}

// Metodos de FactorsIteratorReverse

void FactorsIteratorReverse::loadFactor(unsigned long long f, bool reset){
	cur_f = f;
	if( cur_f == (unsigned long long)(-1) || cur_f >= n_factors){
		text_length = 0;
		text_pos = 0;
		return;
	}
	// Convertir el factor posicional creciente a la posicion EN la permutacion 
	
	unsigned long long pu = select1_b->operator()(cur_f + 1);
	unsigned long long lu = select1_b->operator()(cur_f + 2) - pu;
	   
   bwt_pos = (*arr_x_new)[cur_f];	
   
	// valores como string independiente
	if(reset){
		// Notar que este texto INCLUYE al factor actual
		// El llamador obviamente pide el prefijo de f inverso como (f-1)
		text_length = lu;
		text_pos = 0;
	}
}

FactorsIteratorReverse::FactorsIteratorReverse(){
	start_f = 0;
	n_factors = 0;
	select1_s = NULL;
	select1_b = NULL;
	select0_b = NULL;
	pi_inv = NULL;
	ref_text = NULL;
	fm_index = NULL;
	full_size = 0;
	cur_pos = 0;
	cur_f = 0;
	cur_f_ini = 0;
	cur_f_fin = 0;
	text_length = 0;
	text_pos = 0;
	max_length = 100;
	reset();
}

FactorsIteratorReverse::FactorsIteratorReverse( unsigned long long _start_f, unsigned long long _n_factors, 
		bits_s_type::select_1_type *_select1_s, 
		bits_b_type::select_1_type *_select1_b, 
		bits_b_type::select_0_type *_select0_b, 
		int_vector<> *_pi_inv,
		int_vector<> *_pi_inv_new,
		int_vector<> *_arr_x_new,
		const char *_ref_text,
		fm_index_type *_fm_index,
		unsigned long long _full_size ){
	start_f = _start_f;
	n_factors = _n_factors;
	select1_s = _select1_s;
	select1_b = _select1_b;
	select0_b = _select0_b;
	pi_inv = _pi_inv;
	pi_inv_new = _pi_inv_new;
	arr_x_new = _arr_x_new; // OJO, eliminar esto cuando esté listo
	ref_text = _ref_text;
	fm_index = _fm_index;
	full_size = _full_size;
	cur_pos = 0;
	cur_f = 0;
	cur_f_ini = 0;
	cur_f_fin = 0;
	max_length = 100;
	reset();
}

void FactorsIteratorReverse::reset(){
	loadFactor( start_f, true );
}

char FactorsIteratorReverse::next(){
	char ret = 0;
	if( ref_text != NULL ){
		ret = ref_text[cur_pos];
	}
	else{
      ret = fm_index->bwt[bwt_pos];
      bwt_pos = fm_index->lf[bwt_pos];
	}
	--cur_pos;
	++text_pos;
	return ret;
}

bool FactorsIteratorReverse::hasNext(){
	if( text_pos < text_length ){
		return true;
	}
	return false;
}

unsigned long long FactorsIteratorReverse::length(){
	return text_length;
}

unsigned long long FactorsIteratorReverse::position(){
	return text_pos;
}

