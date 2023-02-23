#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <Arduino.h> // Font for display

class Library{

    private:
        String * fileArray;
        static const int SIZE = 10;
        int size;
        int index;
    
    public:
        // Constructor and Destructor
        Library(int size = SIZE) : size(size), index(0) {fileArray = new String[size];}
        ~Library() {delete [] fileArray;}

        // access methods

        // add value to library array
        void addValue(String value);

        // return value from array
        String returnValue(int index);

        //print value ----- PROBABLY NOT NEEDED
        void printValue(int index);

};

#endif