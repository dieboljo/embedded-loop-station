#include <library.hpp>


// Add a value to the array
void Library::addValue(String value){
    fileArray[index++] = value;
}

// Print value fron array
void Library::printValue(int index){
    String a = fileArray[index];
    Serial.println(a);
}

// Return value from array
String Library::returnValue(int index){
    String a = fileArray[index];
    return a;
}