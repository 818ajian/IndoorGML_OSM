//
// Created by byeonggon on 2018-11-02.
//

#ifndef INDOORGML2OSM_TRANSFORM_H
#define INDOORGML2OSM_TRANSFORM_H
#include <string>
#include <vector>
#include <ostream>
#include "rapidxml_print.hpp"
#include "rapidxml.hpp"
class Pos;
namespace CONVERTER {
    class IC {
    public:
        int osm_id;
        int type;
        std::string gml_id;
        std::string Description;
        IC *duality = NULL;
        std::vector<IC *> connects;

    };

    class Pos : public IC {
    public:
        std::string latitude;
        std::string longitude;
        std::string height;
        Pos(){
           type=0;
        }
    };
    class CellSpace : public IC {
    public:
        std::string name;
        std::vector<Pos *> pos_vector;
        CellSpace(){
            type=1;
        }
    };
    class CellSpaceBoundary : public IC {
    public:
        std::vector<Pos *> pos_vector;
        CellSpaceBoundary(){
            type=2;
        }
    };
    class State : public IC {
    public:
        Pos *pos;
        State(){
            type=3;
        }
    };
    class Transition : public IC {
    public:
        std::string weight;
        std::vector<Pos *> pos_vector;
        Transition(){
            type=4;
        }
    };
}
#endif //INDOORGML2OSM_TRANSFORM_H
