#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv)
 {
     std::ifstream infile(argv[1]);
     string line;

     getline( infile, line );

     while(getline(infile, line)) {
         cout << line;
     }
    
    return 0; 

 }
