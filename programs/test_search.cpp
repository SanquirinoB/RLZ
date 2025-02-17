#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <algorithm>
#include <vector>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/inv_perm_support.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "ReferenceIndexBasic.h"
#include "CompressorSingleBuffer.h"
#include "CoderBlocksRelz.h"
#include "DecoderBlocksRelz.h"
#include "TextFilterFull.h"

#include "FactorsIterator.h"
#include "FactorsIteratorComparator.h"

using namespace sdsl;
using namespace std;

void recursive_rmq(unsigned long long ini, unsigned long long fin, unsigned long long crit, rmq_succinct_sct<false, bp_support_sada<256,32,rank_support_v5<> > > &rmq, int_vector<> &ez, inv_perm_support<> &perm){
	cout << " -> recursive_rmq(" << ini << ", " << fin << ")\n";
	
	unsigned long long pos_max = rmq(ini, fin);
	
	cout << " -> max pos Ez: " << pos_max << " (Ez: " << ez[pos_max] << ", factor: " << perm[pos_max] << ")\n";
	if( ez[pos_max] < crit ){
		cout << "Omitiendo\n";
		return;
	}
	else{
		cout << "Agregando\n";
	}
	
	if( (pos_max > 0) && (ini < pos_max) ){
		recursive_rmq(ini, pos_max-1, crit, rmq, ez, perm);
	}
	if( pos_max < fin ){
		recursive_rmq(pos_max+1, fin, crit, rmq, ez, perm);
	}
}

void recursive_rmq_v2(unsigned long long ini, unsigned long long fin, unsigned long long crit, rmq_succinct_sct<false, bp_support_sada<256,32,rank_support_v5<> > > &rmq, rrr_vector<127>::select_1_type &select1_s, rrr_vector<127>::select_1_type &select1_b, rrr_vector<127>::select_0_type &select0_b, inv_perm_support<> &perm){
	cout << " -> recursive_rmq(" << ini << ", " << fin << ")\n";
	
	unsigned long long pos_max = rmq(ini, fin);
	
	unsigned long long tu = select1_s(pos_max + 1) - pos_max;
	unsigned long long pu = select1_b(perm[pos_max] + 1);
	unsigned long long lu = select1_b(perm[pos_max] + 2) - pu;
	
//	cout << " -> max pos Ez: " << pos_max << " (Ez: " << ez[pos_max] << ", factor: " << perm[pos_max] << ")\n";
	cout << " -> max pos Ez: " << pos_max << " (tu: " << tu << ", pu: " << pu << ", lu: " << lu << ")\n";
	if( tu + lu < crit ){
		cout << "Omitiendo\n";
		return;
	}
	else{
		cout << "Agregando\n";
	}
	
	if( (pos_max > 0) && (ini < pos_max) ){
		recursive_rmq_v2(ini, pos_max-1, crit, rmq, select1_s, select1_b, select0_b, perm);
	}
	if( pos_max < fin ){
		recursive_rmq_v2(pos_max+1, fin, crit, rmq, select1_s, select1_b, select0_b, perm);
	}
}

unordered_map<unsigned long long, FactorsIterator> mapa_iterators;

char getChar(unsigned long long factor, unsigned long long pos, 
			unsigned long long n_factors, 
			rrr_vector<127>::select_1_type *select1_s, 
			rrr_vector<127>::select_1_type *select1_b, 
			rrr_vector<127>::select_0_type *select0_b, 
			inv_perm_support<> *perm, 
			inv_perm_support<> *perm_inv, 
			const char *ref_text, 
			unsigned long long full_size ){
	
	// Segundo enfoque: cache de iteradores completos
	if( mapa_iterators.find(factor) == mapa_iterators.end() ){
		mapa_iterators[factor] = FactorsIterator(factor, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
	}
	FactorsIterator it = mapa_iterators[factor];
	if( pos >= it.length() ){
		return 0;
	}
	if( it.position() > pos ){
		it.reset();
	}
	
	// Primer enfoque: sin caches 
	// (porque quiza haga un multiiterador con caches internos solo para los valores especificos necesarios)
//	FactorsIterator it( factor, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size );
	char c = 0;
	while( it.position() <= pos ){
		c = it.next();
	}
	return c;
}

unordered_map<unsigned long long, FactorsIteratorReverse> mapa_iterators_rev;

char getCharRev(unsigned long long factor, unsigned long long pos, 
			unsigned long long n_factors, 
			rrr_vector<127>::select_1_type *select1_s, 
			rrr_vector<127>::select_1_type *select1_b, 
			rrr_vector<127>::select_0_type *select0_b, 
			inv_perm_support<> *perm, 
			inv_perm_support<> *perm_inv, 
			const char *ref_text, 
			unsigned long long full_size ){
	
	if( factor == (unsigned long long)(-1) ){
		return 0;
	}
	
	// Segundo enfoque: cache de iteradores completos
	if( mapa_iterators_rev.find(factor) == mapa_iterators_rev.end() ){
		mapa_iterators_rev[factor] = FactorsIteratorReverse(factor, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
	}
	FactorsIteratorReverse it = mapa_iterators_rev[factor];
	if( pos >= it.length() ){
		return 0;
	}
	if( it.position() > pos ){
		it.reset();
	}
	char c = 0;
	while( it.position() <= pos ){
		c = it.next();
	}
	return c;
}

// Notar que, a diferencia de la busqueda en referencia, esta debe ser completa
// Es decir, solo importa el rango que contiene al patron completo
pair<unsigned long long, unsigned long long> getRangeY(
			const char *pattern,
			inv_perm_support<> &perm_y,
			unsigned long long n_factors, 
			rrr_vector<127>::select_1_type *select1_s, 
			rrr_vector<127>::select_1_type *select1_b, 
			rrr_vector<127>::select_0_type *select0_b, 
			inv_perm_support<> *perm, 
			inv_perm_support<> *perm_inv, 
			const char *ref_text, 
			unsigned long long full_size ){
	
	unsigned long long pat_len = strlen(pattern);
	unsigned long long izq = 0;
	unsigned long long der = n_factors-1;
	
	cout << "getRangeY - Inicio (pat_len: " << pat_len << ", izq: " << izq << ", der: " << der << ")\n";
	
	for( unsigned long long cur_pos = 0; cur_pos < pat_len; ++cur_pos ){
		cout << "getRangeY - cur_pos: " << cur_pos << " (pattern[" << cur_pos << "]: " << pattern[cur_pos] << ")\n";
		
		unsigned long long l = izq;
		unsigned long long h = der;
		unsigned long long m;
		unsigned long long fm;
		char c;
		unsigned long long text_len;
		
		// Busqueda binaria del lado izquierdo
		cout << "getRangeY - l: " << l << ", h: " << h << "\n";
		while(l < h){
			m = l + ((h-l)>>1);
			fm = perm_y[m];
//			if( arr[m] + cur_pos > largo || *(ref + arr[m] + cur_pos) < (unsigned char)(*(text + cur_pos)) ){
			c = getChar(fm, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
			text_len = mapa_iterators[fm].length();
			cout << "getRangeY - m: " << m << ", fm: " << fm << ", c: " << c << ", text_len: " << text_len << "\n";
			if( cur_pos > text_len || (unsigned char)(c) < (unsigned char)(pattern[cur_pos]) ){
				cout << "getRangeY - caso 1: l = " << (m+1) << "\n";
				l = m+1;
			}
			else{
				cout << "getRangeY - caso 2: h = " << m << "\n";
				h = m;
			}
		}
		izq = h;
		fm = perm_y[izq];
		c = getChar(fm, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
		text_len = mapa_iterators[fm].length();
		if( (cur_pos < text_len) && (unsigned char)(c) < (unsigned char)(pattern[cur_pos]) ){
			++izq;
		}
		cout << "getRangeY - izq: " << izq << "\n";
		cout << "getRangeY - -----\n";
		
		// Busqueda binaria del lado derecho
		l = izq;
		h = der;
		cout << "getRangeY - l: " << l << ", h: " << h << "\n";
		while(l < h){
			m = l + ((h-l)>>1);
			fm = perm_y[m];
			c = getChar(fm, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
			text_len = mapa_iterators[fm].length();
//			if( arr[m] + cur_pos > largo || *(ref + arr[m] + cur_pos) <= (unsigned char)(*(text + cur_pos)) ){
			cout << "getRangeY - m: " << m << ", fm: " << fm << ", c: " << c << ", text_len: " << text_len << "\n";
			if( cur_pos > text_len || (unsigned char)(c) <= (unsigned char)(pattern[cur_pos]) ){
				cout << "getRangeY - caso 1: l = " << (m+1) << "\n";
				l = m+1;
			}
			else{
				cout << "getRangeY - caso 2: h = " << m << "\n";
				h = m;
			}
		}
		der = h;
		fm = perm_y[der];
		c = getChar(fm, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
		text_len = mapa_iterators[fm].length();
		if( (cur_pos < text_len) && (unsigned char)(c) > (unsigned char)(pattern[cur_pos]) ){
			--der;
		}
		cout << "getRangeY - der: " << der << "\n";
		cout << "getRangeY - -----\n";
		
	}
	
	cout << "getRangeY - result: (" << izq << ", " << der << ")\n";
	return pair<unsigned long long, unsigned long long>(izq, der);
}

pair<unsigned long long, unsigned long long> getRangeX(
			const char *pattern,
			inv_perm_support<> &perm_x,
			unsigned long long n_factors, 
			rrr_vector<127>::select_1_type *select1_s, 
			rrr_vector<127>::select_1_type *select1_b, 
			rrr_vector<127>::select_0_type *select0_b, 
			inv_perm_support<> *perm, 
			inv_perm_support<> *perm_inv, 
			const char *ref_text, 
			unsigned long long full_size ){
	
	unsigned long long pat_len = strlen(pattern);
	unsigned long long izq = 0;
	unsigned long long der = n_factors-1;
	
	cout << "getRangeX - Inicio (pat_len: " << pat_len << ", izq: " << izq << ", der: " << der << ")\n";
	
	for( unsigned long long cur_pos = 0; cur_pos < pat_len; ++cur_pos ){
		cout << "getRangeX - cur_pos: " << cur_pos << " (pattern[" << cur_pos << "]: " << pattern[cur_pos] << ")\n";
		
		unsigned long long l = izq;
		unsigned long long h = der;
		unsigned long long m;
		unsigned long long fm;
		char c;
		unsigned long long text_len;
		
		// Busqueda binaria del lado izquierdo
		cout << "getRangeX - l: " << l << ", h: " << h << "\n";
		while(l < h){
			m = l + ((h-l)>>1);
			fm = perm_x[m];
//			if( arr[m] + cur_pos > largo || *(ref + arr[m] + cur_pos) < (unsigned char)(*(text + cur_pos)) ){
			c = getCharRev(fm-1, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
			text_len = mapa_iterators_rev[fm-1].length();
			cout << "getRangeX - m: " << m << ", fm: " << fm << ", c: " << c << ", text_len: " << text_len << "\n";
			if( cur_pos > text_len || (unsigned char)(c) < (unsigned char)(pattern[cur_pos]) ){
				cout << "getRangeX - caso 1: l = " << (m+1) << "\n";
				l = m+1;
			}
			else{
				cout << "getRangeX - caso 2: h = " << m << "\n";
				h = m;
			}
		}
		izq = h;
		fm = perm_x[izq];
		c = getCharRev(fm-1, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
		text_len = mapa_iterators_rev[fm-1].length();
		if( (cur_pos < text_len) && (unsigned char)(c) < (unsigned char)(pattern[cur_pos]) ){
			++izq;
		}
		cout << "getRangeX - izq: " << izq << "\n";
		cout << "getRangeX - -----\n";
		
		// Busqueda binaria del lado derecho
		l = izq;
		h = der;
		cout << "getRangeX - l: " << l << ", h: " << h << "\n";
		while(l < h){
			m = l + ((h-l)>>1);
			fm = perm_x[m];
			c = getCharRev(fm-1, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
			text_len = mapa_iterators_rev[fm-1].length();
//			if( arr[m] + cur_pos > largo || *(ref + arr[m] + cur_pos) <= (unsigned char)(*(text + cur_pos)) ){
			cout << "getRangeX - m: " << m << ", fm: " << fm << ", c: " << c << ", text_len: " << text_len << "\n";
			if( cur_pos > text_len || (unsigned char)(c) <= (unsigned char)(pattern[cur_pos]) ){
				cout << "getRangeX - caso 1: l = " << (m+1) << "\n";
				l = m+1;
			}
			else{
				cout << "getRangeX - caso 2: h = " << m << "\n";
				h = m;
			}
		}
		der = h;
		fm = perm_x[der];
		c = getCharRev(fm-1, cur_pos, n_factors, select1_s, select1_b, select0_b, perm, perm_inv, ref_text, full_size);
		text_len = mapa_iterators_rev[fm-1].length();
		if( (cur_pos < text_len) && (unsigned char)(c) > (unsigned char)(pattern[cur_pos]) ){
			--der;
		}
		cout << "getRangeX - der: " << der << "\n";
		cout << "getRangeX - -----\n";
		
	}
	
	cout << "getRangeX - result: (" << izq << ", " << der << ")\n";
	return pair<unsigned long long, unsigned long long>(izq, der);
}

int main() {
	
	string ref = "ALABARDAS";
	string text = "ALBA";
	text += "BALAS";
	text += "LALABAS";
	string input = "test_text.txt";
	string output = "test_output.relz";
	string serialized_reference = "test_ref.bin";
	
	vector<pair<unsigned long long, unsigned long long> > factors;
	
	unsigned long long len_ref = ref.length();
	unsigned long long len_text = text.length();
	unsigned long long z = 0;
	
	// Construir SA referencia
	// Esto eventualmente se convertira en un FM-index (csa_wt)
	ReferenceIndex *reference = new ReferenceIndexBasic(ref.c_str(), 1);
	reference->save(serialized_reference.c_str());
	
	// Preparar Compresor
	TextFilter *filter = new TextFilterFull();
	CompressorSingleBuffer compressor(
		output.c_str(), 
		new CoderBlocksRelz(reference), 
		new DecoderBlocksRelz(reference->getText()), 
		filter
	);
	
	compressor.compress(input.c_str(), 1, 1000000, 0, &factors);
	
	cout << "Factors: \n";
	// Factores en version ini, fin (absoluto) y ordenados por ini
//	vector<pair<unsigned long long, unsigned long long> > factors_sort;
	vector<pair<unsigned long long, pair<unsigned long long, unsigned long long> > > factors_sort;
	unsigned cur_pos = 0;
	for( pair<unsigned long long, unsigned long long> factor : factors ){
		cout << "(" << factor.first << ", " << factor.second << ", " << cur_pos << ")\n";
		factors_sort.push_back( 
			pair<unsigned long long, pair<unsigned long long, unsigned long long> >(
				factor.first, pair<unsigned long long, unsigned long long>(factor.first + factor.second - 1, cur_pos++)
				)
			);
	}
	sort(factors_sort.begin(), factors_sort.end());
	cout << "Factors Sorted: \n";
	for( pair<unsigned long long, pair<unsigned long long, unsigned long long> > factor : factors_sort ){
		cout << "(" << factor.first << ", " << factor.second.first << ", " << factor.second.second << ")\n";
	}
	z = factors_sort.size();
	
	// Bit vector S
	bit_vector arr_s(len_ref + z, 0);
	unsigned cur_ref = 0;
	cur_pos = 0;
	for( unsigned long long i = 0; i < z; ++i ){
		unsigned long long ini = factors_sort[i].first;
//		unsigned long long fin = factors_sort[i].second.first;
		if( ini == cur_ref ){
			arr_s[cur_pos++] = 1;
		}
		else{
			arr_s[cur_pos++] = 0;
			++cur_ref;
			--i;
		}
	}
	cout << "Bit Array S: \n";
	for( unsigned long long i = 0; i < len_ref + z; ++i ){
		cout << "arr_s[" << i << "]: " << arr_s[i] << "\n";
	}
	
	rrr_vector<127> rrr_s(arr_s);
	rrr_vector<127>::select_1_type select1_s(&rrr_s);
	cout << "Posicion de primer 1: " << select1_s(1) << "\n";
	cout << "Posicion de tercer 1: " << select1_s(3) << "\n";
	cout << "Posicion de quinto 1: " << select1_s(5) << "\n";
	
	// Notar que la posicion del select DEBE empezar desde 1, no desde 0
	// De este modo, hay que sumar 1 a las posiciones de la ref para buscar en S
	rrr_vector<127>::select_0_type select0_s(&rrr_s);
	cout << "Posicion de primer 0: " << select0_s(1) << "\n";
	cout << "Posicion de tercer 0: " << select0_s(3) << "\n";
	cout << "Posicion de quinto 0: " << select0_s(5) << "\n";
	cout << "Posicion de 0th 0: " << select0_s(0) << "\n";
	
	
	// Permutacion 
	int_vector<> pi(z);
	int_vector<> pi_inv(z);
	for( unsigned long long i = 0; i < z; ++i ){
		pi[i] = factors_sort[i].second.second;
		pi_inv[ factors_sort[i].second.second ] = i;
	}
	inv_perm_support<> perm_inv(&pi);
	inv_perm_support<> perm(&pi_inv);
	cout << "Permutation: \n";
	for( unsigned long long i = 0; i < z; ++i ){
		cout << "pi[" << i << "]: " << pi[i] << ", perm[" << i << "]: " << perm[i] << ", perm_inv[" << i << "]: " << perm_inv[i] << "\n";
	}
	
	// Posiciones finales Ez
//	vector<unsigned long long> ez(8);
	int_vector<> ez(z);
	for( unsigned long long i = 0; i < z; ++i ){
		ez[i] = factors_sort[i].second.first;
	}
	// rmq_succinct_sct<> rmq(&ez);
	rmq_succinct_sct<false, bp_support_sada<256,32,rank_support_v5<> > > rmq(&ez);
	// rmq_maximum_sct<> rmq(&ez);
	
	// Bit vector B (inicio de las frases en texto)
	bit_vector arr_b(len_text, 0);
	unsigned long long pos_text = 0;
	for( unsigned long long i = 0; i < z; ++i ){
//		unsigned long long ini = factors[i].first;
		unsigned long long len = factors[i].second;
		arr_b[ pos_text ] = 1;
		pos_text += len;
	}
	cout << "Bit Vector B: \n";
	for( unsigned long long i = 0; i < len_text; ++i ){
		cout << "B[" << i << "]: " << arr_b[i] << "\n";
	}
	rrr_vector<127> rrr_b(arr_b);
	rrr_vector<127>::select_1_type select1_b(&rrr_b);
	rrr_vector<127>::select_0_type select0_b(&rrr_b);
	
	// Construccion del FM Index (aunque use el SA original para la compresion, esto es para la busqueda)
	csa_wt<> fm_index;
	// Construccion con datos en memoria, en un string
	construct_im(fm_index, ref, 1);
	// Construccion con datos en un archivo
//	construct(fm_index, file, 1);
	
	cout << "Texto de ref: \"" << ref << "\"\n";
	// Notar que con extract SE PUEDE EXTRAER el texto, incluso caracter a caracter (aunque no tengo certeza del tiempo)
	cout << "fm_index[0]: \"" << fm_index[0] << "\", extract(0): " << extract(fm_index, 0, 0) << ", extract(1): " << extract(fm_index, 1, 1) << ", extract(2): " << extract(fm_index, 2, 2) << ", \n";
	
//	string query = "LA";
	string query = "BA";
	size_t m = query.size();
	size_t occs = sdsl::count(fm_index, query.begin(), query.end());
	cout << "# occs de \"" << query << "\": " << occs << "\n";
	if( occs > 0 ){
		auto locations = locate(fm_index, query.begin(), query.begin()+m);
		sort(locations.begin(), locations.end());
		for( unsigned long long i = 0; i < occs; ++i ){
			unsigned long long occ_i = locations[i];
			cout << "occ[" << i << "]: " << occ_i << " (" << ref.substr(occ_i, m) << ")\n";
			// Comprobar los factores que cuben esta ocurrencia (el string ref[occ_i, occ_i + m - 1])
			unsigned long long select = select0_s(occ_i + 1);
			unsigned long long pos_ez = select - 1 - occ_i;
			cout << "select: " << select << " => pos_ez: " << pos_ez << "\n";
			
			// Ahora la busqueda (recursiva) en el rmq (entre 0 y pos_ez)
//			unsigned long long pos_max = rmq(0, pos_ez);
//			cout << "max pos Ez: " << pos_max << " (Ez: " << ez[pos_max] << ", factor: " << perm[pos_max] << ")\n";
			cout << "----- Search V1 -----\n";
			recursive_rmq(0, pos_ez, (occ_i + m - 1), rmq, ez, perm);
			cout << "----- Search V2 -----\n";
			recursive_rmq_v2(0, pos_ez, (occ_i + m), rmq, select1_s, select1_b, select0_b, perm);
			cout << "----- -----\n";
			
		}
	}
	
	// Indice secundario
	cout << "Preparando estructuras de indice secundario\n";
	
	// Creo que seria ideal preparar iteradores de factor directo y reverso, que accedan a la referencia
	// Esas estructuras deberian poder usar las estructuras comprimidas para evaluar la info de los factores
	// Cada iterador internamente puede mantener los valores actuales dada un cur_pos actual
	// Basta con que los iteradores retornen el char de cierta pos FactorsIterator::get(unsigned long long pos) o "char FactorsIterator::next()"
	// A parte del next, necesitaria una forma de controlar el final del iterator, quizas "bool FactorsIterator::hasNext()"
	
	
	cout << "Probando Iterador\n";
	
	FactorsIterator it( 2, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	while( it.hasNext() ){
		char c = it.next();
		cout << "it.next(): " << c << " (text_pos " << it.position() << " / " << it.length() << ")\n";
		cout << "-----\n";
	}
	
	cout << "Fin prueba iterador\n";
	cout << "-----\n";
	
	cout << "Probando Iterador Reverso\n";
	
	FactorsIteratorReverse it_rev( 2, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}
	
	cout << "Fin prueba iterador\n";
	cout << "-----\n";

	
	cout << "Probando Iterador Reverso -1\n";
	it_rev = FactorsIteratorReverse( -1, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}

	cout << "Probando Iterador Reverso 0\n";
	it_rev = FactorsIteratorReverse( 0, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}

	cout << "Probando Iterador Reverso 1\n";
	it_rev = FactorsIteratorReverse( 1, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}

	cout << "Probando Iterador Reverso 5\n";
	it_rev = FactorsIteratorReverse( 5, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}

	cout << "Probando Iterador Reverso 7\n";
	it_rev = FactorsIteratorReverse( 7, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}
	
	cout << "-----\n";
		
	cout << "Probando Iterador Reverso 8\n";
	it_rev = FactorsIteratorReverse( 8, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}
	cout << "Probando reset\n";
	it_rev.reset();
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}

	cout << "-----\n";

	cout << "Probando Iterador Reverso 5\n";
	it_rev = FactorsIteratorReverse( 5, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}
	cout << "Probando reset\n";
	it_rev.reset();
	while( it_rev.hasNext() ){
		char c = it_rev.next();
		cout << "it_rev.next(): " << c << " (text_pos " << it_rev.position() << " / " << it_rev.length() << ")\n";
		cout << "-----\n";
	}
	
	cout << "-----\n";
	
	cout << "Probando acceso posicional\n";
	unsigned long long f = 5;
	for( unsigned long long i = 0; i < 10; ++i ){
		char c = getChar( f, i, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		if( c == 0){
			break;
		}
		cout << "factor_" << f << "[" << i << "]: " << c << " \n";
	}
	cout << "-----\n";
	f = 7;
	for( unsigned long long i = 0; i < 10; ++i ){
		char c = getChar( f, i, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		if( c == 0){
			break;
		}
		cout << "factor_" << f << "[" << i << "]: " << c << " \n";
	}
	cout << "-----\n";
	f = 3;
	for( unsigned long long i = 0; i < 10; ++i ){
		char c = getChar( f, i, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		if( c == 0){
			break;
		}
		cout << "factor_" << f << "[" << i << "]: " << c << " \n";
	}
	cout << "-----\n";
	
	cout << "Probando acceso posicional Reverso\n";
	f = 3;
	for( unsigned long long i = 0; i < 10; ++i ){
		char c = getCharRev( f-1, i, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		if( c == 0){
			break;
		}
		cout << "factor_rev_" << f << "[" << i << "]: " << c << " \n";
	}
	cout << "-----\n";
	f = 5;
	for( unsigned long long i = 0; i < 10; ++i ){
		char c = getCharRev( f-1, i, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		if( c == 0){
			break;
		}
		cout << "factor_rev_" << f << "[" << i << "]: " << c << " \n";
	}
	cout << "-----\n";
	cout << "factor_rev_" << f << "[" << 3 << "]: " << getCharRev( f-1, 3, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text) << " \n";
	cout << "factor_rev_" << f << "[" << 5 << "]: " << getCharRev( f-1, 5, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text) << " \n";
	cout << "factor_rev_" << f << "[" << 2 << "]: " << getCharRev( f-1, 2, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text) << " \n";
	cout << "factor_rev_" << f << "[" << 0 << "]: " << getCharRev( f-1, 0, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text) << " \n";
	cout << "-----\n";
	
	cout << "Preparando arr X\n";
	vector<unsigned long long> arr_x(z);
	for( unsigned long long i = 0; i < z; ++i ){
		arr_x[i] = i;
	}
	FactorsIteratorReverseComparator comp_rev(z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	stable_sort(arr_x.begin(), arr_x.end(), comp_rev);
	int_vector<> pre_x_inv(z);
	for( unsigned long long i = 0; i < z; ++i ){
		pre_x_inv[ arr_x[i] ] = i;
	}
	inv_perm_support<> perm_x(&pre_x_inv);
	for( unsigned long long i = 0; i < z; ++i ){
		pre_x_inv[ arr_x[i] ] = i;
		cout << " arr_x[" << i << "]: " << arr_x[i] << " (perm_x[" << i << "]: " << perm_x[i] << ") \n";
	}
	cout << "-----\n";
	
	cout << "Preparando arr Y\n";
	vector<unsigned long long> arr_y(z);
	for( unsigned long long i = 0; i < z; ++i ){
		arr_y[i] = i;
	}
	FactorsIteratorComparator comp(z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	stable_sort(arr_y.begin(), arr_y.end(), comp);
	int_vector<> pre_y(z);
	int_vector<> pre_y_inv(z);
	for( unsigned long long i = 0; i < z; ++i ){
		pre_y[i] = arr_y[i];
		pre_y_inv[ arr_y[i] ] = i;
	}
	inv_perm_support<> perm_y_inv(&pre_y);
	inv_perm_support<> perm_y(&pre_y_inv);
	for( unsigned long long i = 0; i < z; ++i ){
		cout << " arr_y[" << i << "]: " << arr_y[i] << " (perm_y[" << i << "]: " << perm_y[i] << ", perm_y_inv[" << i << "]: " << perm_y_inv[i] << ")\n";
	}
	cout << "-----\n";
	
	cout << "Preparando WT\n";
	int_vector<> values_wt(z);
	for( unsigned long long i = 0; i < z; ++i ){
		values_wt[i] = perm_y_inv[ arr_x[ i ] ];
		cout << " values_wt[" << i << "]: " << values_wt[i] << " \n";
	}
	
	wt_int<rrr_vector<63>> wt;
	construct_im(wt, values_wt);
	
//	cout << "Buscando en [1, 2] x [1, 5]:\n";
//	auto res = wt.range_search_2d(1, 2, 1, 5);
//	for (auto point : res.second){
//		cout << "(" << point.first << ", " << point.second << ")\n";
//	}
	
	cout << "Buscando en [1, 5] x [1, 2]:\n";
	auto res = wt.range_search_2d(1, 5, 1, 2);
	for (auto point : res.second){
		cout << "(" << point.first << ", " << point.second << ") => factor " << perm_y[point.second] << "\n";
		// Aqui tengo el id posicional del factor
		// Puedo sacar sus datos con las formulas para tu, pu, lu
		// Es necesario leer tambien el factor anterior tambien, pero la posicion se tiene
		// Notar que las posiciones de corte en m las conozco porque itero por ella (en m^2)
	}
	
	cout << "Realizando busquedas reales en el WT\n";
	// El codigo de la busqueda de rangos deberia estar basado en el codigo de reference
	
	cout << "Prueba de patron \"A\"\n";
	getRangeY("A", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"B\"\n";
	getRangeY("B", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"BA\"\n";
	getRangeY("BA", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"BAL\"\n";
	getRangeY("BAL", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"BALB\"\n";
	getRangeY("BALB", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"Z\"\n";
	getRangeY("Z", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"0\"\n";
	getRangeY("0", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	cout << "Prueba de patron \"\"\n";
	getRangeY("", perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
	cout << "-----\n";
	
	
	string pattern = "AB";
	for(unsigned long long i = 1; i < pattern.length(); ++i){
		string p1 = pattern.substr(0, i);
		string p2 = pattern.substr(i, pattern.length() - i);
		cout << "Corte de \"" << pattern << "\": (" << p1 << "| " << p2 << ")\n";
		pair<unsigned long long, unsigned long long> r1 = getRangeX(p1.c_str(), perm_x, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		pair<unsigned long long, unsigned long long> r2 = getRangeY(p2.c_str(), perm_y, z, &select1_s, &select1_b, &select0_b, &perm, &perm_inv, ref.c_str(), len_text);
		
		if( r1.second == (unsigned long long)(-1) || r1.second < r1.first
			|| r2.second == (unsigned long long)(-1) || r2.second < r1.first ){
			cout << "Rangos Invalidos, omitiendo...\n";
			continue;
		}
		
		cout << "Buscando en [" << r1.first << ", " << r1.second << "] x [" << r2.first << ", " << r2.second << "]:\n";
		auto res = wt.range_search_2d(r1.first, r1.second, r2.first, r2.second);
		for (auto point : res.second){
			cout << "(" << point.first << ", " << point.second << ") => factor " << perm_y[point.second] << "\n";
		}
		
	}
	
	
	delete reference;




}




















