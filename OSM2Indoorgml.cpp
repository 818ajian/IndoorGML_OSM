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
    vector <Member*>Member_vector;
    string type;
};

class pos{
public:
    int latitude;
    int longitude;
    int height;
};

class Cellspace{
public:
    int osm_id;
    int duality_id;
    string indoorgml_id;
    Way* way;

};

class CellSpaceBoundary{
public:
    int osm_id;
    int duality_id;
    string indoorgml_id;
    Way* way;

};

class Transition{
public:
    int duality;
    double weight=0.0;
    string indoorgml_id;
    vector<int>connects;
    Way* way;
};
class State{
public:
    int osm_id;
    int duality_id;
    Node State_point;
    vector<int>connects;
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
    vector<State*>State_vector;

    vector<Relation*>Relation_vector;
    vector<Transition*>Transition_vector;
    vector<PrimeSpaceFeature*>PrimeSpaceFeature_vector;

    PrimeSpaceFeature* PrimeSpaceFeature_input=new PrimeSpaceFeature();


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
    }//node


    for (xml_node<> * xml_Relation = root_node->first_node("relation"); xml_Relation; xml_Relation = xml_Relation->next_sibling("relation")) {
        Relation *relation_input= new Relation();
        for (xml_node<> * xml_member = xml_Relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")) {
            Member* member_input=new Member();
            member_input->type=xml_member->first_attribute("type")->value();
            member_input->ref_id=xml_member->first_attribute("ref")->value();
            if(member_input->type=="node"){
                State *state_input=new State();
                state_input->osm_id=atoi(member_input->ref_id.c_str());
                vector <string> coordinate;
                coordinate=find_coordinate(Node_vector,atoi(member_input->ref_id.c_str()));
                state_input->State_point.latitude=coordinate[0];
                state_input->State_point.longitutde=coordinate[1];
                State_vector.push_back(state_input);
            }
            relation_input->Member_vector.push_back(member_input);
        }
        xml_node<> * xml_tag=xml_Relation->first_node("tag");
        if(strcmp(xml_tag->first_attribute("k")->value(),"type")==0) {
            relation_input->type = xml_tag->first_attribute("v")->value();
            Relation_vector.push_back(relation_input);
        }
    }//relation


    for (xml_node<> * xml_way = root_node->first_node("way"); xml_way; xml_way = xml_way->next_sibling("way")) {
        Cellspace *cellspace_input= new Cellspace();
        CellSpaceBoundary *CellSpaceBoundary_input=new CellSpaceBoundary();
        Transition *Transition_input=new Transition();
        cellspace_input->way=new Way();
        CellSpaceBoundary_input->way=new Way();
        Transition_input->way=new Way();

        cellspace_input->osm_id = atoi(xml_way->first_attribute("id")->value());
        CellSpaceBoundary_input->osm_id = atoi(xml_way->first_attribute("id")->value());;
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
                    cellspace_input->way->Indoorgml_type="CellSpace";
                    cellspace_input->indoorgml_id="C"+to_string(Cellspace_id++);
                    PrimeSpaceFeature_input->Cellspace_vector.push_back(cellspace_input);
                }
                if(strcmp(des_input->value.c_str(),"CellSpaceBoundary")==0){
                    CellSpaceBoundary_input->way->Way_node_vector=node_input;
                    CellSpaceBoundary_input->way->Way_des_vector = description_input;
                    CellSpaceBoundary_input->way->Indoorgml_type="CellSpaceBoundary";
                    CellSpaceBoundary_input->indoorgml_id="B"+to_string(Cellspaceboundary_id++);
                    PrimeSpaceFeature_input->Cellspace_boundary_vector.push_back(CellSpaceBoundary_input);
                }
                if(strcmp(des_input->value.c_str(),"Transition")==0){
                    Transition_input->way->Way_node_vector=node_input;
                    Transition_input->way->Way_des_vector = description_input;
                    Transition_input->way->Indoorgml_type="Transition";
                    Transition_input->indoorgml_id="T"+to_string(Cellspaceboundary_id++);
                    Transition_vector.push_back(Transition_input);
                }
                continue;
            }
            description_input.push_back(des_input);
        }
    }
    PrimeSpaceFeature_vector.push_back(PrimeSpaceFeature_input);



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
    xml_node<>* xml_multiLayeredGraph=doc1.allocate_node(rapidxml::node_element, "multiLayeredGraph");
    xml_node<>* xml_MultiLayeredGraph=doc1.allocate_node(rapidxml::node_element, "MultiLayeredGraph");
    xml_node<>* xml_spaceLayers=doc1.allocate_node(rapidxml::node_element, "spaceLayers");
    xml_node<>* xml_spaceLayerMember=doc1.allocate_node(rapidxml::node_element, "spaceLayerMember");
    xml_node<>* xml_SpaceLayer=doc1.allocate_node(rapidxml::node_element, "SpaceLayer");

    xml_node<>* xml_nodes=doc1.allocate_node(rapidxml::node_element, "nodes");
    xml_node<>* xml_edges=doc1.allocate_node(rapidxml::node_element, "edges");

    for(auto iter=PrimeSpaceFeature_vector[0]->Cellspace_vector.begin();iter!=PrimeSpaceFeature_vector[0]->Cellspace_vector.end();++iter){
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

        xml_CellSpace->append_attribute(doc1.allocate_attribute("gml:id",(*iter)->indoorgml_id.c_str()));

        xml_name->value((*iter)->way->name.c_str());
        string sum="";
        for(auto iter_1=(*iter)->way->Way_des_vector.begin();iter_1!=(*iter)->way->Way_des_vector.end();++iter_1){
            string name = (*iter_1)->name;
            string value= (*iter_1)->value;
            sum=sum+name+" : "+value+" ";
        }
        sum=trim(sum);

        xml_description->value(doc1.allocate_string(sum.c_str()));
        xml_CellSpace->append_node(xml_description);
        xml_CellSpace->append_node(xml_name);
        xml_CellSpace -> append_node(xml_cellSpaceGeometry);
        xml_CellSpace->append_node(xml_Cellspace_bound);
        xml_cellSpaceMember->append_node(xml_CellSpace);
        xml_PrimalSpaceFeatures->append_node(xml_cellSpaceMember);
    }//cellspace


    for(auto iter=PrimeSpaceFeature_vector[0]->Cellspace_boundary_vector.begin();iter!=PrimeSpaceFeature_vector[0]->Cellspace_boundary_vector.end();++iter){
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
        xml_CellSpaceBoundary->append_attribute(doc1.allocate_attribute("gml:id",(*iter)->indoorgml_id.c_str()));
        xml_Geometry2D->append_node(xml_LineString);
        xml_cellSpaceBoundaryGeometry->append_node(xml_Geometry2D);
        xml_CellSpaceBoundary->append_node(xml_cellSpaceBoundaryGeometry);
        xml_cellSpaceBoundaryMember->append_node(xml_CellSpaceBoundary);

        xml_PrimalSpaceFeatures->append_node(xml_cellSpaceBoundaryMember);
    }//cellspaceboundary


    xml_node<>* xml_state_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
    xml_state_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

    for(auto iter=State_vector.begin();iter!=State_vector.end();++iter){
        xml_node<>* xml_stateMember=doc1.allocate_node(rapidxml::node_element, "stateMember");
        xml_node<>* xml_State=doc1.allocate_node(rapidxml::node_element, "State");
        xml_node<>* xml_State_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_State_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

        xml_node<>* xml_geometry = doc1.allocate_node(rapidxml::node_element, "geometry");
        xml_node<>* xml_gml_Point = doc1.allocate_node(rapidxml::node_element, "gml:Point");
        xml_node<>* xml_gml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");

        xml_gml_pos->value(doc1.allocate_string(((*iter)->State_point.latitude+" "+(*iter)->State_point.longitutde+" "+height).c_str()));
        xml_gml_Point->append_node(xml_gml_pos);
        xml_geometry->append_node(xml_gml_Point);
        xml_State->append_node(xml_geometry);
        xml_stateMember->append_node(xml_State);
        xml_nodes->append_node(xml_stateMember);
    }//State


    for(auto iter=Transition_vector.begin();iter!=Transition_vector.end();++iter){
        xml_node<>* xml_transitionMember=doc1.allocate_node(rapidxml::node_element, "transitionMember");
        xml_node<>* xml_Transition=doc1.allocate_node(rapidxml::node_element, "Transition");
        xml_node<>* xml_Transition_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_Transition_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

        xml_node<>* xml_geometry = doc1.allocate_node(rapidxml::node_element, "geometry");
        xml_node<>* xml_LineString = doc1.allocate_node(rapidxml::node_element, "gml:LineString");
        xml_node<>* xml_gml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");

        for(auto iter_1=(*iter)->way->Way_node_vector.begin();iter_1!=(*iter)->way->Way_node_vector.end();++iter_1){
            xml_node<>* xml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
            xml_pos->append_attribute(doc1.allocate_attribute("srcDimension",srsDimension.c_str()));
            vector <string> coordinate;
            coordinate=find_coordinate(Node_vector,(*iter_1)->osm_id);
            xml_pos->value(doc1.allocate_string((coordinate[0]+" "+coordinate[1]+" "+height).c_str()));
            xml_LineString->append_node(xml_pos);
        }

        xml_geometry->append_node(xml_LineString);
        xml_Transition->append_node(xml_geometry);
        xml_transitionMember->append_node(xml_Transition);
        xml_edges->append_node(xml_transitionMember);
    }//Transition

    xml_multiLayeredGraph->append_node(xml_MultiLayeredGraph);
    xml_MultiLayeredGraph->append_node(xml_bound);
    xml_MultiLayeredGraph->append_node(xml_spaceLayers);
    xml_spaceLayers->append_node(xml_SpaceLayer);
    xml_SpaceLayer->append_node(xml_nodes);
    xml_SpaceLayer->append_node(xml_edges);
    xml_primalSpaceFeatures->append_node(xml_PrimalSpaceFeatures);
    root->append_node(xml_state_bound);
    root->append_node(xml_primalSpaceFeatures);
    root->append_node(xml_multiLayeredGraph);
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