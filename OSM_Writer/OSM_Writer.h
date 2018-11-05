//
// Created by byeonggon on 2018-11-02.
//

#ifndef INDOORGML2OSM_OSM_WRITER_H
#define INDOORGML2OSM_OSM_WRITER_H
#include <TRANSFORM/Transform.h>


namespace OSM{
    void Write(std::vector<CONVERTER::IC*> IC_vector,std::string PATH);
    std::string trim(const std::string& str);
    std::vector<std::string> split(std::string& strToSplit, char delimeter);
}

#endif //INDOORGML2OSM_OSM_WRITER_H
