#include "BlockHeaders.h"

BlockHeaders::BlockHeaders(){
	text_size = 0;
	block_size = 0;
	data_pos = 0;
	metadata = NULL;
	bytes_total_initial = 0;
	unprepared_block = 0;
}

BlockHeaders::BlockHeaders(unsigned long long _text_size, unsigned long long _block_size, Metadata *_metadata){
	text_size = _text_size;
	block_size = _block_size;
	data_pos = 0;
	metadata = _metadata;
	bytes_total_initial = 0;
	unprepared_block = 0;
}

BlockHeaders::~BlockHeaders(){
	if(metadata != NULL){
		delete metadata;
		metadata = NULL;
	}
}
	
Metadata *BlockHeaders::getMetadata(){
	return metadata;
}

void BlockHeaders::setMetadata(Metadata *_metadata){
	metadata = _metadata;
}

void BlockHeaders::addBlock(Header *header){
	cerr<<"BlockHeaders::addBlock - Not Implemented\n";
}

void BlockHeaders::loadBlock(fstream *reader, unsigned long long bytes){
	cerr<<"BlockHeaders::loadBlock - Not Implemented\n";
}

void BlockHeaders::reloadBlock(fstream *reader, unsigned long long bytes, unsigned long long pos){
	cerr<<"BlockHeaders::loadBlock - Not Implemented\n";
}

unsigned long long BlockHeaders::save(fstream *writer){
	cerr<<"BlockHeaders::save - Not Implemented\n";
	return 0;
}

void BlockHeaders::load(fstream *reader){
	cerr<<"BlockHeaders::load - Not Implemented\n";
}

void BlockHeaders::load(BytesReader *reader){
	cerr<<"BlockHeaders::load - Not Implemented\n";
}

unsigned long long BlockHeaders::getBlockSize(){
	return block_size;
}

unsigned long long BlockHeaders::getTextSize(){
	if( metadata != NULL ){
		return text_size + metadata->totalNewLines();
	}
	return text_size;
}

unsigned long long BlockHeaders::getDataPosition(){
	return data_pos;
}

unsigned long long BlockHeaders::getBlockPosition(unsigned long long block){
	cerr<<"BlockHeaders::getBlockPosition - Not Implemented\n";
	return 0;
}
	
unsigned long long BlockHeaders::getNumBlocks(){
	cerr<<"BlockHeaders::getNumBlocks - Not Implemented\n";
	return 0;
}

void BlockHeaders::prepare(){
	cerr<<"BlockHeaders::prepare - Not Implemented\n";
}

void BlockHeaders::unprepare(unsigned long long block_ini){
	cerr<<"BlockHeaders::unprepare - Not Implemented\n";
}

void BlockHeaders::adjustCase(char *buff, unsigned long long ini, unsigned long long length){
	if( metadata == NULL ){
		return;
	}
//	cout<<"BlockHeaders::adjustCase - metadata->adjustCase...\n";
	metadata->adjustCase(buff, ini, length);
}

unsigned long long BlockHeaders::countNewLines(unsigned long long pos){
	if( metadata == NULL ){
		return 0;
	}
//	cout<<"BlockHeaders::countNewLines - metadata->countNewLines...\n";
	return metadata->countNewLines(pos);
}

//Asume que la pos de inicio ya esta ajustada para ser absoluta (ini = rel_pos + nl_izq) al menos por ahora
void BlockHeaders::adjustNewLines(char *buff, unsigned long long ini, unsigned long long length, unsigned long long nl_izq, unsigned long long nl_med, char *copy_buff){
	if( metadata == NULL ){
		return;
	}
//	cout<<"BlockHeaders::adjustNewLines - metadata->adjustNewLines...\n";
	metadata->adjustNewLines(buff, ini, length, nl_izq, nl_med, copy_buff);
}

void BlockHeaders::filterNewText(const char *in_buff, unsigned long long length, unsigned long long pos_ini, char *out_buff, unsigned long long &adjusted_length, unsigned long long &adjusted_pos_ini){
	if( metadata == NULL ){
		return;
	}
	metadata->filterNewText(in_buff, length, pos_ini, out_buff, adjusted_length, adjusted_pos_ini);
}









