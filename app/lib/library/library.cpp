#include <library.hpp>
#include <Arduino.h>
#include <SD.h>


const int SDCARD_MOSI_PIN = 11;
const int SDCARD_SCK_PIN = 13;
const int SDCARD_CS_PIN = BUILTIN_SDCARD;

// create the array of filenames
void Library::array(){
    File root = SD.open("/");
    int count = 0;
    while(true){
        File track = root.openNextFile();
        if(!track){
            break;
        }
         // Skip directories
        if (track.isDirectory()) {
        continue;
        }
        Serial.print(track.name());
        fileArray[count++] = track.name();
        //count++;
        track.close();
    };
    root.close();
}

// Add a value to the array
void Library::addValue(String value){
    fileArray[index++] = value;
}

// Print value fron array
String Library::printValue(int index){
    String a = fileArray[index];
    //Serial.println(a);
    return a;
}

// Return value from array
String * Library::returnValue(){
    //String a = fileArray[index];
    return fileArray;
}