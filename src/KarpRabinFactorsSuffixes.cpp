#include "KarpRabinFactorsSuffixes.h"

KarpRabinFactorsSuffixes::KarpRabinFactorsSuffixes(){
	n_factors = 0;
	arr_kr_s = NULL;
	delete_krs = false;
	karp_rabin = NULL;
	ref_text = NULL;
	compacted_text = NULL;
	select1_s = NULL;
	select1_b = NULL;
	select0_b = NULL;
	pi_inv = NULL;
}

KarpRabinFactorsSuffixes::KarpRabinFactorsSuffixes(unsigned long long _n_factors, 
		vector<unsigned long long> *_arr_kr_s, 
		KarpRabin *_karp_rabin, 
		const char *_ref_text, 
		bits_s_type::select_1_type *_select1_s, 
		bits_b_type::select_1_type *_select1_b, 
		bits_b_type::select_0_type *_select0_b, 
		int_vector<> *_pi_inv){
	n_factors = _n_factors;
	arr_kr_s = _arr_kr_s;
	delete_krs = false;
	karp_rabin = _karp_rabin;
	if( arr_kr_s->size() < n_factors+1 ){
		cout << "KarpRabinFactorsSuffixes - Warning (insufficient prefixes, must include one for the whole collection)\n";
	}
	ref_text = _ref_text;
	compacted_text = NULL;
	select1_s = _select1_s;
	select1_b = _select1_b;
	select0_b = _select0_b;
	pi_inv = _pi_inv;
}

KarpRabinFactorsSuffixes::KarpRabinFactorsSuffixes(const string &file, 
		KarpRabin *_karp_rabin, 
		const char *_ref_text, 
		bits_s_type::select_1_type *_select1_s, 
		bits_b_type::select_1_type *_select1_b, 
		bits_b_type::select_0_type *_select0_b, 
		int_vector<> *_pi_inv){
	// Basic variables loaded from file
	n_factors = 0;
	arr_kr_s = 0;
	delete_krs = false;
	
	load(file);
	// External variables
	karp_rabin = _karp_rabin;
	ref_text = _ref_text;
	compacted_text = NULL;
	select1_s = _select1_s;
	select1_b = _select1_b;
	select0_b = _select0_b;
	pi_inv = _pi_inv;
}

KarpRabinFactorsSuffixes::KarpRabinFactorsSuffixes(unsigned long long _n_factors, 
		vector<unsigned long long> *_arr_kr_s, 
		KarpRabin *_karp_rabin, 
		CompactedText *_compacted_text, 
		bits_s_type::select_1_type *_select1_s, 
		bits_b_type::select_1_type *_select1_b, 
		bits_b_type::select_0_type *_select0_b, 
		int_vector<> *_pi_inv){
	n_factors = _n_factors;
	arr_kr_s = _arr_kr_s;
	delete_krs = false;
	karp_rabin = _karp_rabin;
	if( arr_kr_s->size() < n_factors+1 ){
		cout << "KarpRabinFactorsSuffixes - Warning (insufficient prefixes, must include one for the whole collection)\n";
	}
	ref_text = NULL;
	compacted_text = _compacted_text;
	select1_s = _select1_s;
	select1_b = _select1_b;
	select0_b = _select0_b;
	pi_inv = _pi_inv;
}

KarpRabinFactorsSuffixes::KarpRabinFactorsSuffixes(const string &file, 
		KarpRabin *_karp_rabin, 
		CompactedText *_compacted_text, 
		bits_s_type::select_1_type *_select1_s, 
		bits_b_type::select_1_type *_select1_b, 
		bits_b_type::select_0_type *_select0_b, 
		int_vector<> *_pi_inv){
	// Basic variables loaded from file
	n_factors = 0;
	arr_kr_s = 0;
	delete_krs = false;
	
	load(file);
	// External variables
	karp_rabin = _karp_rabin;
	ref_text = NULL;
	compacted_text = _compacted_text;
	select1_s = _select1_s;
	select1_b = _select1_b;
	select0_b = _select0_b;
	pi_inv = _pi_inv;
}

KarpRabinFactorsSuffixes::~KarpRabinFactorsSuffixes(){
	n_factors = 0;
	if( delete_krs && arr_kr_s != NULL ){
		arr_kr_s->clear();
		delete arr_kr_s;
	}
	arr_kr_s = NULL;
	karp_rabin = NULL;
	ref_text = NULL;
	compacted_text = NULL;
}

// Fast version, only valid for offset and length < karp_rabin->getTableSize()
unsigned long long KarpRabinFactorsSuffixes::hash(unsigned long long factor_ini, unsigned long long length){
//	cout << "KarpRabinFactorsSuffixes::hash - Start (factor_ini: " << factor_ini << ", length: " << length << ", n_factors: " << n_factors << ", n_pi_inv: " << pi_inv->size() << ")\n";
	unsigned long long kr1 = 0;
	
	unsigned long long cur_len = 0;
	unsigned long long factor_cur = 0;
	unsigned long long tu = 0;
	
	// Version con Busqueda Binaria
	unsigned long long l = factor_ini;
	// TODO : verificar n_factors - 1
	unsigned long long h = n_factors;
	unsigned long long factor_cur_bin = factor_ini;
	unsigned long long pu_ini = select1_b->operator()(factor_ini + 1);
	
	unsigned long long cur_perm_bin = (*pi_inv)[factor_cur_bin];
	unsigned long long tu_bin = select1_s->operator()(cur_perm_bin + 1) - cur_perm_bin;
	unsigned long long pu_bin = select1_b->operator()(factor_cur_bin + 1);
	
//	cout << "1\n";
	while(l < h){
		factor_cur_bin = l + ((h-l)>>1);
		pu_bin = select1_b->operator()(factor_cur_bin + 1);
//		cout << "factor_cur_bin: " << factor_cur_bin << ", factor_ini: " << factor_ini << ", len: " << (pu_bin - pu_ini) << "\n";
		if( (pu_bin - pu_ini) < length ){
			l = factor_cur_bin+1;
		}
		else{
			h = factor_cur_bin;
		}
	}
//	cout << "2 (factor_cur_bin: " << factor_cur_bin << ", h: " << h << ")\n";
//	factor_cur_bin = h;
	if( (factor_cur_bin > factor_ini) && (pu_bin - pu_ini) >= length ){
		--factor_cur_bin;
		pu_bin = select1_b->operator()(factor_cur_bin + 1);
	}
//	cout << "3 (factor_cur_bin: " << factor_cur_bin << ")\n";
	unsigned long long cur_len_bin = (pu_bin - pu_ini);
	cur_perm_bin = (*pi_inv)[factor_cur_bin];
	tu_bin = select1_s->operator()(cur_perm_bin + 1) - cur_perm_bin;
	
	cur_len = cur_len_bin;
	factor_cur = factor_cur_bin;
	tu = tu_bin;
	
	if( factor_cur > factor_ini ){
//		cout << "KarpRabinFactorsSuffixes::hash - Preparing kr1 subtract_prefix(" << arr_kr_s->at(factor_cur) << ", " << arr_kr_s->at(factor_ini) << ", " << cur_len << ")\n";
		kr1 = karp_rabin->subtract_prefix(arr_kr_s->at(factor_cur), arr_kr_s->at(factor_ini), cur_len);
	}
//	cout << "KarpRabinFactorsSuffixes::hash - kr1: " << kr1 << ", cur_len: " << cur_len << "\n";
	// Calcular el hash de los (length - cur_len) primeros caracteres del factor actual
	// Notar que lo que sigue se puede extraer er KarpRabinReference (desde tu, de largo length - cur_len)
	
//	cout << "KarpRabinFactorsSuffixes::hash - karp_rabin->hash(ref_text + " << tu << ", " << length << " - " << cur_len << ")\n";
	unsigned long long kr2;
	if( ref_text != NULL ){
		kr2 = karp_rabin->hash(ref_text + tu, length - cur_len);
	}
	else{
		kr2 = karp_rabin->hash(compacted_text, tu, length - cur_len);
	}
	
//	cout << "KarpRabinFactorsSuffixes::hash - kr2: " << kr2 << ", len: " << length - cur_len << "\n";
	unsigned long long kr12 = karp_rabin->concat(kr1, kr2, length - cur_len);
//	cout << "KarpRabinFactorsSuffixes::hash - End (kr12: " << kr12 << ")\n";
	return kr12;
}

// Fast version, only valid for offset and length < karp_rabin->getTableSize()
unsigned long long KarpRabinFactorsSuffixes::hash(unsigned long long factor_ini, unsigned long long offset, unsigned long long length){
//	cout << "KarpRabinFactorsSuffixes::hash - Start (factor_ini: " << factor_ini << ", offset: " << offset << ", length: " << length << ")\n";
	
	if( length == 0 ){
		return 0;
	}
	
	unsigned long long kr_total = hash(factor_ini, offset + length);
//	cout << "KarpRabinFactorsSuffixes::hash - kr_total: " << kr_total << "\n";
	unsigned long long kr_prefix = hash(factor_ini, offset);
//	cout << "KarpRabinFactorsSuffixes::hash - kr_prefix: " << kr_prefix << "\n";
	unsigned long long kr = karp_rabin->subtract_prefix(kr_total, kr_prefix, length);
//	cout << "KarpRabinFactorsSuffixes::hash - kr: " << kr << "\n";
	return kr;
}

void KarpRabinFactorsSuffixes::save(const string &file){
	
	fstream writer(file, fstream::out | fstream::trunc);
	
	writer.write((char*)&n_factors, sizeof(int));
	
	unsigned long long n_arr = arr_kr_s->size();
	writer.write((char*)&n_arr, sizeof(int));
	for(unsigned long long i = 0; i < n_arr; ++i){
		unsigned long long kr = (unsigned long long )(arr_kr_s->at(i));
		writer.write((char*)&kr, sizeof(int));
	}
	
	writer.close();
	
}

void KarpRabinFactorsSuffixes::load(const string &file){

	fstream reader(file, fstream::in);
	
	n_factors = 0;
	reader.read((char*)&n_factors, sizeof(int));
	
	delete_krs = true;
	arr_kr_s = new vector<unsigned long long>();
	
	unsigned long long n_arr = 0;
	reader.read((char*)&n_arr, sizeof(int));
	for(unsigned long long i = 0; i < n_arr; ++i){
		unsigned long long kr = 0;
		reader.read((char*)&kr, sizeof(int));
		arr_kr_s->push_back(kr);
	}
	
	reader.close();
	
}














