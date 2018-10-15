//
// Created by byeonggon on 2018-10-12.
//
#include <iostream>
#include "rapidxml_print.hpp"
#include "rapidxml.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <string.h>

#define HEIGHT 0;
#define srsDimensionn 3;
int Cellspace_id = 1;
int Cellspaceboundary_id=1;
int state_id=1;
int transition_id=1;

using namespace std;
using namespace rapidxml;

const string  boundedby_value="true";


class Description{
public:
    string name;
    string value;
};

class Node{
public:
    int osm_id;
    int latitude;
    int longitutde;
};

class Way{
public:
    int osm_id;
    vector<Node*> Way_node_vector;
    vector<Description*> Way_des_vector;
    string name;
    string Indoorgml_type;
};
class Member{
public:
    string type;
    string ref_id;
};
class Relation{
public:
    int osm_id;
    vector<Description*> Way_des_vector;
    vector <Member*>Member_vector;
    string type;
};



class pos{
public:
    int latitude;
    int longitude;
    int height;
};
class LinearRing{
public:
    vector <pos*> pos;
};
class Exterior{
public:
    vector <LinearRing*> Lin;
};
class Interior{
public:
    vector <LinearRing*> Lin;
};
class Polygon{
public:
    string id;
    vector <Exterior*> exterior;
    vector <Interior*> interior;
};
class Gemetry2D{
public:
    vector <Polygon*> polygon;
};
class CellSpaceGeometry{
public:
    vector <Gemetry2D*> gemetry_2d;
};
class CellSpaceBoundaryGeometry{
public:
    vector <Gemetry2D*> gemetry_2d;
};

class Cellspace{
public:
    vector <Description*>Des;
    Way* way;
    string name;
    string boundedby=boundedby_value;
    CellSpaceGeometry cellspace_cellspacegeomery;
};

class CellSpaceBoundary{
public:
    vector <Description*>Des;
    string name;
    Way* way;
    string boundedby=boundedby_value;
    CellSpaceBoundaryGeometry CellSpaceBoundary_CellSpaceBoundaryGeometry;
};

class Transition{
public:
    vector <Description*>Des;
    string name;
    string boundedby=boundedby_value;
    CellSpaceBoundaryGeometry Transition_CellSpaceBoundaryGeometry;
};

class PrimeSpaceFeature{
public:
    vector<Cellspace*> cellspace_vector;
    vector<CellSpaceBoundary*>cellspace_boundary_vector;
};
//indoorGML


std::vector<std::string> split(std::string& strToSplit, char delimeter);
std::string trim(const std::string& str);


int main(void){
    vector<Node*>Node_vector;
    vector<Way*>Way_vector;
    vector<Relation*>Relation_vector;

    vector<Cellspace*>Cellspace_vector;
    vector<CellSpaceBoundary*>Cellspace_boundary_vector;

    cout << "Parsing OSM..." ;
    xml_document<> doc;
    xml_node<> * root_node;
    ifstream theFile ("C:\\Users\\byeonggon\\CLionProjects\\IndoorGML2OSM\\cmake-build-debug\\stored.osm");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]);

    root_node = doc.first_node("osm");
    for (xml_node<> * xml_node = root_node->first_node("node"); xml_node; xml_node = xml_node->next_sibling("node")) {
        Node *node_input = new Node();
        node_input->osm_id=atoi(xml_node->first_attribute("id")->value());
        node_input->latitude=atoi(xml_node->first_attribute("lat")->value());
        node_input->longitutde=atoi(xml_node->first_attribute("lon")->value());
        Node_vector.push_back(node_input);
    }

    for (xml_node<> * xml_way = root_node->first_node("way"); xml_way; xml_way = xml_way->next_sibling("way")) {
        Cellspace *cellspace_input= new Cellspace();
        CellSpaceBoundary *CellSpaceBoundary_input=new CellSpaceBoundary();

        Way *way_input=new Way();
        way_input->osm_id = atoi(xml_way->first_attribute("id")->value());
        for (xml_node<> * xml_node = xml_way->first_node("node"); xml_node; xml_node = xml_node->next_sibling("node")) {
            Node *node_input = new Node();
            node_input->osm_id=atoi(xml_way->first_attribute("ref")->value());
            way_input->Way_node_vector.push_back(node_input);
        }
        for (xml_node<> * xml_tag = xml_way->first_node("tag"); xml_tag; xml_tag = xml_tag->next_sibling("tag")) {
            Description *des_input= new Description();
            des_input->name=xml_tag->first_attribute("k")->value();
            des_input->value=xml_tag->first_attribute("v")->value();
            if(strcmp(des_input->name.c_str(),"name")==0) {
                way_input->name=des_input->value;
                continue;
            }
            if(strcmp(des_input->name.c_str(),"Indoorgml")==0) {
                way_input->Indoorgml_type = des_input->value;
                if(strcmp(way_input->Indoorgml_type.c_str(),"CellSpace")==0){
                    cellspace_input->way=way_input;
                    Cellspace_vector.push_back(cellspace_input);
                }
                if(strcmp(way_input->Indoorgml_type.c_str(),"CellSpaceBoundary")==0){
                    CellSpaceBoundary_input->way=way_input;
                    Cellspace_boundary_vector.push_back(CellSpaceBoundary_input);
                }
                continue;
            }
            way_input->Way_des_vector.push_back(des_input);
        }
    }
//    for(auto iter=Cellspace_vector.begin();iter!=Cellspace_vector.end();++iter){
//        for(auto iter1=(*iter)->way->Way_des_vector.begin();iter1!=(*iter)->way->Way_des_vector.end();++iter1){
//            cout<<(*iter1)->name<<" "<<(*iter1)->value<<endl;
//        }
//    }

/*    for (xml_node<> * xml_relation = root_node->first_node("relation"); xml_relation; xml_relation = xml_relation->next_sibling("relation")) {
        Relation *relation_input=new Relation();
        relation_input->osm_id=atoi(xml_relation->first_attribute("id")->value());
        for (xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")) {
            Member *member_input=new Member();
            member_input->type=xml_member->first_attribute("type")->value();
            member_input->ref_id=atoi(xml_member->first_attribute("ref")->value());
        }
        for (xml_node<> * xml_tag = xml_relation->first_node("tag"); xml_tag; xml_tag = xml_tag->next_sibling("tag")) {
            Description *des_input= new Description();
            des_input->name=xml_tag->first_attribute("k")->value();
            des_input->value=xml_tag->first_attribute("v")->value();
            if(strcmp(des_input->name.c_str(),"type")==0) {
                relation_input->type = des_input->value;
                continue;
            }
            relation_input->Way_des_vector.push_back(des_input);
        }
    }*/
    cout<<endl<<Cellspace_vector.size()<<endl;
    cout<<Cellspace_boundary_vector.size();


    xml_document<> doc1;
    xml_node<>* root = doc1.allocate_node(rapidxml::node_element, "IndoorFeatures");
    root->append_attribute(doc1.allocate_attribute("xmlns:gml", "http://www.opengis.net/gml/3.2"));
    root->append_attribute(doc1.allocate_attribute("xmlns:xlink", "http://www.w3.org/1999/xlink"));
    root->append_attribute(doc1.allocate_attribute("xmlns", "http://www.opengis.net/indoorgml/1.0/core"));
    root->append_attribute(doc1.allocate_attribute("xmlns:navi", "http://www.opengis.net/indoorgml/1.0/navigation"));
    root->append_attribute(doc1.allocate_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"));
    root->append_attribute(doc1.allocate_attribute("gml:id", "h9cc9b77-ae31-6074-2fe7-0c91a90f08f9"));
    root->append_attribute(doc1.allocate_attribute("xsi:schemaLocation", "http://www.opengis.net/indoorgml/1.0/core http://schemas.opengis.net/indoorgml/1.0/indoorgmlcore.xsd http://www.opengis.net/indoorgml/1.0/navigation http://schemas.opengis.net/indoorgml/1.0/indoorgmlnavi.xsd"));

    xml_node<>* bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
    bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

    xml_node<>* primalSpaceFeatures = doc1.allocate_node(rapidxml::node_element, "primalSpaceFeatures");
    xml_node<>* PrimalSpaceFeatures = doc1.allocate_node(rapidxml::node_element, "PrimalSpaceFeatures");
    for(auto iter=Cellspace_vector.begin();iter!=Cellspace_vector.end();++iter){
        xml_node<>* cellSpaceMember = doc1.allocate_node(rapidxml::node_element, "cellSpaceMember");
        xml_node<>* CellSpace = doc1.allocate_node(rapidxml::node_element, "CellSpace");
        xml_node<>* description= doc1.allocate_node(rapidxml::node_element, "gml:description");
        xml_node<>* name= doc1.allocate_node(rapidxml::node_element, "gml:name");
        xml_node<>* Cellspace_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        Cellspace_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));
        xml_node<>* cellSpaceGeometry = doc1.allocate_node(rapidxml::node_element, "cellSpaceGeometry");
        xml_node<>* Geometry2D = doc1.allocate_node(rapidxml::node_element, "Geometry2D");
        xml_node<>* Polygon = doc1.allocate_node(rapidxml::node_element, "gml:Polygon");

        name->value((*iter)->way->name.c_str());

        string sum="";
        for(auto iter_1=(*iter)->way->Way_des_vector.begin();iter_1!=(*iter)->way->Way_des_vector.end();++iter_1){
            string name = (*iter_1)->name;
            string value= (*iter_1)->value;
            sum=sum+name+" : "+value+" ";
        }

        description->value(sum.c_str());
        CellSpace->append_node(description);
        CellSpace->append_node(name);
        CellSpace->append_node(Cellspace_bound);
        cellSpaceMember->append_node(CellSpace);
        PrimalSpaceFeatures->append_node(cellSpaceMember);
    }
    primalSpaceFeatures->append_node(PrimalSpaceFeatures);
    root->append_node(bound);
    root->append_node(primalSpaceFeatures);
    doc1.append_node(root);

    ofstream file_stored("osm2indoorgml.xml");

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
