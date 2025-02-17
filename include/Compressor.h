#ifndef _COMPRESSOR_H
#define _COMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <thread>
#include <mutex>
#include <vector>

#include "BitsUtils.h"
#include "NanoTimer.h"

#include "CoderBlocks.h"
#include "DecoderBlocks.h"
#include "BlockHeaders.h"
#include "BlockHeadersFactory.h"
#include "Metadata.h"

#include "TextFilter.h"
#include "TextFilterBasic.h"

using namespace std;

class Compressor{

protected: 
	
	char *master_file;
	CoderBlocks *coder;
	DecoderBlocks *decoder;
	mutex mutex_interno;
	TextFilter *filter;
	
	static const bool adjust_text = true;
	
	// Compresion efectiva, separada para simplificar ligeramente el codigo
	// Retorna true en exito, false en caso de fallo
	bool realCompress(const char *in_file, unsigned long long n_threads = 4, unsigned long long block_size = 1000000, bool use_metadata = true, vector<pair<unsigned long long, unsigned long long> > *external_factors = NULL);
	
public: 

	class ThreadCompressData{
	public:
	
		ThreadCompressData(){
			id = 0;
			file_headers[0] = 0;
			file_data[0] = 0;
			n_blocks = 0;
			block_size = 0;
			coder = NULL;
			shared_pos = NULL;
			shared_mutex = NULL;
			lista_textos = NULL;
			vector_bytes_headers = NULL;
			vector_bytes_data = NULL;
			vector_thread_block = NULL;
		}
		
		~ThreadCompressData(){
		}
		
		//Datos del thread
		unsigned long long id;
		//Ojo con el largo del nombre de archivo
		char file_headers[128];
		char file_data[128];
		
		//Datos comunes (constantes)
		unsigned long long n_blocks;
		unsigned long long block_size;
		
		//Datos compartidos (punteros)
		const CoderBlocks *coder;
		unsigned long long *shared_pos;
		mutex *shared_mutex;
		vector< pair<char*, unsigned long long> > *lista_textos;
		vector<unsigned long long> *vector_bytes_headers;
		vector<unsigned long long> *vector_bytes_data;
		vector<unsigned long long> *vector_thread_block;
		
	};

	// Constructor vacio
	Compressor();
	
	// Constructor principal de la clase Compressor
	// _master_file: ruta asociada al archivo (hacia donde se comprime o desde donde se descomprime, guarda una copia del string)
	// _coder: CoderBlocks que se usara para la compresion y escritura. El Compressor toma posecion del objeto y lo borrara al terminar
	// _decoder: DecoderBlocks que se usara para descompresion y lectura. El Compressor tambien toma posecion de este objeto.
	Compressor(const char *_master_file, CoderBlocks *_coder, DecoderBlocks *_decoder, TextFilter *_filter = NULL);
	
	virtual ~Compressor();
	
	// Retoran el largo del texto comprimido
	// Si basta con decoder->getTextSize(), puede dejarse sin redefinir
	virtual long long getTextSize();
	
	// Extrae texto comprimido
	// Escribe length chars desde la posicion absoluta pos_ini en buff (asume al menos length + 1 chars de espacio)
	// Retorna el largo del texto escrito (igual a strlen de buff)
	virtual unsigned long long read(unsigned long long pos_ini, unsigned long long length, char *buff);
	
	// Escribe texto recomprimiendo los bloques involucrados
	// ESTO AUN NO ESTA TOTALMENTE IMPLEMENTADO
	virtual unsigned long long write(const char *text, unsigned long long length, unsigned long long pos_ini);
	
	// Descompresion completa del master_file asociado a este compressor
	// Escribe el texto descomprimido en out_file (en lineas de un cierto largo)
	// Eventualmente line_size sera removida
	// Datos como eso de ser necesarios, estaran en los metadatos de BlockHeaders del Coder/Decoder
	// Retorna true en exito, false en caso de fallo
	virtual bool decompress(const char *out_file, unsigned long long line_size = 70);
	
	// Compresion completa de un archivo de entrada in_file
	// REEMPLAZANDO el master_file asociado a este compressor
	// Retorna true en exito, false en caso de fallo
	virtual bool compress(const char *in_file, unsigned long long n_threads = 4, unsigned long long block_size = 1000000, bool use_metadata = true, vector<pair<unsigned long long, unsigned long long> > *external_factors = NULL);
	
	// Compress only to produce the factors
	// This method also returns the processed text and stores its length in text_length
	// This because the method removes newlines and potentially other special characters
	char * compressFactors(const char *in_file, unsigned long long block_size, unsigned long long &text_length, vector<pair<unsigned long long, unsigned long long> > *external_factors);
	
	// Este metodo debe recargar al decoder y dejar al compresor en estado inicial
	// Eso puede implicar resetear buffers y otras variables de estado
	virtual void reloadDecoder(){
		if(decoder == NULL || master_file == NULL || strlen(master_file) < 1){
			cerr<<"Compressor::reloadDecoder - Datos incorrectos\n";
		}
		else{
			decoder->load(master_file);
		}
	}
	
};







#endif //_COMPRESSOR_H





