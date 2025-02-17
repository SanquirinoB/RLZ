#include "CoderBlocksRelz.h"

CoderBlocksRelz::CoderBlocksRelz(const ReferenceIndex *_referencia){
	referencia = _referencia;
	if(referencia == NULL){
		cerr<<"CoderBlocksRelz - Advertencia, referencia NULL\n";
	}
}

CoderBlocksRelz::~CoderBlocksRelz(){
}

unsigned long long CoderBlocksRelz::codingBufferSize(unsigned long long block_size){
	return 2 * (block_size + 1) * sizeof(int);
}

void CoderBlocksRelz::codeBlock(const char *text, unsigned long long text_size, fstream *file_headers, fstream *file_data, unsigned long long &bytes_headers, unsigned long long &bytes_data, char *full_buffer, vector<pair<unsigned long long, unsigned long long> > *external_factors){
	
	if( file_headers == NULL || file_data == NULL ){
		cout << "CoderBlocksRelz::codeBlock - Omiting Files\n";
	}
	if( referencia == NULL){
		cerr << "CoderBlocksRelz::codeBlock - Reference not Found\n";
		return;
	}
	if(text == NULL || text_size == 0){
		return;
	}
	bytes_headers = 0;
	bytes_data = 0;
	
	//Buffers locales (a partes del buffer entregado)
	unsigned long long *buff_pos = (unsigned long long *)full_buffer;
	unsigned long long *buff_len = (buff_pos + text_size + 1);
	//Variables para la compresion
	unsigned long long compressed_text = 0;
	unsigned long long pos_prefijo = 0;
	unsigned long long largo_prefijo = 0;
	unsigned long long n_factores = 0;
	unsigned long long max_pos = 0;
	
//	cout<<"CoderBlocksRelz::codeBlock - Text: \""<<string(text, (text_size>10)?10:text_size)<<((text_size>10)?"...":"")<<"\"\n";
	
	while(text_size > 0){
//		cout<<"CoderBlocksRelz::codeBlock - referencia->find...\n";
		referencia->find(text + compressed_text, text_size, pos_prefijo, largo_prefijo);
		// PRUEBA DE LARGO MAXIMO (100)
//		referencia->find(text + compressed_text, (text_size<100)?text_size:100, pos_prefijo, largo_prefijo);
		
		if(largo_prefijo == 0){
			string s(text + compressed_text, 10);
			cout<<"CoderBlocksRelz::codeBlock - Error - Prefijo de largo 0, saliendo (\"" << s << "\")\n";
			return;
		}
		
		//largo_prefijo+= 9; // OJO!! BORRAR
      //if (text_size < largo_prefijo) text_size = 0;		
		//else 
		text_size -= largo_prefijo;
		compressed_text += largo_prefijo;
		if(pos_prefijo > max_pos){
			max_pos = pos_prefijo;
		}
		
    	//cout<<"factor\t"<<n_factores<<"\t"<<pos_prefijo<<"\t"<<largo_prefijo<<"\n";
		
		if( external_factors != NULL ){
			external_factors->push_back( pair<unsigned long long, unsigned long long>(pos_prefijo, largo_prefijo) );
		}
		
//		cout<<"("<<pos_prefijo<<", "<<largo_prefijo<<")\n";
//		cout<<"factor_length\t"<<largo_prefijo<<"\n";
		buff_pos[n_factores] = pos_prefijo;
		buff_len[n_factores] = largo_prefijo;
		++n_factores;
	}//while... procesar factores
  		
	if( file_headers != NULL && file_headers->good() && file_data != NULL && file_data->good() ){
//		cout<<"CoderBlocksRelz::codeBlock - Escribiendo datos\n";
		//Escribir datos
		unsigned long long bytes_pos = inner_pos_coder.encodeBlockMaxBits(buff_pos, n_factores, inner_utils.n_bits(max_pos), file_data);
		unsigned long long bytes_len = inner_len_coder.encodeBlockGolomb(buff_len, n_factores, file_data);
		bytes_data = bytes_pos + bytes_len;
//		cout<<"CoderBlocksRelz::codeBlock - Escribiendo header\n";
		//Escribir header
		BlockHeadersRelz::HeaderRelz header(n_factores, bytes_pos, bytes_len);
		header.save(file_headers);
		bytes_headers = header.size();
	}
	
}

CoderBlocks* CoderBlocksRelz::copy() const{
	CoderBlocks *coder_copy = new CoderBlocksRelz(referencia);
	return (CoderBlocks*)coder_copy;
}











