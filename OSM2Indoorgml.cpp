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


using namespace std;
using namespace rapidxml;

int Cellspace_id = 1;
int Cellspaceboundary_id=1;
int state_id=1;
int transition_id=1;
const string  boundedby_value="true";
const string srsDimension="3";
const string height="0.0";
class Description{
public:
    string name;
    string value;
};

class Node{
public:
    int osm_id;
    string latitude;
    string longitutde;
};

class Way{
public:
    int osm_id;
    vector <Node*> Way_node_vector;
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
    int osm_id;
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
    Way* way;
    string boundedby=boundedby_value;
    CellSpaceBoundaryGeometry Transition_CellSpaceBoundaryGeometry;
};

class PrimeSpaceFeature{
public:
    vector<Cellspace*> Cellspace_vector;
    vector<CellSpaceBoundary*>Cellspace_boundary_vector;
};
//indoorGML

vector<string> find_coordinate(vector<Node*>Node_vector,int osm_id);
std::vector<std::string> split(std::string& strToSplit, char delimeter);
std::string trim(const std::string& str);


int main(void){
    vector<Node*>Node_vector;
    vector<Way*>Way_vector;
    vector<Relation*>Relation_vector;
    vector<Transition*>Transition_vector;

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
        node_input->latitude=xml_node->first_attribute("lat")->value();
        node_input->longitutde=xml_node->first_attribute("lon")->value();
        Node_vector.push_back(node_input);
    }

    for (xml_node<> * xml_way = root_node->first_node("way"); xml_way; xml_way = xml_way->next_sibling("way")) {
        Cellspace *cellspace_input= new Cellspace();
        CellSpaceBoundary *CellSpaceBoundary_input=new CellSpaceBoundary();
        Transition *Transition_input=new Transition();
        cellspace_input->way=new Way();
        CellSpaceBoundary_input->way=new Way();
        Transition_input->way=new Way();

        cellspace_input->osm_id = atoi(xml_way->first_attribute("id")->value());
        vector <Node*>node_input;
        vector<Description*>description_input;

        for (xml_node<> * xml_node = xml_way->first_node("nd"); xml_node; xml_node = xml_node->next_sibling("nd")) {
            Node* node=new Node();
            node->osm_id=atoi(xml_node->first_attribute("ref")->value());
            node_input.push_back(node);
        }


        for (xml_node<> * xml_tag = xml_way->first_node("tag"); xml_tag; xml_tag = xml_tag->next_sibling("tag")) {
            Description *des_input= new Description();
            des_input->name=xml_tag->first_attribute("k")->value();
            des_input->value=xml_tag->first_attribute("v")->value();
            if(strcmp(des_input->name.c_str(),"name")==0) {
                cellspace_input->way->name=des_input->value;
                CellSpaceBoundary_input->way->name=des_input->value;
                continue;
            }
            if(strcmp(des_input->name.c_str(),"Indoorgml")==0) {
                if(strcmp(des_input->value.c_str(),"CellSpace")==0){
                    cellspace_input->way->Way_node_vector=node_input;
                    cellspace_input->way->Way_des_vector = description_input;
                    Cellspace_vector.push_back(cellspace_input);
                }
                if(strcmp(des_input->value.c_str(),"CellSpaceBoundary")==0){
                    CellSpaceBoundary_input->way->Way_node_vector=node_input;
                    CellSpaceBoundary_input->way->Way_des_vector = description_input;
                    Cellspace_boundary_vector.push_back(CellSpaceBoundary_input);
                }
                if(strcmp(des_input->value.c_str(),"Transition")==0){
                    Transition_input->way->Way_node_vector=node_input;
                    Transition_input->way->Way_des_vector = description_input;
                    Transition_vector.push_back(Transition_input);
                }
                continue;
            }
            description_input.push_back(des_input);
            //cout<<des_input->value<<" "<<des_input->name<<endl;
        }
    }

    cout<<endl<<Cellspace_vector.size()<<endl;



    xml_document<> doc1;
    xml_node<>* root = doc1.allocate_node(rapidxml::node_element, "IndoorFeatures");
    root->append_attribute(doc1.allocate_attribute("xmlns:gml", "http://www.opengis.net/gml/3.2"));
    root->append_attribute(doc1.allocate_attribute("xmlns:xlink", "http://www.w3.org/1999/xlink"));
    root->append_attribute(doc1.allocate_attribute("xmlns", "http://www.opengis.net/indoorgml/1.0/core"));
    root->append_attribute(doc1.allocate_attribute("xmlns:navi", "http://www.opengis.net/indoorgml/1.0/navigation"));
    root->append_attribute(doc1.allocate_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"));
    root->append_attribute(doc1.allocate_attribute("gml:id", "h9cc9b77-ae31-6074-2fe7-0c91a90f08f9"));
    root->append_attribute(doc1.allocate_attribute("xsi:schemaLocation", "http://www.opengis.net/indoorgml/1.0/core http://schemas.opengis.net/indoorgml/1.0/indoorgmlcore.xsd http://www.opengis.net/indoorgml/1.0/navigation http://schemas.opengis.net/indoorgml/1.0/indoorgmlnavi.xsd"));

    xml_node<>* xml_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
    xml_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

    xml_node<>* xml_primalSpaceFeatures = doc1.allocate_node(rapidxml::node_element, "primalSpaceFeatures");
    xml_node<>* xml_PrimalSpaceFeatures = doc1.allocate_node(rapidxml::node_element, "PrimalSpaceFeatures");

    for(auto iter=Cellspace_vector.begin();iter!=Cellspace_vector.end();++iter){
        xml_node<>* xml_cellSpaceMember = doc1.allocate_node(rapidxml::node_element, "cellSpaceMember");
        xml_node<>* xml_CellSpace = doc1.allocate_node(rapidxml::node_element, "CellSpace");
        xml_node<>* xml_description= doc1.allocate_node(rapidxml::node_element, "gml:description");
        xml_node<>* xml_name= doc1.allocate_node(rapidxml::node_element, "gml:name");
        xml_node<>* xml_Cellspace_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_Cellspace_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));
        xml_node<>* xml_cellSpaceGeometry = doc1.allocate_node(rapidxml::node_element, "cellSpaceGeometry");
        xml_node<>* xml_Geometry2D = doc1.allocate_node(rapidxml::node_element, "Geometry2D");
        xml_node<>* xml_Polygon = doc1.allocate_node(rapidxml::node_element, "gml:Polygon");
        xml_node<>* xml_exterior = doc1.allocate_node(rapidxml::node_element, "gml:exterior");
        xml_node<>* xml_LinearRing = doc1.allocate_node(rapidxml::node_element, "gml:LinearRing");

        for(auto iter_1=(*iter)->way->Way_node_vector.begin();iter_1!=(*iter)->way->Way_node_vector.end();++iter_1){
            xml_node<>* xml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
            xml_pos->append_attribute(doc1.allocate_attribute("srcDimension",srsDimension.c_str()));
            vector <string> coordinate;
            coordinate=find_coordinate(Node_vector,(*iter_1)->osm_id);
            xml_pos->value(doc1.allocate_string((coordinate[0]+" "+coordinate[1]+" "+height).c_str()));
            xml_LinearRing->append_node(xml_pos);
        }

        xml_exterior->append_node(xml_LinearRing);
        xml_Polygon->append_node(xml_exterior);
        xml_Geometry2D->append_node(xml_Polygon);
        xml_cellSpaceGeometry->append_node(xml_Geometry2D);


        xml_name->value((*iter)->way->name.c_str());

        string sum="";
        for(auto iter_1=(*iter)->way->Way_des_vector.begin();iter_1!=(*iter)->way->Way_des_vector.end();++iter_1){
            string name = (*iter_1)->name;
            string value= (*iter_1)->value;
            sum=sum+name+" : "+value+" ";

        }

        xml_description->value(doc1.allocate_string(sum.c_str()));
        xml_CellSpace->append_node(xml_description);
        xml_CellSpace->append_node(xml_name);
        xml_CellSpace -> append_node(xml_cellSpaceGeometry);
        xml_CellSpace->append_node(xml_Cellspace_bound);
        xml_cellSpaceMember->append_node(xml_CellSpace);
        xml_PrimalSpaceFeatures->append_node(xml_cellSpaceMember);
    }//cellspace


    for(auto iter=Cellspace_boundary_vector.begin();iter!=Cellspace_boundary_vector.end();++iter){
        xml_node<>* xml_cellSpaceBoundaryMember = doc1.allocate_node(rapidxml::node_element, "cellSpaceBoundaryMember");
        xml_node<>* xml_CellSpaceBoundary = doc1.allocate_node(rapidxml::node_element, "CellSpaceBoundary");
        xml_node<>* xml_CellSpaceBoundary_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_CellSpaceBoundary_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

        xml_node<>* xml_cellSpaceBoundaryGeometry = doc1.allocate_node(rapidxml::node_element, "cellSpaceBoundaryGeometry");
        xml_node<>* xml_Geometry2D = doc1.allocate_node(rapidxml::node_element, "Geometry2D");
        xml_node<>* xml_LineString = doc1.allocate_node(rapidxml::node_element, "gml:LineString");

        for(auto iter_1=(*iter)->way->Way_node_vector.begin();iter_1!=(*iter)->way->Way_node_vector.end();++iter_1){
            xml_node<>* xml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
            xml_pos->append_attribute(doc1.allocate_attribute("srcDimension",srsDimension.c_str()));
            vector <string> coordinate;
            coordinate=find_coordinate(Node_vector,(*iter_1)->osm_id);
            xml_pos->value(doc1.allocate_string((coordinate[0]+" "+coordinate[1]+" "+height).c_str()));
            xml_LineString->append_node(xml_pos);
        }

        xml_Geometry2D->append_node(xml_LineString);
        xml_cellSpaceBoundaryGeometry->append_node(xml_Geometry2D);
        xml_CellSpaceBoundary->append_node(xml_cellSpaceBoundaryGeometry);
        xml_cellSpaceBoundaryMember->append_node(xml_CellSpaceBoundary);

        xml_PrimalSpaceFeatures->append_node(xml_cellSpaceBoundaryMember);
    }//cellspaceboundary

    xml_primalSpaceFeatures->append_node(xml_PrimalSpaceFeatures);
    root->append_node(xml_bound);
    root->append_node(xml_primalSpaceFeatures);
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
vector<string> find_coordinate(vector<Node*>Node_vector, int node_osm_id){
    vector<string>output;
    for(auto iter=Node_vector.begin();iter!=Node_vector.end();++iter){
        if((*iter)->osm_id==node_osm_id){
            output.push_back((*iter)->longitutde);
            output.push_back((*iter)->latitude);
            break;
        }
    }
    return output;
}