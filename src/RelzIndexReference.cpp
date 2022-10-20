#include "RelzIndexReference.h"

RelzIndexReference::RelzIndexReference(){
	len_text = 0;
	ref_text = NULL;
	n_factors = 0;
}

RelzIndexReference::RelzIndexReference(vector<pair<unsigned long long, unsigned long long> > &factors, char *full_text, unsigned long long _len_text, const char *_ref_text, unsigned long long _len_ref){
	
	len_text = _len_text;
	n_factors = factors.size();
	
	ref_text = new CompactedText(_ref_text, _len_ref);
	
	NanoTimer timer;
	
	cout << "RelzIndexReference - Inicio (factors: " << factors.size() << ", len_text: " << len_text << ", len_ref: " << ref_text->length() << ")" << endl;
	
	cout << "RelzIndexReference - Preparing Factors" << endl;
	// Factores en version ini, fin (absoluto) y ordenados por ini
	vector<pair<unsigned long long, pair<unsigned long long, unsigned long long> > > factors_sort;
	unsigned long long cur_pos = 0;
	for( pair<unsigned long long, unsigned long long> factor : factors ){
//		cout << "(" << factor.first << ", " << factor.second << ", " << cur_pos << ") - cur_start: " << cur_start << endl;
		factors_sort.push_back( 
			pair<unsigned long long, pair<unsigned long long, unsigned long long> >(
				factor.first, pair<unsigned long long, unsigned long long>(factor.first + factor.second - 1ULL, cur_pos++)
				)
			);
	}
	sort(factors_sort.begin(), factors_sort.end());
	cout << "RelzIndexReference - Factors Sorted prepared in " << timer.getMilisec() << endl;
	timer.reset();

	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;
//	cout << "Factors Sorted: " << endl;
//	for( pair<unsigned long long, pair<unsigned long long, unsigned long long> > factor : factors_sort ){
//		cout << "(" << factor.first << ", " << factor.second.first << ", " << factor.second.second << ")" << endl;
//	}
	
	// Bit vector S
	cout << "RelzIndexReference - Preparing Vector S" << endl;
	bit_vector arr_s(ref_text->length() + n_factors, 0);
	unsigned cur_ref = 0;
	cur_pos = 0;
	for( unsigned long long i = 0; i < n_factors; ++i ){
		unsigned long long ini = factors_sort[i].first;
		if( ini == cur_ref ){
			arr_s[cur_pos++] = 1;
		}
		else{
			arr_s[cur_pos++] = 0;
			++cur_ref;
			--i;
		}
	}
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;

	cout << "RelzIndexReference - Vector S prepared in " << timer.getMilisec() << endl;
	timer.reset();
	
	bits_s = bits_s_type(arr_s);
	select1_s = bits_s_type::select_1_type(&bits_s);
	select0_s = bits_s_type::select_0_type(&bits_s);
	
	// Permutacion 
	cout << "RelzIndexReference - Preparing Permutation PI" << endl;
	pi = int_vector<>(n_factors);
	pi_inv = int_vector<>(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		pi[i] = factors_sort[i].second.second;
		pi_inv[ factors_sort[i].second.second ] = i;
	}
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;
	
	cout << "RelzIndexReference - PI prepared in " << timer.getMilisec() << endl;
	timer.reset();
	
	// Posiciones finales Ez
	int_vector<> ez = int_vector<>(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		ez[i] = factors_sort[i].second.first;
	}
	rmq = rmq_type(&ez);
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;
	
	// Bit vector B (inicio de las frases en texto)
	cout << "RelzIndexReference - Preparing Vector B" << endl;
	bit_vector arr_b(len_text, 0);
	unsigned long long pos_text = 0;
	for( unsigned long long i = 0; i < n_factors; ++i ){
		unsigned long long len = factors[i].second;
		arr_b[ pos_text ] = 1;
		pos_text += len;
	}
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;
	cout << "RelzIndexReference - Vector B prepared in " << timer.getMilisec() << endl;
	timer.reset();

	bits_b = bits_b_type(arr_b);
	select1_b = bits_b_type::select_1_type(&bits_b);
	select0_b = bits_b_type::select_0_type(&bits_b);
	
	// Construccion del FM Index (aunque use el SA original para la compresion, esto es para la busqueda)
	// Construccion con datos en memoria, en un string
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;

	cout << "RelzIndexReference - Preparing fm_index\n";
	construct_im(fm_index, _ref_text, 1);
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;

	cout << "RelzIndexReference - fm_index prepared in " << timer.getMilisec() << "\n";
	timer.reset();
	
	// Preparacion de permutaciones X e Y
	cout << "RelzIndexReference - Preparing arr X" << endl;
	
	vector<unsigned long long> arr_x_original(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		arr_x_original[i] = i;
	}

	CompactedFactorsFastIteratorReverseComparator comp_rev(factors, (const char*) _ref_text);

	stable_sort(arr_x_original.begin(), arr_x_original.end(), comp_rev);
	// cout << "[DEBUG] Sort X finished" << endl;
	
	arr_x = int_vector<>(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		arr_x[i] = arr_x_original[i];
	}
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;

	
//	for( unsigned long long i = 0; i < n_factors; ++i ){
//		cout << " arr_x[" << i << "]: " << arr_x[i] << " -> ";
//		char c = 0;
//		FactorsIteratorReverse it(arr_x[i] - 1, n_factors, &select1_s, &select1_b, &select0_b, &pi_inv, _ref_text, &fm_index, len_text);
//		for(unsigned long long k = 0; k < 20 && it.hasNext() && (c = it.next()) != 0; ++k ) 
//			cout << c;
//		cout << endl;
//	}
//	cout << "-----" << endl;
	
	cout << "RelzIndexReference - Preparing arr Y" << endl;
	
	vector<unsigned long long> arr_y_original(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		arr_y_original[i] = i;
	}

	CompactedFactorsFastIteratorComparator comp(factors, (const char*) _ref_text);

	stable_sort(arr_y_original.begin(), arr_y_original.end(), comp);
	// cout << "[DEBUG] Sort X finished" << endl;
	
	arr_y = int_vector<>(n_factors);
	int_vector<> arr_y_inv(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		arr_y[i] = arr_y_original[i];
		arr_y_inv[ arr_y_original[i] ] = i;
	}
	
//	for( unsigned long long i = 0; i < n_factors; ++i ){
//		cout << " arr_y[" << i << "]: " << arr_y[i] << " -> ";
//		char c = 0;
//		FactorsIterator it(arr_y[i], n_factors, &select1_s, &select1_b, &select0_b, &pi_inv, _ref_text, &fm_index, len_text);
//		for(unsigned long long k = 0; k < 20 && it.hasNext() && (c = it.next()) != 0; ++k ) 
//			cout << c;
//		cout << " (" << it.length() << ")" << endl;
//	}
//	cout << "-----" << endl;
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;

	
	cout << "RelzIndexReference - X & Y prepared in " << timer.getMilisec() << endl;
	timer.reset();
	
	cout << "RelzIndexReference - Preparing WT" << endl;
	int_vector<> values_wt(n_factors);
	for( unsigned long long i = 0; i < n_factors; ++i ){
		values_wt[i] = arr_y_inv[ arr_x[ i ] ];
	}
	construct_im(wt, values_wt);
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;

	cout << "RelzIndexReference - WT prepared in " << timer.getMilisec() << endl;
	timer.reset();
	
	// Prueba de aceleracion de recursive_rmq almacenando los datos de los factores descomprimidos
	if(precompute_rmq){
		for( unsigned long long i = 0; i < n_factors; ++i ){
			unsigned long long tu = select1_s(i + 1) - i;
			unsigned long long pu = select1_b(pi[i] + 1);
			unsigned long long lu = select1_b(pi[i] + 2) - pu;
			arr_tu.push_back(tu);
			arr_pu.push_back(pu);
			arr_lu.push_back(lu);
		}
	}
	
	// Compactacion de arreglos descomprimidos
	sdsl::util::bit_compress(pi);
	sdsl::util::bit_compress(pi_inv);
	sdsl::util::bit_compress(arr_x);
	sdsl::util::bit_compress(arr_y);
	// cout << "[DEBUG] Constructor: len = " << ref_text->length() << endl;
	
	cout << "RelzIndexReference - End" << endl;
	
}

RelzIndexReference::~RelzIndexReference(){
	if( ref_text != NULL ){
		delete ref_text;
		ref_text = NULL;
	}
}

unsigned long long RelzIndexReference::RefLen()
{
	return ref_text->length();
}

void RelzIndexReference::findTimes(const string &pattern, vector<unsigned long long> &results){

//	cout << "RelzIndexReference::findTimes - Start (\"" << pattern << "\")" << endl;
	NanoTimer timer;
	
//	cout << "RelzIndexReference::findTimes - Section A, reference" << endl;
	
	size_t m = pattern.size();
	size_t occs = sdsl::count(fm_index, pattern.begin(), pattern.end());
	occs_a += occs;
	vector<int_vector<64>> arr_locations;
	if( occs > 0 ){
		arr_locations.push_back(locate(fm_index, pattern.begin(), pattern.begin()+m));
		sort(arr_locations.back().begin(), arr_locations.back().end());
	}
	querytime_p1 += timer.getNanosec();
	timer.reset();
	
	for( int_vector<64> locations : arr_locations ){
		for( unsigned long long i = 0; i < occs; ++i ){
			unsigned long long occ_i = locations[i];
			// Comprobar los factores que cuben esta ocurrencia (el string ref[occ_i, occ_i + m - 1])
			unsigned long long select = select0_s(occ_i + 1);
			unsigned long long pos_ez = select - 1 - occ_i;
			// Now the recursive search in rmq (0 - pos_ez)
			if( occ_i >= select ){
				continue;
			}
			recursive_rmq(0, pos_ez, (occ_i + m), occ_i, results);
		}
	}
	querytime_p2 += timer.getNanosec();
	
//	cout << "RelzIndexReference::findTimes - Section B, ranges" << endl;
	for(unsigned long long i = 1; i < pattern.length(); ++i){
		timer.reset();
		string p1 = pattern.substr(0, i);
		string p1_rev = "";
		for( unsigned long long k = 0; k < p1.length(); ++k ){
			p1_rev += p1[ p1.length() - 1 - k ];
		}
		string p2 = pattern.substr(i, pattern.length() - i);
		
		// Rango X
		pair<unsigned long long, unsigned long long> r1 = getRangeX(p1_rev.c_str());
		querytime_p3 += timer.getNanosec();
		timer.reset();
		
		if( r1.first == (unsigned long long)(-1) || r1.second == (unsigned long long)(-1) || r1.second < r1.first ){
//			cout << "RelzIndexReference::findTimes - getRangeX invalido, omitiendo" << endl;
			continue;
		}
		
		// Rango Y
		pair<unsigned long long, unsigned long long> r2 = getRangeY(p2.c_str());
		querytime_p3 += timer.getNanosec();
		timer.reset();
		
		if( r2.first == (unsigned long long)(-1) || r2.second == (unsigned long long)(-1) || r2.second < r2.first ){
//			cout << "RelzIndexReference::findTimes - getRangeY invalido, omitiendo" << endl;
			continue;
		}
		
//		cout << "RelzIndexReference::findTimes - Searching in [" << r1.first << ", " << r1.second << "] x [" << r2.first << ", " << r2.second << "]:" << endl;
		auto res = wt.range_search_2d(r1.first, r1.second, r2.first, r2.second);
		for (auto point : res.second){
			unsigned long long f = arr_y[point.second];
			unsigned long long pu = select1_b(f + 1);
//			cout << "RelzIndexReference::findTimes - Adding pos " << (pu - p1.length()) << endl;
			results.push_back(pu - p1.length());
			++occs_c;
		}
		
		querytime_p4 += timer.getNanosec();
	}
//	cout << "RelzIndexReference::findTimes - End" << endl;
	
}

void RelzIndexReference::recursive_rmq(unsigned long long ini, unsigned long long fin, unsigned long long min_pos, unsigned long long occ_ref, vector<unsigned long long> &results){
//	cout << "RelzIndexReference::recursive_rmq - " << ini << ", " << fin << endl;
	
	unsigned long long pos_max = rmq(ini, fin);
	
//	cout << "RelzIndexReference::recursive_rmq - Computing factor" << endl;
	assert(pos_max < n_factors);
	unsigned long long tu = 0;
	unsigned long long pu = 0;
	unsigned long long lu = 0;
	if( precompute_rmq ){
		tu = arr_tu[pos_max];
		pu = arr_pu[pos_max];
		lu = arr_lu[pos_max];
	}
	else{
		tu = select1_s(pos_max + 1) - pos_max;
		pu = select1_b(pi[pos_max] + 1);
		// cout << "pi(t) = " << pi[pos_max] << endl;

		if(pi[pos_max] + 1 < n_factors)
		{
			lu = select1_b(pi[pos_max] + 2) - pu;
		} 
		else 
		{
			lu = bits_b.size() - pu;
		}
	}

	// cout << "tu = " << tu << ", pu = " << pu << ", lu = " << lu  << ", min_pos = " << min_pos << endl;
	
//	cout << "RelzIndexReference::recursive_rmq - tu: " << tu << ", pu: " << pu << ", lu: " << lu << endl;
	
	if( tu + lu < min_pos ){
		return;
	}
	else{
//		cout << " -> Adding " << (pu + (occ_ref - tu)) << endl;
		results.push_back(pu + (occ_ref - tu));
		++occs_b;
	}
	
	if( (pos_max > 0) && (ini < pos_max) ){
		recursive_rmq(ini, pos_max-1, min_pos, occ_ref, results);
	}
	if( pos_max < fin ){
		recursive_rmq(pos_max+1, fin, min_pos, occ_ref, results);
	}
}

template <typename ItereatorType>
bool RelzIndexReference::factorLess(unsigned long long factor, const char *pattern, unsigned long long len, bool equal){
	if( factor == (unsigned long long)(-1) ){
		return true;
	}
//	ItereatorType it(factor, n_factors, &select1_s, &select1_b, &select0_b, &pi_inv, ref_text, &fm_index, len_text);
	ItereatorType it(factor, n_factors, &select1_s, &select1_b, &select0_b, &pi_inv, ref_text, len_text);
	
	if( it.length() == 0 ){
		return true;
	}
	it.setMaxLength(len);
	
	char c1 = it.next();
	unsigned long long pos = 0;
	char c2 = pattern[pos++];
	bool next = true;
//	cout << "RelzIndexReference::factorLess - " << c1 << " vs " << c2 << endl;
	while( next && (c1 == c2) ){
		if( it.hasNext() ){
			c1 = it.next();
		}
		else{
			c1 = 0;
			next = false;
		}
		if(pos < len){
			c2 = pattern[pos++];
		}
		else{
			c2 = 0;
			next = false;
		}
//		cout << "RelzIndexReference::factorLess - " << c1 << " vs " << c2 << endl;
	}
	if( equal && (c2 == 0) ){
//		cout << "RelzIndexReference::factorLess - res " << (c1 <= c2) << endl;
		return true;
	}
	else{
//		cout << "RelzIndexReference::factorLess - res " << (c1 < c2) << endl;
		return (c1 < c2);
	}
}

// Notar que, a diferencia de la busqueda en referencia, esta debe ser completa
// Es decir, solo importa el rango que contiene al patron completo
pair<unsigned long long, unsigned long long> RelzIndexReference::getRangeY(const char *pattern){
	
	// Version de Revision completa
	
	unsigned long long izq = -1;
	unsigned long long der = -1;
	unsigned long long pat_len = strlen(pattern);
	unsigned long long l, h, m, fm;
	
	// Busqueda binaria del lado izquierdo
//	cout << "getRangeY - Busqueda Izquierda" << endl;
	l = 0;
	h = n_factors-1;
//	cout << "getRangeY - l: " << l << ", h: " << h << endl;
	while(l < h){
		m = l + ((h-l)>>1);
		fm = arr_y[m];
//		cout << "getRangeY - FactorRev " << fm << " < pattern?" << endl;
		if( factorLess<FactorsIteratorCompacted>(fm, pattern, pat_len) ){
//			cout << "getRangeY - caso 1: l = " << (m+1) << endl;
			l = m+1;
		}
		else{
//			cout << "getRangeY - caso 2: h = " << m << endl;
			h = m;
		}
	}
//	cout << "getRangeY - end h: " << h << endl;
	izq = h;
	fm = arr_y[izq];
	if( factorLess<FactorsIteratorCompacted>(fm, pattern, pat_len) ){
		++izq;
	}
//	cout << "getRangeY - izq: " << izq << endl;
	
	// Busqueda binaria del lado derecho
//	cout << "getRangeY - Busqueda derecha" << endl;
	l = izq;
	h = n_factors-1;
//	cout << "getRangeY - l: " << l << ", h: " << h << endl;
	while(l < h){
		m = l + ((h-l)>>1);
		fm = arr_y[m];
//		cout << "getRangeY - FactorRev " << fm << " < pattern?" << endl;
		if( factorLess<FactorsIteratorCompacted>(fm, pattern, pat_len, true) ){
//			cout << "getRangeY - caso 1: l = " << (m+1) << endl;
			l = m+1;
		}
		else{
//			cout << "getRangeY - caso 2: h = " << m << endl;
			h = m;
		}
	}
//	cout << "getRangeY - end h: " << h << endl;
	der = h;
	fm = arr_y[der];
	if( !factorLess<FactorsIteratorCompacted>(fm, pattern, pat_len, true) ){
		--der;
	}
//	cout << "getRangeY - der: " << der << endl;
	
//	cout << "getRangeY - result: (" << izq << ", " << der << ")" << endl;
	return pair<unsigned long long, unsigned long long>(izq, der);
}

#define LEVEL_BINARY_SEARCH 20

pair<unsigned long long, unsigned long long> RelzIndexReference::getRangeX(const char *pattern){
	
	// Version de Revision completa
	
	unsigned long long izq = -1;
	unsigned long long der = -1;
	unsigned long long pat_len = strlen(pattern);
	unsigned long long l, h, m, fm, level;
	
	// Busqueda binaria del lado izquierdo
//	cout << "getRangeX - Busqueda Izquierda" << endl;
	l = 0;
	h = n_factors-1;
//	cout << "getRangeX - l: " << l << ", h: " << h << endl;
        level = 0;
	while(l < h){
		m = l + ((h-l)>>1);
		if (level < LEVEL_BINARY_SEARCH) { 
                    fm = arr_x[m]; level++;
                } else
		    fm = arr_y[wt[m]];
//		cout << "getRangeX - FactorRev " << fm << " < pattern?" << endl;
		if( factorLess<FactorsIteratorCompactedReverse>(fm-1, pattern, pat_len) ){
//			cout << "getRangeX - caso 1: l = " << (m+1) << endl;
			l = m+1;
		}
		else{
//			cout << "getRangeX - caso 2: h = " << m << endl;
			h = m;
		}
	}
//	cout << "getRangeX - end h: " << h << endl;
	izq = h;
        if (level < LEVEL_BINARY_SEARCH) fm = arr_x[izq];	
	else fm = arr_y[wt[izq]];
	if( factorLess<FactorsIteratorCompactedReverse>(fm-1, pattern, pat_len) ){
		++izq;
	}
//	cout << "getRangeX - izq: " << izq << endl;
	
	// Busqueda binaria del lado derecho
//	cout << "getRangeX - Busqueda derecha" << endl;
	l = izq;
	h = n_factors-1;
//	cout << "getRangeX - l: " << l << ", h: " << h << endl;
        level = 0;
	while(l < h){
		m = l + ((h-l)>>1);
		if (level < LEVEL_BINARY_SEARCH) {
		    fm = arr_x[m];level++;
		} else
		    fm = arr_y[wt[m]];
//		cout << "getRangeX - FactorRev " << fm << " < pattern?" << endl;
		if( factorLess<FactorsIteratorCompactedReverse>(fm-1, pattern, pat_len, true) ){
//			cout << "getRangeX - caso 1: l = " << (m+1) << endl;
			l = m+1;
		}
		else{
//			cout << "getRangeX - caso 2: h = " << m << endl;
			h = m;
		}
	}
//	cout << "getRangeX - end h: " << h << endl;
	der = h;
	if (level < LEVEL_BINARY_SEARCH) fm = arr_x[der];
	else fm = arr_y[wt[der]];
	if( (der > 0) && !factorLess<FactorsIteratorCompactedReverse>(fm-1, pattern, pat_len, true) ){
		--der;
	}
//	cout << "getRangeX - der: " << der << endl;
	
//	cout << "getRangeX - result: (" << izq << ", " << der << ")" << endl;
	return pair<unsigned long long, unsigned long long>(izq, der);
}

double RelzIndexReference::getSize(){
	double total_bytes = 0;
	
	// texto descomprimido
	unsigned long long len_ref = ref_text->length();
	total_bytes += len_ref/4;
	cout << "RelzIndexReference::getSizeInMB - Reference Text: " << (2.0*len_ref/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(fm_index);
	cout << "RelzIndexReference::getSizeInMB - fm_index: " << (8.0*size_in_bytes(fm_index)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(rmq);
	cout << "RelzIndexReference::getSizeInMB - rmq: " << (8.0*size_in_bytes(rmq)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(bits_s);
	cout << "RelzIndexReference::getSizeInMB - bits_s: " << (8.0*size_in_bytes(bits_s)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(bits_b);
	cout << "RelzIndexReference::getSizeInMB - bits_b: " << (8.0*size_in_bytes(bits_b)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(pi);
	cout << "RelzIndexReference::getSizeInMB - pi: " << (8.0*size_in_bytes(pi)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(pi_inv);
	cout << "RelzIndexReference::getSizeInMB - pi_inv: " << (8.0*size_in_bytes(pi_inv)/len_text) << " bps" << endl;
	
	//total_bytes += size_in_bytes(arr_x);
	//cout << "RelzIndexReference::getSizeInMB - arr_x: " << (8.0*size_in_bytes(arr_x)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(arr_y);
	cout << "RelzIndexReference::getSizeInMB - arr_y: " << (8.0*size_in_bytes(arr_y)/len_text) << " bps" << endl;
	
	total_bytes += size_in_bytes(wt);
	cout << "RelzIndexReference::getSizeInMB - wt: " << (8.0*size_in_bytes(wt)/len_text) << " bps" << endl;
	
	cout << "RelzIndexReference::getSizeInMB - Total " << total_bytes/(1024*1024) << " MB (" << (8.0*total_bytes/len_text) << " bps)" << endl;
	
	return total_bytes;
}

void RelzIndexReference::save(const string &file_base){
	cout << "RelzIndexReference::save - Start (base \"" << file_base << "\")" << endl;
	
	// Base
	string index_basic_file = file_base + ".base";
	fstream writer(index_basic_file, fstream::out | fstream::trunc);
	// Version of the index
	unsigned char version = 2;
	writer.write((char*)&version, 1);
	// len_text
	writer.write((char*)&len_text, sizeof(int));
	// n_factors
	writer.write((char*)&n_factors, sizeof(int));
	// ref_text
	ref_text->save(&writer);
	
	// Close Base
	writer.close();
	
	// fm_index
	string fm_index_file = file_base + ".fm";
	store_to_file(fm_index, fm_index_file);
	
	// rmq
	string rmq_file = file_base + ".rmq";
	store_to_file(rmq, rmq_file);
	
	// bits_s
	string bits_s_file = file_base + ".arrs";
	store_to_file(bits_s, bits_s_file);
	
	// bits_b
	string bits_b_file = file_base + ".arrb";
	store_to_file(bits_b, bits_b_file);
	
	// pi
	string pi_file = file_base + ".pi";
	store_to_file(pi, pi_file);
	
	// pi_inv
	string pi_inv_file = file_base + ".pi_inv";
	store_to_file(pi_inv, pi_inv_file);
	
	// arr_x
	string x_file = file_base + ".x";
	store_to_file(arr_x, x_file);
	
	// arr_y
	string y_file = file_base + ".y";
	store_to_file(arr_y, y_file);
	
	// wt
	string wt_file = file_base + ".wt";
	store_to_file(wt, wt_file);
	
	cout << "RelzIndexReference::save - End" << endl;
}

void RelzIndexReference::load(const string &file_base){
	cout << "RelzIndexReference::load - Start (base \"" << file_base << "\")" << endl;
	
	// Base
	string index_basic_file = file_base + ".base";
	fstream reader(index_basic_file, fstream::in);
	// Version of the index
	unsigned char version = 0;
	reader.read((char*)&version, 1);
	if( version != 2 ){
		cout << "RelzIndexReference::load - Wrong Version" << endl;
		return;
	}
	// len_text
	reader.read((char*)&len_text, sizeof(int));
	// n_factors
	reader.read((char*)&n_factors, sizeof(int));
	// ref_text
	ref_text = new CompactedText();
	ref_text->load(&reader);
	
	// Close Base
	reader.close();
	
	// fm_index
	cout << "RelzIndexReference::load - fm_index" << endl;
	string fm_index_file = file_base + ".fm";
	load_from_file(fm_index, fm_index_file);
	
	// rmq
	cout << "RelzIndexReference::load - rmq" << endl;
	string rmq_file = file_base + ".rmq";
	load_from_file(rmq, rmq_file);
	
	// bits_s
	cout << "RelzIndexReference::load - bits_s" << endl;
	string bits_s_file = file_base + ".arrs";
	load_from_file(bits_s, bits_s_file);
	select1_s = bits_s_type::select_1_type(&bits_s);
	select0_s = bits_s_type::select_0_type(&bits_s);
	
	// bits_b
	cout << "RelzIndexReference::load - bits_b" << endl;
	string bits_b_file = file_base + ".arrb";
	load_from_file(bits_b, bits_b_file);
	select1_b = bits_b_type::select_1_type(&bits_b);
	select0_b = bits_b_type::select_0_type(&bits_b);
	
	// pi
	string pi_file = file_base + ".pi";
	load_from_file(pi, pi_file);
	
	// pi_inv
	string pi_inv_file = file_base + ".pi_inv";
	load_from_file(pi_inv, pi_inv_file);
	
	// arr_x
	string x_file = file_base + ".x";
	load_from_file(arr_x, x_file);
	
	// arr_y
	string y_file = file_base + ".y";
	load_from_file(arr_y, y_file);
	
	// wt
	cout << "RelzIndexReference::load - wt" << endl;
	string wt_file = file_base + ".wt";
	load_from_file(wt, wt_file);
	
	if(precompute_rmq){
		for( unsigned long long i = 0; i < n_factors; ++i ){
			unsigned long long tu = select1_s(i + 1) - i;
			unsigned long long pu = select1_b(pi[i] + 1);
			unsigned long long lu = select1_b(pi[i] + 2) - pu;
			arr_tu.push_back(tu);
			arr_pu.push_back(pu);
			arr_lu.push_back(lu);
		}
	}
	
	cout << "RelzIndexReference::load - End" << endl;
	
}




















