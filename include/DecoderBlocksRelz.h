#ifndef _DECODER_BLOCKS_RELZ_H
#define _DECODER_BLOCKS_RELZ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DecoderBlocks.h"

#include "BlockHeadersRelz.h"
#include "LengthsCoderBlocks.h"
#include "PositionsCoderBlocks.h"

#include "DecoderBlocksRelzBytes.h"

using namespace std;

class DecoderBlocksRelz : public DecoderBlocks {

protected:

	//Este es solo un puntero al texto de referencia, NO una copia
	const char *texto_ref;
	//Puntero al nombre del archivo (solo puntero)
	const char *master_file;

	LengthsCoderBlocks *len_coder;
	PositionsCoderBlocks *pos_coder;
	
	//Buffers para almacenar el bloque actual descomprimido
	unsigned long long buff_size;
	unsigned long long *buff_pos;
	unsigned long long *buff_len;
	unsigned long long cur_block;
	unsigned long long cur_block_factores;
	
	//Borra headers, coders y resetea cur_block
	//Conserva los buffers
	void deleteData();
	
	//Borra los buffers
	void deleteBuffers();
	
	//Resetea y extiende los buffers solo si new_size es mayor a buff_size
	void prepareBuffer(unsigned long long new_size);
	
public: 
	//Notar que todo DecoderBlocksRelz usable DEBE construirse con un texto de referencia valido
	DecoderBlocksRelz(const char *_texto_ref = NULL);
	DecoderBlocksRelz(const char *_master_file, const char *_texto_ref);
	virtual ~DecoderBlocksRelz();
	
	virtual void load(const char *_master_file);
	
	virtual unsigned long long decodeBlock(unsigned long long block_pos, char *buff);
	
	//Este metodo retorna un NUEVO objeto headers del tipo correcto
	//Usa los argumentos en la creacion del header
	virtual BlockHeaders *getNewHeaders(unsigned long long _text_size, unsigned long long _block_size, Metadata *_metadata = NULL);
	
	//Retorna un puntero a un nuevo Decoder del mismo tipo
	//Adicionalmente podria copiar otros datos internos (pero el nuevo objeto debe ser independiente)
	//El llamador debe encargarse de borrar la copia
	virtual DecoderBlocks *copy() const;
	
	virtual DecoderBlocks *copyBytes(const char *bytes) const;
	
	
	// Estos metodos son solo para acceder a los buffers para tomar estadisticos
	
	unsigned long long *getBuffPos(){
		return buff_pos; 
	}
	unsigned long long *getBuffLen(){
		return buff_len; 
	}
	unsigned long long getNumFactors(){
		return cur_block_factores; 
	}
	
	
};







#endif //_DECODER_BLOCKS_H





