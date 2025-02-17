#include "LengthsCoderBlocksBytes.h"

LengthsCoderBlocksBytes::LengthsCoderBlocksBytes(){
	archivo = NULL;
	buff_size = 0;
	buff = NULL;
	byte_ini = 0;
}

LengthsCoderBlocksBytes::LengthsCoderBlocksBytes(const char *bytes){
	archivo = new BytesReader(bytes);
	buff_size = 0;
	buff = NULL;
	byte_ini = 0;
}

LengthsCoderBlocksBytes::~LengthsCoderBlocksBytes(){
	if(archivo != NULL){
		archivo->close();
		delete archivo;
		archivo = NULL;
	}
	deleteBuffer();
}

void LengthsCoderBlocksBytes::deleteBuffer(){
	if(buff != NULL){
		delete [] buff;
		buff = NULL;
		buff_size = 0;
	}
}

void LengthsCoderBlocksBytes::prepareBuffer(unsigned long long new_size){
	if( new_size > buff_size ){
		deleteBuffer();
		buff_size = new_size;
		buff = new unsigned long long[buff_size];
	}
}

void LengthsCoderBlocksBytes::setGolombBase(unsigned long long potencia_base){
	utils.setGolombBase(potencia_base);
}

//Metodo de Lectura
void LengthsCoderBlocksBytes::open(const char *bytes){
//	cout<<"LengthsCoderBlocksBytes::open - inicio\n";
	if(archivo != NULL){
		archivo->close();
		delete archivo;
		archivo = NULL;
	}
	archivo = new BytesReader(bytes);
}

void LengthsCoderBlocksBytes::decodeBlockGolomb(unsigned long long byte_start, unsigned long long n_bytes, unsigned long long n_factores, unsigned long long *arr_len){
	if( archivo == NULL ){
		cerr<<"LengthsCoderBlocksBytes::decodeBlockGolomb - Error, lector NULL\n";
	}
	cout<<"LengthsCoderBlocksBytes::decodeBlockGolomb - Inicio (inicio: "<<byte_start<<", bytes: "<<n_bytes<<", n_factores: "<<n_factores<<")\n";
	unsigned long long max_ints = 1 + (n_bytes >> 2);
	prepareBuffer(max_ints);
//	prepareBuffer(n_factores + 1);
	memset(buff, 0, max_ints * sizeof(int));
	archivo->seekg(byte_start + byte_ini, archivo->beg);
	archivo->read((char*)buff, n_bytes);
	unsigned long long pos_buff = 0;
	for(unsigned long long i = 0; i < n_factores; ++i){
		pos_buff += utils.read_golomb(buff, pos_buff, arr_len[i]);
	}
	cout<<"LengthsCoderBlocksBytes::decodeBlockGolomb - Fin\n";
	
}






