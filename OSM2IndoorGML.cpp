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

class IC;
class Pos;
class CellSpace;
class CellSpaceBoundary;
class Transition;
class State;

const string height="0.0";
const string boundedby_value="true";
const string srsDimension="3";
int CellSpace_ID=1;
int CellSpaceBoundary_ID=1;
int State_id=1;
int Transition_id=1;


class IC{
public:
    string gml_id;
    int osm_id;
    string Description;
    string bounded_by;
    IC* duaility = NULL;
    vector <IC*> connects;
};

class Pos : public IC{
public:
    string latitude;
    string longitude;
    string height;

};
class CellSpace : public IC{
public:
    string name;
    vector <Pos*> pos_vector;
};
class CellSpaceBoundary:public IC{
public:
    vector <Pos*> pos_vector;
};
class State : public IC{
public:
    Pos *pos;
};
class Transition : public IC{
public:
    string weight;
    vector <Pos*> pos_vector;
};


int main(){
    vector <CellSpace*> CellSpace_vector;
    vector <CellSpaceBoundary*>CellSpaceBoundary_vector;
    vector <State*>State_vector;
    vector <Transition*>Transition_vector;
    vector <Pos*> node_vector;
    vector <IC*> IC_vector;

    cout << "OSM to IndoorGML..."<<endl ;
    xml_document<> doc;//input
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
        //IC_vector.push_back(node_input);
    }//node input

    for (xml_node<> * xml_relation = root_node->first_node("relation"); xml_relation; xml_relation = xml_relation->next_sibling("relation")) {
        xml_node<> * xml_tag = xml_relation->first_node("tag");
        if (strcmp(xml_tag->first_attribute("v")->value(),"CellSpace") == 0){
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                CellSpace *cellspace_input = new CellSpace();
                cellspace_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                cellspace_input->gml_id="C"+to_string(CellSpace_ID++);
                CellSpace_vector.push_back(cellspace_input);
                IC_vector.push_back(cellspace_input);
            }
        }
        if (strcmp(xml_tag->first_attribute("v")->value(), "CellSpaceBoundary") == 0) {
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                CellSpaceBoundary *cellspaceboundary_input = new CellSpaceBoundary();
                cellspaceboundary_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                cellspaceboundary_input->gml_id="B"+to_string(CellSpaceBoundary_ID++);
                CellSpaceBoundary_vector.push_back(cellspaceboundary_input);
                IC_vector.push_back(cellspaceboundary_input);
            }
        }
        if (strcmp(xml_tag->first_attribute("v")->value(), "State") == 0) {
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                State *state_input = new State();
                state_input->pos=matching_id(node_vector,atoi(xml_member->first_attribute("ref")->value()));
                state_input->osm_id=state_input->pos->osm_id;
                state_input->gml_id="S"+to_string(State_id++);
                State_vector.push_back(state_input);
                IC_vector.push_back(state_input);
            }
        }
        if (strcmp(xml_tag->first_attribute("v")->value(), "Transition") == 0) {
            for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                Transition *transition_input=new Transition();
                transition_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                transition_input->gml_id="T"+to_string(Transition_id++);
                Transition_vector.push_back(transition_input);
                IC_vector.push_back(transition_input);
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
                CellSpace_Pointer->Description.append(xml_tag->first_attribute("k")->value());
                CellSpace_Pointer->Description.append(" : ");
                CellSpace_Pointer->Description.append(xml_tag->first_attribute("v")->value());
                CellSpace_Pointer->Description.append(" ");
            }
        }//Cellspace_way
        if(matching_id(CellSpaceBoundary_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
            CellSpaceBoundary * CellSpaceBoundary_Pointer=matching_id(CellSpaceBoundary_vector,atoi(xml_way->first_attribute("id")->value()));
            for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                CellSpaceBoundary_Pointer->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
            }
        }//CellSpaceBoundary_way
        if(matching_id(Transition_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
            Transition * Transition_Pointer=matching_id(Transition_vector,atoi(xml_way->first_attribute("id")->value()));
            for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                Transition_Pointer->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
            }
        }//Transition_way
    }
    for (xml_node<> * xml_relation = root_node->first_node("relation"); xml_relation; xml_relation = xml_relation->next_sibling("relation")) {
        xml_node<> * xml_tag = xml_relation->first_node("tag");
        if(xml_tag==NULL)continue;
        if(strcmp(xml_tag->first_attribute("v")->value(),"duality")==0){
            xml_node<> *xml_member=xml_relation->first_node("member");
            xml_node<> *xml_member_1=xml_member->next_sibling("member");
            matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()))->duaility=matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value()));
            matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value()))->duaility=matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()));
        }
        if(strcmp(xml_tag->first_attribute("v")->value(),"connects")==0){
            xml_node<> *xml_member=xml_relation->first_node("member");
            xml_node<> *xml_member_1=xml_member->next_sibling("member");
            matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()))->connects.push_back(matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value())));
            matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value()))->connects.push_back(matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value())));
        }
    }

    doc.clear();

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

    xml_MultiLayeredGraph->append_node(xml_spaceLayers);
    xml_spaceLayers->append_node(xml_spaceLayerMember);
    xml_spaceLayerMember->append_node(xml_SpaceLayer);
    xml_SpaceLayer->append_node(xml_nodes);
    xml_SpaceLayer->append_node(xml_edges);

    for(auto it : CellSpace_vector) {
        xml_node<>* xml_cellSpaceMember = doc1.allocate_node(rapidxml::node_element, "cellSpaceMember");
        xml_node<>* xml_CellSpace = doc1.allocate_node(rapidxml::node_element, "CellSpace");
        xml_CellSpace->append_attribute(doc1.allocate_attribute("gml:id",doc1.allocate_string(it->gml_id.c_str())));
        xml_node<>* xml_description= doc1.allocate_node(rapidxml::node_element, "gml:description");
        xml_node<>* xml_name= doc1.allocate_node(rapidxml::node_element, "gml:name");
        xml_node<>* xml_Cellspace_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_Cellspace_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));
        xml_node<>* xml_cellSpaceGeometry = doc1.allocate_node(rapidxml::node_element, "cellSpaceGeometry");
        xml_node<>* xml_Geometry2D = doc1.allocate_node(rapidxml::node_element, "Geometry2D");
        xml_node<>* xml_Polygon = doc1.allocate_node(rapidxml::node_element, "gml:Polygon");
        xml_node<>* xml_exterior = doc1.allocate_node(rapidxml::node_element, "gml:exterior");
        xml_node<>* xml_LinearRing = doc1.allocate_node(rapidxml::node_element, "gml:LinearRing");
        xml_node<>* xml_duality = doc1.allocate_node(rapidxml::node_element, "duality");
        xml_duality->append_attribute(doc1.allocate_attribute("xlink:href",doc1.allocate_string(("#"+it->duaility->gml_id).c_str())));
        xml_description->value(doc1.allocate_string(trim(it->Description).c_str()));
        xml_name->value(doc1.allocate_string((it->name).c_str()));
        for(auto it1 : it->pos_vector){
            xml_node<>* xml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
            xml_pos->value(doc1.allocate_string((it1->latitude+" "+it1->longitude+" "+height).c_str()));
            xml_LinearRing->append_node(xml_pos);
        }
        xml_exterior->append_node(xml_LinearRing);
        xml_Polygon->append_node(xml_exterior);
        xml_Geometry2D->append_node(xml_Polygon);

        xml_cellSpaceGeometry->append_node(xml_Geometry2D);
        xml_CellSpace->append_node(xml_name);
        xml_CellSpace->append_node(xml_description);
        xml_CellSpace->append_node(xml_cellSpaceGeometry);
        xml_CellSpace->append_node(xml_duality);

        xml_cellSpaceMember->append_node(xml_CellSpace);
        xml_PrimalSpaceFeatures->append_node(xml_cellSpaceMember);
    }
    for(auto it : CellSpaceBoundary_vector){
        xml_node<>* xml_cellSpaceBoundaryMember = doc1.allocate_node(rapidxml::node_element, "cellSpaceBoundaryMember");
        xml_node<>* xml_CellSpaceBoundary = doc1.allocate_node(rapidxml::node_element, "CellSpaceBoundary");
        xml_node<>* xml_CellSpaceBoundary_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_CellSpaceBoundary_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));
        xml_node<>* xml_duality = doc1.allocate_node(rapidxml::node_element, "duality");
        if(it->duaility!=NULL)
            xml_duality->append_attribute(doc1.allocate_attribute("xlink:href",doc1.allocate_string(("#"+it->duaility->gml_id).c_str())));
        xml_node<>* xml_cellSpaceBoundaryGeometry = doc1.allocate_node(rapidxml::node_element, "cellSpaceBoundaryGeometry");
        xml_node<>* xml_Geometry2D = doc1.allocate_node(rapidxml::node_element, "Geometry2D");
        xml_node<>* xml_LineString = doc1.allocate_node(rapidxml::node_element, "gml:LineString");

        for(auto it1 : it->pos_vector){
            xml_node<>* xml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
            xml_pos->append_attribute(doc1.allocate_attribute("srcDimension",srsDimension.c_str()));
            xml_pos->value(doc1.allocate_string((it1->latitude+" "+it1->longitude+" "+height).c_str()));
            xml_LineString->append_node(xml_pos);
        }
        xml_CellSpaceBoundary->append_attribute(doc1.allocate_attribute("gml:id",it->gml_id.c_str()));
        xml_Geometry2D->append_node(xml_LineString);
        xml_cellSpaceBoundaryGeometry->append_node(xml_Geometry2D);
        xml_CellSpaceBoundary->append_node(xml_duality);
        xml_CellSpaceBoundary->append_node(xml_cellSpaceBoundaryGeometry);
        xml_cellSpaceBoundaryMember->append_node(xml_CellSpaceBoundary);

        xml_PrimalSpaceFeatures->append_node(xml_cellSpaceBoundaryMember);
    }//cellspaceboundary

    for(auto it : State_vector){
        xml_node<>* xml_stateMember=doc1.allocate_node(rapidxml::node_element, "stateMember");
        xml_node<>* xml_State=doc1.allocate_node(rapidxml::node_element, "State");
        xml_State->append_attribute(doc1.allocate_attribute("gml:id",doc1.allocate_string(it->gml_id.c_str())));
        xml_node<>* xml_State_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_State_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

        if(it->duaility!=NULL) {
            xml_node < > *xml_duality = doc1.allocate_node(rapidxml::node_element, "duality");
            xml_duality->append_attribute(doc1.allocate_attribute("xlink:href", doc1.allocate_string(("#" + it->duaility->gml_id).c_str())));
            xml_State->append_node(xml_duality);
        }
        for(auto it1 : it->connects){
            xml_node < > *xml_connects = doc1.allocate_node(rapidxml::node_element, "connects");
            xml_connects->append_attribute(doc1.allocate_attribute("xlink:href", doc1.allocate_string(("#" + it1->gml_id).c_str())));
            xml_State->append_node(xml_connects);
        }
        xml_node<>* xml_geometry = doc1.allocate_node(rapidxml::node_element, "geometry");
        xml_node<>* xml_gml_Point = doc1.allocate_node(rapidxml::node_element, "gml:Point");
        xml_node<>* xml_gml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");

        xml_gml_pos->value(doc1.allocate_string((it->pos->latitude+" "+it->pos->longitude+" "+height).c_str()));
        xml_gml_Point->append_node(xml_gml_pos);
        xml_geometry->append_node(xml_gml_Point);
        xml_State->append_node(xml_geometry);
        xml_stateMember->append_node(xml_State);
        xml_nodes->append_node(xml_stateMember);
    }//State
    for(auto it : Transition_vector){
        xml_node<>* xml_transitionMember=doc1.allocate_node(rapidxml::node_element, "transitionMember");
        xml_node<>* xml_Transition=doc1.allocate_node(rapidxml::node_element, "Transition");
        xml_Transition->append_attribute(doc1.allocate_attribute("gml:id",doc1.allocate_string(it->gml_id.c_str())));
        xml_node<>* xml_Transition_bound = doc1.allocate_node(rapidxml::node_element, "gml:boundedBy");
        xml_Transition_bound->append_attribute(doc1.allocate_attribute("xsi:nil", boundedby_value.c_str()));

        xml_node<>* xml_geometry = doc1.allocate_node(rapidxml::node_element, "geometry");
        xml_node<>* xml_LineString = doc1.allocate_node(rapidxml::node_element, "gml:LineString");
        xml_node<>* xml_gml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
        if(it->duaility!=NULL) {
            xml_node < > *xml_duality = doc1.allocate_node(rapidxml::node_element, "duality");
            xml_duality->append_attribute(doc1.allocate_attribute("xlink:href", doc1.allocate_string(("#" + it->duaility->gml_id).c_str())));
            xml_Transition->append_node(xml_duality);
        }
        for(auto it1 : it->connects){
            xml_node < > *xml_connects = doc1.allocate_node(rapidxml::node_element, "connects");
            xml_connects->append_attribute(doc1.allocate_attribute("xlink:href", doc1.allocate_string(("#" + it1->gml_id).c_str())));
            xml_Transition->append_node(xml_connects);
        }
        for(auto it1 : it->pos_vector){
            xml_node<>* xml_pos = doc1.allocate_node(rapidxml::node_element, "gml:pos");
            xml_pos->append_attribute(doc1.allocate_attribute("srcDimension",srsDimension.c_str()));
            xml_pos->value(doc1.allocate_string((it1->latitude+" "+it1->longitude+" "+height).c_str()));
            xml_LineString->append_node(xml_pos);
        }

        xml_geometry->append_node(xml_LineString);
        xml_Transition->append_node(xml_geometry);
        xml_transitionMember->append_node(xml_Transition);
        xml_edges->append_node(xml_transitionMember);
    }//Transition
    xml_multiLayeredGraph->append_node(xml_MultiLayeredGraph);
    xml_primalSpaceFeatures->append_node(xml_PrimalSpaceFeatures);
    root->append_node(xml_primalSpaceFeatures);
    root->append_node(xml_multiLayeredGraph);
    doc1.append_node(root);

    ofstream file_stored("../OSM2IndoorGML.xml");
    file_stored << doc1;
    file_stored.close();
    doc1.clear();
    cout<<"SUCESS";


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