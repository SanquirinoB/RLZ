#include "CoderBlocks.h"

CoderBlocks::CoderBlocks(){
}

CoderBlocks::~CoderBlocks(){
}

void CoderBlocks::codeBlock(const char *text, unsigned long long text_size, fstream *file_headers, fstream *file_data, unsigned long long &bytes_headers, unsigned long long &bytes_data, char *full_buffer, vector<pair<unsigned long long, unsigned long long> > *external_factors){
}

unsigned long long CoderBlocks::codingBufferSize(unsigned long long block_size){
	return 0;
}

CoderBlocks* CoderBlocks::copy() const{
	return new CoderBlocks();
}










