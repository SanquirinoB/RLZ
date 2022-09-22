#ifndef _CODER_BLOCKS_RELZ_H
#define _CODER_BLOCKS_RELZ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <thread>
#include <mutex>

#include "CoderBlocks.h"

#include "BlockHeadersRelz.h"

#include "BitsUtils.h"
#include "LengthsCoderBlocks.h"
#include "PositionsCoderBlocks.h"
#include "ReferenceIndex.h"

using namespace std;

class CoderBlocksRelz : public CoderBlocks{

protected: 
	BitsUtils inner_utils;
	PositionsCoderBlocks inner_pos_coder;
	LengthsCoderBlocks inner_len_coder;
	
	const ReferenceIndex *referencia;
	
public: 
	
	//Notar que todo CoderBlocksRelz usable DEBE construirse con una referencia valida
	CoderBlocksRelz(const ReferenceIndex *referencia = NULL);
	virtual ~CoderBlocksRelz();
	
//	virtual void codeBlock(const char *text, unsigned long long text_size, fstream *file_headers, fstream *file_data, unsigned long long &bytes_headers, unsigned long long &bytes_data, const ReferenceIndex *referencia, char *full_buffer);
	virtual void codeBlock(const char *text, unsigned long long text_size, fstream *file_headers, fstream *file_data, unsigned long long &bytes_headers, unsigned long long &bytes_data, char *full_buffer, vector<pair<unsigned long long, unsigned long long> > *external_factors = NULL);
	
	//Retorna el tamaño en bytes del buffer necesario para codeBlock
	virtual unsigned long long codingBufferSize(unsigned long long block_size);
	
	//Retorna un puntero a un nuevo Coder del mismo tipo
	//Adicionalmente podria copiar otros datos internos (pero el nuevo objeto debe ser independiente)
	//El llamador debe encargarse de borrar la copia
	virtual CoderBlocks *copy() const;
	
};







#endif //_DECODER_BLOCKS_H





