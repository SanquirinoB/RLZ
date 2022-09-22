#ifndef _POSITIONS_CODER_BLOCKS_BYTES_H
#define _POSITIONS_CODER_BLOCKS_BYTES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "BitsUtils.h"
#include "BytesReader.h"

using namespace std;

//Por ahora esta clase incluye TODOS los algoritmos de escritura y lectura de posiciones.
//Notar que seria mejor tener una jerarquia de clases para los diferentes algoritmos.
//Uso este esquema exclusivamente por simpleza.

class PositionsCoderBlocksBytes{

private: 
	BitsUtils utils;
	BytesReader *archivo;
	unsigned long long buff_size;
	unsigned long long *buff;
	unsigned long long byte_ini;
	
	void deleteBuffer();
	void prepareBuffer(unsigned long long new_size);
	
public: 
	
	//Constructor de Lectura
	//Esperara un open para abrir un archivo y esperar lecturas
	PositionsCoderBlocksBytes();
	
	//Constructor de Escritura
	//Crea el archivo (con trunc) para comenzar a escribir
	//Luego de las escrituras debe cerrarse con close
	PositionsCoderBlocksBytes(const char *bytes);

	virtual ~PositionsCoderBlocksBytes();
	
//	//Como el anterior, pero escribe en un archivo dado
//	//Retorna el numero de bytes escritos
//	unsigned long long encodeBlockMaxBits(unsigned long long *arr_pos, unsigned long long n_factores, unsigned char max_bits, fstream *escritor);
//	
//	//Metodo de Escritura
//	//Retorna el numero de bytes escritos
//	unsigned long long encodeBlockVarByte(unsigned long long *arr_pos, unsigned long long n_factores, fstream *escritor);
	
	//Metodo de Lectura
	void open(const char *bytes);
	
	//Metodo de Lectura
	//Lee el archivo (ya abierto) desde byte_start
	//Comienza leyendo max_bits y luego carga n_factores posiciones en arr_pos
	//Asume que arr_pos tiene al menos (n_factores + 1) ints de espacio
	void decodeBlockMaxBits(unsigned long long byte_start, unsigned long long n_bytes, unsigned long long n_factores, unsigned long long *arr_pos);
	
	//Metodo de Lectura
	//Lee el archivo (ya abierto) desde byte_start
	//Carga n_factores posiciones en arr_pos usando read_varbyte
	//Asume que arr_pos tiene al menos (n_factores + 1) ints de espacio
	void decodeBlockVarByte(unsigned long long byte_start, unsigned long long n_bytes, unsigned long long n_factores, unsigned long long *arr_pos);
	
};







#endif //_POSITIONS_CODER_H





