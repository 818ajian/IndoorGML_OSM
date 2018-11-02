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
T1 matching_id(std::vector<T1> &a,string b);

int OSM_NODE_ID=-1;
int OSM_WAY_ID=-30000;
int OSM_RELATION_ID=-60000;
class IC{
public:
    string gml_id;
    int osm_id;
    string Description;
    IC* duality = NULL;
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
    vector <IC*>IC_vector;
    cout << "Indoorgml to OSM..."<<endl ;
    xml_document<> doc;//input
    xml_document<> doc1;//output
    xml_node<> * root_node;
    ifstream theFile ("C:\\Users\\byeonggon\\CLionProjects\\IndoorGML2OSM\\313_4F_2D.gml");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]);
    root_node = doc.first_node("IndoorFeatures");

    xml_node<> * PrimalSpaceFeatures = root_node->first_node("primalSpaceFeatures")->first_node("PrimalSpaceFeatures");
    xml_node<> * SpaceLayer = root_node->first_node("multiLayeredGraph")->first_node("MultiLayeredGraph")->first_node("spaceLayers")->first_node("spaceLayerMember")->first_node("SpaceLayer");
    xml_node<> * nodes = SpaceLayer->first_node("nodes");
    xml_node<> * edges = SpaceLayer->first_node("edges");

    for (xml_node<> * cellSpaceMember = PrimalSpaceFeatures->first_node("cellSpaceMember"); cellSpaceMember; cellSpaceMember = cellSpaceMember->next_sibling("cellSpaceMember")) {
        CellSpace *cellSpace_input=new CellSpace();
        xml_node<> *CellSpace = cellSpaceMember->first_node("CellSpace");
        xml_node<> *Description = CellSpace->first_node("gml:description");
        xml_node<> *name = CellSpace->first_node("gml:name");
        cellSpace_input->gml_id=CellSpace->first_attribute("gml:id")->value();
        cellSpace_input->name=trim(name->value());
        cellSpace_input->Description=trim(Description->value());
        cellSpace_input->osm_id=OSM_WAY_ID--;
        xml_node<> *LinearRing = CellSpace->first_node("cellSpaceGeometry")->first_node("Geometry2D")->first_node("gml:Polygon")->first_node("gml:exterior")->first_node("gml:LinearRing");
        for(xml_node<>*gml_pos=LinearRing->first_node("gml:pos"); gml_pos; gml_pos= gml_pos->next_sibling("gml:pos")){
            Pos *pos_input=new Pos();
            string erase_space = trim(gml_pos->value());
            std::vector<std::string> splittedStrings = split(erase_space, ' ');
            pos_input->latitude=splittedStrings[0];
            pos_input->longitude=splittedStrings[1];
            pos_input->height=splittedStrings[2];
            pos_input->osm_id=OSM_NODE_ID--;
            cellSpace_input->pos_vector.push_back(pos_input);
            node_vector.push_back(pos_input);
        }
        CellSpace_vector.push_back(cellSpace_input);
        IC_vector.push_back(cellSpace_input);
    }//CellSpace

    for (xml_node<> * cellSpaceBoundaryMember = PrimalSpaceFeatures->first_node("cellSpaceBoundaryMember"); cellSpaceBoundaryMember; cellSpaceBoundaryMember = cellSpaceBoundaryMember->next_sibling("cellSpaceBoundaryMember")) {

        CellSpaceBoundary *CellSpaceBoundary_input=new CellSpaceBoundary();
        xml_node<> *CellSpaceBoundary = cellSpaceBoundaryMember->first_node("CellSpaceBoundary");
        CellSpaceBoundary_input->osm_id=OSM_WAY_ID--;
        CellSpaceBoundary_input->gml_id=CellSpaceBoundary->first_attribute("gml:id")->value();
        xml_node<> *LineString = CellSpaceBoundary->first_node("cellSpaceBoundaryGeometry")->first_node("geometry2D")->first_node("gml:LineString");
        for(xml_node<>*gml_pos=LineString->first_node("gml:pos"); gml_pos; gml_pos= gml_pos->next_sibling("gml:pos")){
            Pos *pos_input=new Pos();
            string erase_space = trim(gml_pos->value());
            std::vector<std::string> splittedStrings = split(erase_space, ' ');
            pos_input->latitude=splittedStrings[0];
            pos_input->longitude=splittedStrings[1];
            pos_input->height=splittedStrings[2];
            pos_input->osm_id=OSM_NODE_ID--;
            CellSpaceBoundary_input->pos_vector.push_back(pos_input);
            node_vector.push_back(pos_input);
        }
        CellSpaceBoundary_vector.push_back(CellSpaceBoundary_input);
        IC_vector.push_back(CellSpaceBoundary_input);
    }//CellSpaceBoundary

    for (xml_node<> * stateMember = nodes->first_node("stateMember"); stateMember; stateMember = stateMember->next_sibling("stateMember")) {

        State *State_input=new State();

        xml_node<> *State = stateMember->first_node("State");
        State_input->gml_id=State->first_attribute("gml:id")->value();
        xml_node<> *Point = State->first_node("geometry")->first_node("gml:Point")->first_node("gml:pos");
        Pos *pos_input=new Pos();
        string erase_space = trim(Point->value());
        std::vector<std::string> splittedStrings = split(erase_space, ' ');
        pos_input->latitude=splittedStrings[0];
        pos_input->longitude=splittedStrings[1];
        pos_input->height=splittedStrings[2];
        State_input->pos=pos_input;
        pos_input->osm_id=OSM_NODE_ID--;
        State_input->osm_id=pos_input->osm_id;
        node_vector.push_back(pos_input);
        State_vector.push_back(State_input);
        IC_vector.push_back(State_input);
    }//State


    for (xml_node<> * transitionMember = edges->first_node("transitionMember"); transitionMember; transitionMember = transitionMember->next_sibling("transitionMember")) {

        Transition *Transition_input=new Transition();
        xml_node<> *Transition = transitionMember->first_node("Transition");
        Transition_input->osm_id=OSM_WAY_ID--;
        Transition_input->gml_id=Transition->first_attribute("gml:id")->value();
        xml_node<> *LineString = Transition->first_node("geometry")->first_node("gml:LineString");

        for(xml_node<>*gml_pos=LineString->first_node("gml:pos"); gml_pos; gml_pos= gml_pos->next_sibling("gml:pos")){
            Pos *pos_input=new Pos();
            string erase_space = trim(gml_pos->value());
            std::vector<std::string> splittedStrings = split(erase_space, ' ');
            pos_input->latitude=splittedStrings[0];
            pos_input->longitude=splittedStrings[1];
            pos_input->height=splittedStrings[2];
            pos_input->osm_id=OSM_NODE_ID--;
            Transition_input->pos_vector.push_back(pos_input);
            node_vector.push_back(pos_input);
        }
        Transition_vector.push_back(Transition_input);
        IC_vector.push_back(Transition_input);
    }//Transition

    for (xml_node<> * stateMember = nodes->first_node("stateMember"); stateMember; stateMember = stateMember->next_sibling("stateMember")) {
        xml_node<> *xml_State = stateMember->first_node("State");
        xml_node<> *xml_duality = xml_State->first_node("duality");
        xml_node<> *xml_connects = xml_State->first_node("connects");
        if(xml_duality!=NULL){
            string duality_id;
            duality_id=xml_duality->first_attribute("xlink:href")->value();
            duality_id=duality_id.substr(1,duality_id.length());
            matching_id(IC_vector,xml_State->first_attribute("gml:id")->value())->duality=matching_id(IC_vector,duality_id);
            matching_id(IC_vector,duality_id)->duality=matching_id(IC_vector,xml_State->first_attribute("gml:id")->value());
        }
        if(xml_connects!=NULL){
            for(xml_node<> *xml_connects = xml_State->first_node("connects");xml_connects;xml_connects=xml_connects->next_sibling("connects")) {
                string connects_id;
                connects_id = xml_connects->first_attribute("xlink:href")->value();
                connects_id = connects_id.substr(1, connects_id.length());
                matching_id(IC_vector, xml_State->first_attribute("gml:id")->value())->connects.push_back(matching_id(IC_vector, connects_id));
                matching_id(IC_vector, connects_id)->connects.push_back(matching_id(IC_vector, xml_State->first_attribute("gml:id")->value()));
            }
        }
    }//State duality<->connects

    for (xml_node<> * transitionMember = edges->first_node("transitionMember"); transitionMember; transitionMember = transitionMember->next_sibling("transitionMember")) {
        xml_node<> *Transition = transitionMember->first_node("Transition");
        xml_node<> *xml_duality = Transition->first_node("duality");
        xml_node<> *LineString = Transition->first_node("geometry")->first_node("gml:LineString");
        xml_node<> *xml_connects = Transition->first_node("connects");
        if(xml_duality!=NULL){
            string duality_id;
            duality_id=xml_duality->first_attribute("xlink:href")->value();
            duality_id=duality_id.substr(1,duality_id.length());
            matching_id(IC_vector,Transition->first_attribute("gml:id")->value())->duality=matching_id(IC_vector,duality_id);
            matching_id(IC_vector,duality_id)->duality=matching_id(IC_vector,Transition->first_attribute("gml:id")->value());
        }
    }//Transition duality
    doc.clear();
//
//
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

    for(auto iter = node_vector.begin(); iter!=node_vector.end(); ++iter){
        xml_node<> *node  = doc1.allocate_node(rapidxml::node_element, "node");
        node->append_attribute(doc1.allocate_attribute("action", "modify"));
        node->append_attribute(doc1.allocate_attribute("visible", "true"));
        node->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->osm_id).c_str())));
        node->append_attribute(doc1.allocate_attribute("lat", doc1.allocate_string(((*iter)->latitude).c_str())));
        node->append_attribute(doc1.allocate_attribute("lon", doc1.allocate_string(((*iter)->longitude).c_str())));
        root->append_node(node);
    }//append node


    for(auto iter=CellSpace_vector.begin();iter!=CellSpace_vector.end();++iter){
        xml_node<> *way  = doc1.allocate_node(rapidxml::node_element, "way");
        way->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->osm_id).c_str())));
        way->append_attribute(doc1.allocate_attribute("action", "modify"));
        way->append_attribute(doc1.allocate_attribute("visible", "true"));
        for(int i=0;i < (*iter)->pos_vector.size(); i++) {
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->pos_vector[i]->osm_id).c_str())));
            way->append_node(nd);
        }
        xml_node<> *tag = doc1.allocate_node(rapidxml::node_element, "tag");
        tag->append_attribute(doc1.allocate_attribute("k", "name"));
        tag->append_attribute(doc1.allocate_attribute("v", doc1.allocate_string(((*iter)->name).c_str())));
        way->append_node(tag);
        std::vector<std::string> splittedStrings = split((*iter)->Description, ';');

        for (auto it : splittedStrings) {
            std::vector<std::string> token = split(it, '=');
            xml_node<> *tag = doc1.allocate_node(rapidxml::node_element, "tag");
            tag->append_attribute(doc1.allocate_attribute("k",  doc1.allocate_string(token[0].c_str())));
            tag->append_attribute(doc1.allocate_attribute("v", doc1.allocate_string((token[1].c_str()))));
            way->append_node(tag);
        }
        root->append_node(way);
    }//append Cellspace

    for(auto iter=CellSpaceBoundary_vector.begin();iter!=CellSpaceBoundary_vector.end();++iter){
        xml_node<> *way  = doc1.allocate_node(rapidxml::node_element, "way");
        way->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->osm_id).c_str())));
        way->append_attribute(doc1.allocate_attribute("action", "modify"));
        way->append_attribute(doc1.allocate_attribute("visible", "true"));
        for(int i=0;i < (*iter)->pos_vector.size(); i++) {
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->pos_vector[i]->osm_id).c_str())));
            way->append_node(nd);
        }
        root->append_node(way);
    }//append Cellspace

    for(auto iter=Transition_vector.begin();iter!=Transition_vector.end();++iter){
        xml_node<> *way  = doc1.allocate_node(rapidxml::node_element, "way");
        way->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->osm_id).c_str())));
        way->append_attribute(doc1.allocate_attribute("action", "modify"));
        way->append_attribute(doc1.allocate_attribute("visible", "true"));
        for(int i=0;i < (*iter)->pos_vector.size(); i++) {
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->pos_vector[i]->osm_id).c_str())));
            way->append_node(nd);
        }
        root->append_node(way);
    }//append Transition


    xml_node<> *relation_cellspace  = doc1.allocate_node(rapidxml::node_element, "relation");
    relation_cellspace->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
    relation_cellspace->append_attribute(doc1.allocate_attribute("action", "modify"));
    relation_cellspace->append_attribute(doc1.allocate_attribute("visible", "true"));

    for(auto it:CellSpace_vector){
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "way"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
        relation_cellspace->append_node(member);
    }
    xml_node<> *tag = doc1.allocate_node(rapidxml::node_element, "tag");
    tag->append_attribute(doc1.allocate_attribute("k",  "type"));
    tag->append_attribute(doc1.allocate_attribute("v", "CellSpace"));
    relation_cellspace->append_node(tag);
    root->append_node(relation_cellspace);//Cellspace entity

    xml_node<> *relation_cellspaceboundary  = doc1.allocate_node(rapidxml::node_element, "relation");
    relation_cellspaceboundary->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
    relation_cellspaceboundary->append_attribute(doc1.allocate_attribute("action", "modify"));
    relation_cellspaceboundary->append_attribute(doc1.allocate_attribute("visible", "true"));
    for(auto it:CellSpaceBoundary_vector){
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "way"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
        relation_cellspaceboundary->append_node(member);
    }
    tag = doc1.allocate_node(rapidxml::node_element, "tag");
    tag->append_attribute(doc1.allocate_attribute("k",  "type"));
    tag->append_attribute(doc1.allocate_attribute("v", "CellSpaceBoundary"));
    relation_cellspaceboundary->append_node(tag);
    root->append_node(relation_cellspaceboundary);//CellspaceBoundary entity

    xml_node<> *relation_state  = doc1.allocate_node(rapidxml::node_element, "relation");
    relation_state->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
    relation_state->append_attribute(doc1.allocate_attribute("action", "modify"));
    relation_state->append_attribute(doc1.allocate_attribute("visible", "true"));
    for(auto it:State_vector){
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "node"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
        relation_state->append_node(member);
    }
    tag = doc1.allocate_node(rapidxml::node_element, "tag");
    tag->append_attribute(doc1.allocate_attribute("k",  "type"));
    tag->append_attribute(doc1.allocate_attribute("v", "State"));
    relation_state->append_node(tag);
    root->append_node(relation_state);//State entity

    xml_node<> *relation_transition  = doc1.allocate_node(rapidxml::node_element, "relation");
    relation_transition->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
    relation_transition->append_attribute(doc1.allocate_attribute("action", "modify"));
    relation_transition->append_attribute(doc1.allocate_attribute("visible", "true"));
    for(auto it:Transition_vector){
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "way"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
        relation_transition->append_node(member);
    }
    tag = doc1.allocate_node(rapidxml::node_element, "tag");
    tag->append_attribute(doc1.allocate_attribute("k",  "type"));
    tag->append_attribute(doc1.allocate_attribute("v", "Transition"));
    relation_transition->append_node(tag);
    root->append_node(relation_transition);//State entity

    for(auto it:State_vector){
        if(it->duality==NULL)continue;
        xml_node<> *relation = doc1.allocate_node(rapidxml::node_element, "relation");
        relation->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
        relation->append_attribute(doc1.allocate_attribute("action", "modify"));
        relation->append_attribute(doc1.allocate_attribute("visible", "true"));
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "node"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
        member->append_attribute(doc1.allocate_attribute("role", "State"));
        relation->append_node(member);
        member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "way"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->duality->osm_id).c_str())));
        member->append_attribute(doc1.allocate_attribute("role", "CellSpace"));
        relation->append_node(member);
        tag = doc1.allocate_node(rapidxml::node_element, "tag");
        tag->append_attribute(doc1.allocate_attribute("k",  "type"));
        tag->append_attribute(doc1.allocate_attribute("v", "duality"));
        relation->append_node(tag);
        root->append_node(relation);
    }

    for(auto it:Transition_vector){
        if(it->duality==NULL)continue;
        xml_node<> *relation = doc1.allocate_node(rapidxml::node_element, "relation");
        relation->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
        relation->append_attribute(doc1.allocate_attribute("action", "modify"));
        relation->append_attribute(doc1.allocate_attribute("visible", "true"));
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", "way"));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
        member->append_attribute(doc1.allocate_attribute("role", "Transition"));
        xml_node<> * member_1  = doc1.allocate_node(rapidxml::node_element, "member");
        member_1->append_attribute(doc1.allocate_attribute("type", "way"));
        member_1->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->duality->osm_id).c_str())));
        member_1->append_attribute(doc1.allocate_attribute("role", "CellSpaceBoundary"));
        relation->append_node(member);
        relation->append_node(member_1);
        tag = doc1.allocate_node(rapidxml::node_element, "tag");
        tag->append_attribute(doc1.allocate_attribute("k",  "type"));
        tag->append_attribute(doc1.allocate_attribute("v", "duality"));
        relation->append_node(tag);
        root->append_node(relation);
    }//Transition<->CellSpaceBoundary duality

    for(auto it:State_vector){
        for(auto it1:it->connects) {
            xml_node<> *relation = doc1.allocate_node(rapidxml::node_element, "relation");
            relation->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(OSM_RELATION_ID--).c_str())));
            relation->append_attribute(doc1.allocate_attribute("action", "modify"));
            relation->append_attribute(doc1.allocate_attribute("visible", "true"));
            xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
            member->append_attribute(doc1.allocate_attribute("type", "node"));
            member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it->osm_id).c_str())));
            member->append_attribute(doc1.allocate_attribute("role", "State"));
            xml_node<> *member_1 = doc1.allocate_node(rapidxml::node_element, "member");
            member_1->append_attribute(doc1.allocate_attribute("type", "way"));
            member_1->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string(it1->osm_id).c_str())));
            member_1->append_attribute(doc1.allocate_attribute("role", "Transition"));
            relation->append_node(member);
            relation->append_node(member_1);
            tag = doc1.allocate_node(rapidxml::node_element, "tag");
            tag->append_attribute(doc1.allocate_attribute("k", "type"));
            tag->append_attribute(doc1.allocate_attribute("v", "connects"));
            relation->append_node(tag);
            root->append_node(relation);
        }
    }//State<->Transition Connect

    ofstream file_stored("../IndoorGML2OSM.xml");
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
        for(int i=0;i<item.length();i++) {
            if (item.at(i) == '\n') {
                item.at(i)=' ';
            }
        }
        item=trim(item);
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
T1 matching_id(std::vector<T1> &a,string b){
    T1 result;
    for(auto iter=a.begin();iter!=a.end();++iter){
        if((*iter)->gml_id==b){
            result=*iter;
        }
    }
    return result;
}