#include <iostream>
#include <stdio.h>
#include "rapidxml_print.hpp"
#include "rapidxml.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
using namespace std;
using namespace rapidxml;
int NODE_ID=-1;
int WAY_ID=-30000;

class Descript{
public:
    string name;
    string value;
};
class Relate{
public:
    int first_id;
    int second_id;
};
class Relation{
public:
    vector<Relate*>duality;
    vector<Relate*>connect;
};

class Node{
public:
    int id;
    vector <Descript*> Des;
    string latitude;
    string longitude;
    string indoorgml_id;
    string height;
    string name;
};

class Way{
public:
    vector <Node*> CellSpace;
    vector <Node*> Cellspaceboundary;
    vector <Descript*> Des;
    string name;
    string indoorgml_id;
    int id;
};

std::vector<std::string> split(std::string& strToSplit, char delimeter);
std::string trim(const std::string& str);
int find_osm_way_id(vector<Way*>input,const string& indoorgml_id);
int find_osm_node_id(vector<Node*>input,const string& indoorgml_id);

int main(void) {

    vector <Node*> node_vector;
    vector <Way*> way_vector;
    Relation *relation_vector=new Relation();
    cout << "Parsing IndoorGML..." << endl;
    xml_document<> doc;
    xml_node<> * root_node;
    ifstream theFile ("C:\\Users\\byeonggon\\CLionProjects\\IndoorGML2OSM\\313_4F_2D.gml");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]);
    root_node = doc.first_node("IndoorFeatures");


    string erase_space;
    xml_node<> * primalSpaceFeatures = root_node->first_node("primalSpaceFeatures");
    xml_node<> * PrimalSpaceFeatures = primalSpaceFeatures->first_node("PrimalSpaceFeatures");

    xml_node<> * multiLayeredGraph = root_node->first_node("multiLayeredGraph");
    xml_node<> * MultiLayeredGraph = multiLayeredGraph->first_node("MultiLayeredGraph");
    xml_node<> * spaceLayers = MultiLayeredGraph->first_node("spaceLayers");
    xml_node<> * spaceLayerMember = spaceLayers->first_node("spaceLayerMember");
    xml_node<> * SpaceLayer = spaceLayerMember->first_node("SpaceLayer");
    xml_node<> * nodes = SpaceLayer->first_node("nodes");
    xml_node<> * edges = SpaceLayer->first_node("edges");


    for (xml_node<> * cellSpaceMember = PrimalSpaceFeatures->first_node("cellSpaceMember"); cellSpaceMember; cellSpaceMember = cellSpaceMember->next_sibling("cellSpaceMember")) {
        xml_node<> *CellSpace = cellSpaceMember->first_node("CellSpace");
        xml_node<> *Description = CellSpace->first_node("gml:description");
        xml_node<> *Duality = CellSpace->first_node("duality");
        erase_space = trim(Description->value());
        Way *way_input = new Way();
        std::vector<std::string> splittedStrings = split(erase_space, ' ');
        for (auto it = splittedStrings.begin(); it != splittedStrings.end();) {
            if (strcmp((*it).c_str(), "") == 0) {
                it = splittedStrings.erase(it);
            } else {
                it++;
            }
        }

        for (int i = 0; i < splittedStrings.size();) {
            Descript *des_input = new Descript();
            des_input->name = splittedStrings[i];
            des_input->value = splittedStrings[i + 2];
            way_input->Des.push_back(des_input);
            i += 3;
        }
        //cout<<way_input->Des.size()<<endl;
        xml_node<> *cellSpaceGeometry = CellSpace->first_node("cellSpaceGeometry");
        xml_node<> *Geometry2D = cellSpaceGeometry->first_node("Geometry2D");
        xml_node<> *Polygon = Geometry2D->first_node("gml:Polygon");
        xml_node<> *Exterior = Polygon->first_node("gml:exterior");
        xml_node<> *LinearRing = Exterior->first_node("gml:LinearRing");

        way_input->name = trim(CellSpace->first_node("gml:name")->value());

        for (xml_node<> *pos = LinearRing->first_node("gml:pos"); pos; pos = pos->next_sibling("gml:pos")) {
            erase_space = trim(pos->value());
            std::vector<std::string> splittedStrings = split(erase_space, ' ');
            Node *input = new Node();
            for (int i = 0; i < splittedStrings.size(); i++) {
                input->longitude = splittedStrings[1];
                input->latitude = splittedStrings[0];
                input->height = splittedStrings[2];
            }

            input->id = NODE_ID--;
            way_input->CellSpace.push_back(input);
            node_vector.push_back(input);
        }
        way_input->id = WAY_ID--;
        way_input->indoorgml_id = CellSpace->first_attribute("gml:id")->value();
        //cout<<CellSpace->first_attribute("gml:id")->value()<<endl;
        //id_vector.push_back(make_pair(way_input,CellSpace->first_attribute()->value()));
        way_vector.push_back(way_input);
    }//CellSpace

    for (xml_node<> * cellSpaceBoundaryMember = PrimalSpaceFeatures->first_node("cellSpaceBoundaryMember"); cellSpaceBoundaryMember; cellSpaceBoundaryMember = cellSpaceBoundaryMember->next_sibling("cellSpaceBoundaryMember")) {
        xml_node<> * CellSpaceBoundary = cellSpaceBoundaryMember->first_node("CellSpaceBoundary");
        xml_node<> * cellSpaceBoundaryGeometry = CellSpaceBoundary->first_node("cellSpaceBoundaryGeometry");
        xml_node<> * Geometry2D = cellSpaceBoundaryGeometry->first_node("geometry2D");
        xml_node<> * LineString = Geometry2D->first_node("gml:LineString");
        Way * way_input=new Way();
        way_input->indoorgml_id =CellSpaceBoundary->first_attribute("gml:id")->value();

        for(xml_node<> * pos = LineString->first_node("gml:pos"); pos; pos = pos->next_sibling("gml:pos")){
            string erase_space = trim(pos->value());
            std::vector<std::string> splittedStrings = split(erase_space, ' ');
            Node* input = new Node();
            for(int i = 0; i < splittedStrings.size() ; i++){
                input->longitude = splittedStrings[1];
                input->latitude = splittedStrings[0];
                input->height = splittedStrings[2];
            }
            input->id = NODE_ID--;
            way_input->indoorgml_id=CellSpaceBoundary->first_attribute("gml:id")->value();
            way_input->Cellspaceboundary.push_back(input);

            node_vector.push_back(input);
        }
        way_vector.push_back(way_input);
    }//Cellspaceboundary

    for (xml_node<> * stateMember = nodes->first_node("stateMember"); stateMember; stateMember = stateMember->next_sibling("stateMember")) {
        xml_node<> * State = stateMember->first_node("State");
        xml_node<> * geometry = State->first_node("geometry");
        xml_node<> * Point = geometry->first_node("gml:Point");
        xml_node<> * pos =  Point->first_node("gml:pos");

        string erase_space = trim(pos->value());
        std::vector<std::string> splittedStrings = split(erase_space, ' ');
        Node* input = new Node();
        for(int i = 0; i < splittedStrings.size() ; i++){
            input->longitude = splittedStrings[1];
            input->latitude = splittedStrings[0];
            input->height = splittedStrings[2];
        }
        input->indoorgml_id = State->first_attribute("gml:id")->value();
        input->id = NODE_ID--;
        Relation * relate_input= new Relation();
        for(xml_node<> * duality = State->first_node("duality");duality; duality = duality->next_sibling("duality")){
            Relate * element_input= new Relate();
            element_input->first_id= find_osm_way_id(way_vector,duality->first_attribute()->value());
            element_input->second_id=input->id;
            relation_vector->duality.push_back(element_input);
        }//duality relation make

        for(xml_node<> * connects = State->first_node("connects");connects; connects = connects->next_sibling("connects")){
            Relate * element_input= new Relate();
            element_input->first_id = find_osm_node_id(node_vector,connects->first_attribute()->value());
            element_input->second_id = input->id;
            relation_vector->connect.push_back(element_input);//transition 아직 입력 안됨
        }
        node_vector.push_back(input);
    }//state

    for (xml_node<> * transitionMember = edges->first_node("transitionMember"); transitionMember; transitionMember = transitionMember->next_sibling("transitionMember")) {
        xml_node<> * Transition = transitionMember->first_node("Transition");
        xml_node<> * geometry = Transition->first_node("geometry");
        xml_node<> * LineString = geometry->first_node("gml:LineString");
        xml_node<> * pos =  LineString->first_node("gml:pos");

        string erase_space = trim(pos->value());
        std::vector<std::string> splittedStrings = split(erase_space, ' ');
        Node* input = new Node();
        for(int i = 0; i < splittedStrings.size() ; i++){
            input->longitude = splittedStrings[1];
            input->latitude = splittedStrings[0];
            input->height = splittedStrings[2];
        }
        input->id = NODE_ID--;
        Relation * relate_input= new Relation();
//        for(xml_node<> * duality = Transition->first_node("duality");duality; duality = duality->next_sibling("duality")){
//            Relate * element_input= new Relate();
//            element_input->first_id= find_osm_way_id(way_vector,duality->first_attribute()->value());
//            element_input->second_id=input->id;
//            relation_vector->duality.push_back(element_input);
//        }//duality relation make
//
//        for(xml_node<> * connects = Transition->first_node("connects");connects; connects = connects->next_sibling("connects")){
//            Relate * element_input= new Relate();
//            element_input->first_id = find_osm_way_id(way_vector,connects->first_attribute()->value());
//            element_input->second_id = input->id;
//            relation_vector->connect.push_back(element_input);
//        }
        node_vector.push_back(input);
    }//transition

    doc.clear();
    xml_document<> doc1;


// xml declaration
    xml_node<>* decl = doc1.allocate_node(rapidxml::node_declaration);
    decl->append_attribute(doc1.allocate_attribute("version", "1.0"));
    decl->append_attribute(doc1.allocate_attribute("encoding", "UTF-8"));
    doc1.append_node(decl);

// root node
    xml_node<>* root = doc1.allocate_node(rapidxml::node_element, "osm");
    root->append_attribute(doc1.allocate_attribute("version", "0.6"));
    root->append_attribute(doc1.allocate_attribute("generator", "PNU_STEM_LAB"));
    doc1.append_node(root);

    //cout<<way_vector[0]->CellSpace.size();

    for(auto iter = node_vector.begin(); iter!=node_vector.end(); ++iter){
        xml_node<> *node  = doc1.allocate_node(rapidxml::node_element, "node");
        node->append_attribute(doc1.allocate_attribute("action", "modify"));
        node->append_attribute(doc1.allocate_attribute("visible", "true"));
        node->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->id).c_str())));
        node->append_attribute(doc1.allocate_attribute("lat", doc1.allocate_string(((*iter)->latitude).c_str())));
        node->append_attribute(doc1.allocate_attribute("lon", doc1.allocate_string(((*iter)->longitude).c_str())));
        root->append_node(node);
    }

    for(auto iter=way_vector.begin();iter!=way_vector.end();++iter){

        xml_node<> *way  = doc1.allocate_node(rapidxml::node_element, "way");
        way->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->id).c_str())));
        way->append_attribute(doc1.allocate_attribute("action", "modify"));
        way->append_attribute(doc1.allocate_attribute("visible", "true"));

        for(int i=0;i < (*iter)->CellSpace.size(); i++) {
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->CellSpace[i]->id).c_str())));
            way->append_node(nd);
        }//Cellspace

        for(int i=0;i < (*iter)->Cellspaceboundary.size(); i++) {
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->Cellspaceboundary[i]->id).c_str())));
            way->append_node(nd);
        }//CellspaceBoundary

        xml_node<> *tag  = doc1.allocate_node(rapidxml::node_element, "tag");
        tag->append_attribute(doc1.allocate_attribute("k", "name"));
        tag->append_attribute(doc1.allocate_attribute("v", doc1.allocate_string(((*iter)->name).c_str())));
        way->append_node(tag);

        for(int i=0; i<(*iter)->Des.size();i++){
            xml_node<> *tag  = doc1.allocate_node(rapidxml::node_element, "tag");
            tag->append_attribute(doc1.allocate_attribute("k", doc1.allocate_string(((*iter)->Des[i]->name).c_str())));
            tag->append_attribute(doc1.allocate_attribute("v", doc1.allocate_string(((*iter)->Des[i]->value).c_str())));
            way->append_node(tag);
        }
        root->append_node(way);
    }

    for(auto iter=relation_vector->duality.begin();iter!=relation_vector->duality.end();++iter){
        cout<<(*iter)->first_id<< "  "<<(*iter)->second_id<<" duality"<<endl;
    }
    for(auto iter=relation_vector->connect.begin();iter!=relation_vector->connect.end();++iter){
        cout<<(*iter)->first_id<< "  "<<(*iter)->second_id<<" connects"<<endl;
    }

    ofstream file_stored("stored.xml");
    file_stored << doc1;
    file_stored.close();
    doc1.clear();
    return 0;
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
int find_osm_way_id(vector<Way*>input,const string& indoorgml_id){
    for(auto iter=input.begin();iter!=input.end();++iter){
        if(strcmp((*iter)->indoorgml_id.c_str(),indoorgml_id.substr(1,indoorgml_id.length()-1).c_str())==0) {
            return (*iter)->id;
        }
    }
    return 0 ;
}
int find_osm_node_id(vector<Node*>input,const string& indoorgml_id){
    for(auto iter=input.begin();iter!=input.end();++iter){
        if(strcmp((*iter)->indoorgml_id.c_str(),indoorgml_id.substr(1,indoorgml_id.length()-1).c_str())==0) {
            return (*iter)->id;
        }
    }
    return 0 ;
}