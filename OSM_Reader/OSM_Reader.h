//
// Created by byeonggon on 2018-11-02.
//

#ifndef INDOORGML2OSM_OSM_READER_H
#define INDOORGML2OSM_OSM_READER_H
#include <TRANSFORM/Transform.h>
using namespace rapidxml;

namespace OSM{
    template<class T1>
    T1 matching_id(std::vector<T1> &a, int b);
    std::vector<CONVERTER::IC*> Read(std::string path);
}
#endif //INDOORGML2OSM_OSM_READER_H
