//
// Created by byeonggon on 2018-11-02.
//
#include "IndoorGML_Reader/IndoorGML_Reader.h"
#include "IndoorGML_Writer/IndoorGML_Writer.h"
#include "OSM_Reader/OSM_Reader.h"
#include "OSM_Writer/OSM_Writer.h"
#include <iostream>
#include <string.h>
int main(int argc, char **argv) {
    if(strcmp(argv[1],"IndoorGML")==0 && strcmp(argv[2],"OSM")==0){
        OSM::Write(INDOOR::Read(argv[3]),argv[4]);
    }
    if(strcmp(argv[1],"OSM")==0 && strcmp(argv[2],"IndoorGML")==0){
        INDOOR::Write(OSM::Read(argv[3]),argv[4]);
    }
    return 0;
}