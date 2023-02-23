#ifndef REVERSE_HPP
#define REVERSE_HPP

#include <Arduino.h>
#include <SD.h>

class Reverse{

    private:
        File rawfile;
        uint32_t file_size;
        char array [];

    public:
        //Reverse() {array = new char[size];}
        //~Reverse() {delete [] array;}

        void createArray(const char * trackName);
        void reverseTrack(char * array[]);

};

#endif