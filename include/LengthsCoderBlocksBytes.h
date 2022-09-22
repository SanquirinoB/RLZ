#ifndef _LENGTHS_CODER_BLOCKS_BYTES_H
#define _LENGTHS_CODER_BLOCKS_BYTES_H

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

//CONCURRENCIA
//Varios threads deberian tener su propio objeto y ser capaces de leer todos el mismo archivo.
//Las lecturas deberian ser independientes unas de otras y en cualquier posicion.
//Ojo con variables estaticas o funciones globales.

class LengthsCoderBlocksBytes{

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
	LengthsCoderBlocksBytes();
	
	//Constructor de Escritura
	//Crea el archivo (con trunc) para comenzar a escribir
	//Luego de las escrituras debe cerrarse con close
	LengthsCoderBlocksBytes(const char *bytes);
	
	virtual ~LengthsCoderBlocksBytes();
	
	//Metodo de Lectura o Escritura
	void setGolombBase(unsigned long long potencia_base);
	
//	//Metodo de Escritura
//	//Retorna el numero de bytes escritos o la posicion en que queda el escritor
//	unsigned long long encodeBlockGolomb(unsigned long long *arr_len, unsigned long long n_factores);
	
	//Metodo de Lectura
	void open(const char *bytes);
	
	//Metodo de Lectura
	void decodeBlockGolomb(unsigned long long byte_start, unsigned long long n_bytes, unsigned long long n_factores, unsigned long long *arr_len);
	
};







#endif //_POSITIONS_CODER_H





