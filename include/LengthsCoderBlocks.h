#ifndef _LENGTHS_CODER_BLOCKS_H
#define _LENGTHS_CODER_BLOCKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "BitsUtils.h"

using namespace std;

//Por ahora esta clase incluye TODOS los algoritmos de escritura y lectura de posiciones.
//Notar que seria mejor tener una jerarquia de clases para los diferentes algoritmos.
//Uso este esquema exclusivamente por simpleza.

class LengthsCoderBlocks{

private: 
	BitsUtils utils;
	const char *nombre_archivo;
	unsigned long long buff_size;
	unsigned long long *buff;
	unsigned long long byte_ini;
	
	void deleteBuffer();
	void prepareBuffer(unsigned long long new_size);
	
public: 
	
	//Constructor de Lectura
	//Esperara un open para abrir un archivo y esperar lecturas
	LengthsCoderBlocks();
	
	//Constructor de Escritura
	//Crea el archivo (con trunc) para comenzar a escribir
	//Luego de las escrituras debe cerrarse con close
	LengthsCoderBlocks(const char *_nombre_archivo);
	
	virtual ~LengthsCoderBlocks();
	
	//Metodo de Lectura o Escritura
	void setGolombBase(unsigned long long potencia_base);
	
	//Como el anterior, pero escribe en un archivo dado
	//Retorna el numero de bytes escritos
	unsigned long long encodeBlockGolomb(unsigned long long *arr_len, unsigned long long n_factores, fstream *escritor);
	
	//Metodo de Lectura
	void open(const char *_nombre_archivo, unsigned long long _byte_ini = 0);
	
	//Metodo de Lectura
	void decodeBlockGolomb(unsigned long long byte_start, unsigned long long n_bytes, unsigned long long n_factores, unsigned long long *arr_len);
	
};







#endif //_POSITIONS_CODER_H





