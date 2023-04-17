#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <Arduino.h> // Font for display

class Library{

    private:
        String * fileArray;
        static const int SIZE = 20;
        int size;
        int index;

        friend class Display;

    public:
        // Constructor and Destructor
        Library(int size = SIZE) : size(size), index(0) {fileArray = new String[size];}
        ~Library() {delete [] fileArray;}

        // access methods

        //initialize file array
        void array();
        // add value to library array
        void addValue(String value);
        // resize the array
        void resize();
        // return value from array
        String * returnValue();

};

#endif