//
// Created by byeonggon on 2018-10-29.
//

#include <iostream>
#include "rapidxml_print.hpp"
#include "rapidxml.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string.h>


using namespace std;
using namespace rapidxml;

std::vector<std::string> split(std::string& strToSplit, char delimeter);
std::string trim(const std::string& str);
template <class T1>
T1  matching_id(std::vector<T1> &a,int b);

int CellSpace_ID=1;
int CellSpaceBoundary_ID=1;
int State_id=1;
int Transition_id=1;

class Pos{
public:
    string latitude;
    string longitude;
    string height;
    int osm_id;
};

class CellSpace{
public:
    string gml_id;
    string Description;
    string name;
    string bounded_by;
    vector <Pos*> pos_vector;
    string duality_id;
    int osm_id;
};
class CellSpaceBoundary{
public:
    string gml_id;
    string bounded_by;
    string duality_id;
    vector <Pos*> pos_vector;
    int osm_id;
};
class State{
public:
    string gml_id;
    Pos *pos;
    string bounded_by;
    string duality_id;
    vector<string> connects_vector;
    int osm_id;
};
class Transition{
public:
    string gml_id;
    string bounded_by;
    string weight;
    string duality_id;
    vector <string> connects_vector;
    vector <Pos*> pos_vector;
    int osm_id;
};


int main(){
    vector <CellSpace*> CellSpace_vector;
    vector <CellSpaceBoundary*>CellSpaceBoundary_vector;
    vector <State*>State_vector;
    vector <Transition*>Transition_vector;

    vector <Pos*> node_vector;
    cout << "OSM to IndoorGML..."<<endl ;
    xml_document<> doc;//input
    xml_document<> doc1;//output
    xml_node<> * root_node;
    ifstream theFile ("C:\\Users\\byeonggon\\CLionProjects\\IndoorGML2OSM\\IndoorGML2OSM.xml");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]);
    root_node = doc.first_node("osm");
    for (xml_node<> * xml_node = root_node->first_node("node"); xml_node; xml_node = xml_node->next_sibling("node")) {
        Pos *node_input = new Pos();
        node_input->osm_id=atoi(xml_node->first_attribute("id")->value());
        node_input->latitude=xml_node->first_attribute("lat")->value();
        node_input->longitude=xml_node->first_attribute("lon")->value();
        node_vector.push_back(node_input);
    }//node input

    for (xml_node<> * xml_relation = root_node->first_node("relation"); xml_relation; xml_relation = xml_relation->next_sibling("relation")) {
        xml_node<> * xml_tag = xml_relation->first_node("tag");
        if(xml_tag ==NULL)continue;

        if (strcmp(xml_tag->first_attribute("v")->value(),"CellSpace") == 0){
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                CellSpace *cellspace_input = new CellSpace();
                cellspace_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                CellSpace_vector.push_back(cellspace_input);
            }
        }
        if (strcmp(xml_tag->first_attribute("v")->value(), "CellSpaceBoundary") == 0) {
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                CellSpaceBoundary *cellspaceboundary_input = new CellSpaceBoundary();
                cellspaceboundary_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                CellSpaceBoundary_vector.push_back(cellspaceboundary_input);
            }
        }
        if (strcmp(xml_tag->first_attribute("v")->value(), "State") == 0) {
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                State *state_input = new State();
                state_input->pos=matching_id(node_vector,atoi(xml_member->first_attribute("ref")->value()));
                State_vector.push_back(state_input);
            }
        }
        if (strcmp(xml_tag->first_attribute("v")->value(), "Transition") == 0) {
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                Transition *transition_input=new Transition();
                transition_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                Transition_vector.push_back(transition_input);
            }
        }
    }//cellspace, cellspaceboundary,state,transition push


    for (xml_node<> * xml_way = root_node->first_node("way"); xml_way; xml_way = xml_way->next_sibling("way")) {

        if(matching_id(CellSpace_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
            CellSpace * CellSpace_Pointer=matching_id(CellSpace_vector,atoi(xml_way->first_attribute("id")->value()));
            for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                CellSpace_Pointer->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
            }
            for(xml_node<>*xml_tag=xml_way->first_node("tag");xml_tag;xml_tag=xml_tag->next_sibling("tag")){
                if(strcmp(xml_tag->first_attribute("k")->value(),"name")==0){
                    CellSpace_Pointer->name=xml_tag->first_attribute("v")->value();
                    continue;
                }
                CellSpace_Pointer->Description.append(xml_tag->first_attribute("v")->value());
                CellSpace_Pointer->Description.append(" : ");
                CellSpace_Pointer->Description.append(xml_tag->first_attribute("k")->value());
                CellSpace_Pointer->Description.append(" ");
            }
        }//Cellspace_way
        if(matching_id(CellSpaceBoundary_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
            for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                matching_id(CellSpaceBoundary_vector,atoi(xml_way->first_attribute("id")->value()))->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
            }
        }
        if(matching_id(Transition_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
            for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                matching_id(Transition_vector,atoi(xml_way->first_attribute("id")->value()))->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
            }
        }//Transition_way
    }

    for(auto iter=State_vector.begin();iter!=State_vector.end();++iter){
        cout<<"osm : "<< (*iter)->pos->osm_id<<" ";
        cout<<"latitude : "<< (*iter)->pos->latitude<<" ";
        cout<<"logitutde : "<< (*iter)->pos->longitude<<endl;

    }
}

std::vector<std::string> split(std::string& strToSplit, char delimeter) {
    stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter)) {
        if(item.length()==0)continue;
        if(item.at(item.length()-1)=='\n')
            item=item.substr(0,item.length()-1);
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

std::string trim(const std::string& str) {
    std::string result;
    string whitespaces=" \t\r\n";
    size_t begin = str.find_first_not_of(whitespaces);
    size_t end = str.find_last_not_of(whitespaces);
    if (begin != std::string::npos && end != std::string::npos)
        result = str.substr(begin, end - begin + 1);
    return result;
}
template <class T1>
T1 matching_id(std::vector<T1> &a,int b){
    T1 result;
    for(auto iter=a.begin();iter!=a.end();++iter){
        if((*iter)->osm_id==b){
            result=*iter;
        }
    }
    return result;
}