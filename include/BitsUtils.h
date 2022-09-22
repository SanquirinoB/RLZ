#ifndef _BITS_UTILS_H
#define _BITS_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

class BitsUtils {

private: 
	
	// Parametros de bit[get/put] para 32 bits (fijos y estaticos) 
	static const unsigned long long WORD_SIZE = 32;
	static const unsigned long long BITS_WORD = 5;
	static const unsigned long long MASK_WORD = 0x1f;
	
	// Parametros para golomb (dependen de cada instancia)
	unsigned long long GOLOMB_BASE;
	unsigned long long GOLOMB_BITS_BASE;
	unsigned long long GOLOMB_MASK_BASE;
	
public: 
	
	BitsUtils();
	
	~BitsUtils();
	
	// Cambia la base de golomb a 2 ^ potencia_base
	// potencia_base debe ser entre [2, 32]
	void setGolombBase(unsigned long long base_power);
	
	// Numero de bits (real) para escribir un numero
	unsigned long long n_bits(unsigned long long num);

	// Escribe los "len_num" bits de "num" en la "out", desde su posicion "pos_out" (en bits)
	// Notar que luego de cada escritura, el llamador debe sumar los "len_num" bits escritos a su "pos_out" local
	// - "out" es el puntero a la salida para escribir
	// - "pos_out" es la posicion en bits en la salida para escribir
	// - "len_num" es el largo de la escritura en bits
	// - "num" es el numero que se desea escribir
	void bitput(unsigned long long *out, unsigned long long pos_out, unsigned long long len_num, unsigned long long num);

	// Retorna el numero de "len_num" bits desde "pos" de la "in"
	unsigned long long bitget(unsigned long long *in, unsigned long long pos, unsigned long long len_num);
	
	// Retorna el numero de bits a ser usados para una escritura
	unsigned long long bits_golomb(unsigned long long num);

	// Escribe el numero "num" en la posicion "pos_write" (en bits) de la "out"
	// Retorna el numero de bits usados (para ajustar pos_write)
	unsigned long long write_golomb(unsigned long long *out, unsigned long long pos_write, unsigned long long num);

	// Lee el proximo numero a partir de la posicion "pos_read" de la "in"
	// Guarda el numero leido en "num" y retorna el numero de bits leidos
	unsigned long long read_golomb(unsigned long long *in, unsigned long long pos_read, unsigned long long &num);

	unsigned long long read_gamma(unsigned long long *in, unsigned long long pos_read, unsigned long long &num);

	unsigned long long write_gamma(unsigned long long *out, unsigned long long pos_write, unsigned long long num);
	
	// Escribe el numero en varbyte al inicio del buffer y retorna el numero de bytes usados
	// Notar que el llamador debe mover el buffer entre escrituras
	unsigned long long write_varbyte(unsigned char *buff, unsigned long long num);
	
	// Lee el primer numero en varbyte (y lo almacena en num) retornando el numero de bytes leidos
	// Notar que el llamador debe mover el buffer entre lecturas
	// Esta version trunca el numero leido a 32 bits (pero retorna en numero real de bytes leidos)
	unsigned long long read_varbyte(unsigned char *buff, unsigned long long &num);
	
	// Retorna el numero exacto de bytes que serian usados al escribir este numero con write_varbyte
	// Este metodo se puede usar para preparar un buffer del tamaño adecuado
	unsigned long long size_varbyte(unsigned long long num);
};








#endif //_BITS_UTILS_H

