#include <reverse.hpp>


void Reverse::createArray(const char * trackName){

    // open file
    rawfile = SD.open(trackName, FILE_READ);
    // create array of rawfile size
    array[rawfile.size()];

    
    if (rawfile){
        while (rawfile.available()){

            int max_len = rawfile.available();

        }
    }

    
}


void Reverse::reverseTrack(char * array[]){

    
}