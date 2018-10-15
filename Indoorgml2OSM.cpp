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
int NODE_ID=-1;
int WAY_ID=-30000;
int RELATION_ID=-60000;
class Descript{
public:
    string name;
    string value;
};//Description name : value

class Relate{
public:
    int osm_id;//첫번째 관계에 대한 타입
    int indoor_gml_id_to_osm_id;//
    string osm_type;
    string indoor_gml_type;//2번째 관계에 대한 type indoorgml_ID -> OSM_ID로 변경됨
    string indoorgml_id;
};
class Relation{
public:
    vector<Relate*>duality; //Relation에서 duality.
    vector<Relate*>connect; //Relation에서 connect.
};
class Matching{
public:
    string indoorgml_id;//Indoorgml_id
    int osm_id;//osm_id
};//Matching 시켜줌 indoorgml_id <-> osm_id

class Node{
public:
    int id;//Node의 id
    //vector <Descript*> Des;
    string latitude;//위도
    string longitude;//경도
    string height;//높이
    string indoorgml_id;//Node의 indoorgml id
};

class Way{
public:
    vector <Node*> CellSpace; //Cellspace
    vector <Node*> Cellspaceboundary;//Cellspaceboundary
    vector <Node*> Transition;//Transition
    vector <Descript*> Des;//Description
    string name;//gml:id
    string indoorgml_id;//indoorgml_id Cellspace, Cellspace boundary, Transition의 id
    int id;
};

std::vector<std::string> split(std::string& strToSplit, char delimeter);
std::string trim(const std::string& str);
void find_osm_id(vector<Matching*>matching, vector<Relate*>input);

int main(void) {

    vector <Node*> node_vector;
    vector <Way*> way_vector;
    vector <Matching*> matching_vector;

    Relation *relation_vector=new Relation();
    cout << "Parsing IndoorGML..." ;
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

        Descript *des_input = new Descript();
        des_input->name = "Indoorgml";
        des_input->value = "CellSpace";
        way_input->Des.push_back(des_input);

        //cout<<way_input->Des.size()<<endl;
        xml_node<> *cellSpaceGeometry = CellSpace->first_node("cellSpaceGeometry");
        xml_node<> *Geometry2D = cellSpaceGeometry->first_node("Geometry2D");
        xml_node<> *Polygon = Geometry2D->first_node("gml:Polygon");
        xml_node<> *Exterior = Polygon->first_node("gml:exterior");
        xml_node<> *LinearRing = Exterior->first_node("gml:LinearRing");

        way_input->name = trim(CellSpace->first_node("gml:name")->value());
        way_input->indoorgml_id =CellSpace->first_attribute("gml:id")->value();
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
            input->indoorgml_id="Cellspace_pos";
            way_input->CellSpace.push_back(input);
            node_vector.push_back(input);
        }
        way_input->id = WAY_ID--;

        Matching *mat = new Matching();
        mat->indoorgml_id=CellSpace->first_attribute("gml:id")->value();
        mat->osm_id=way_input->id;
        matching_vector.push_back(mat);

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
            input->indoorgml_id="CellSpaceBoundary_pos";
            way_input->Cellspaceboundary.push_back(input);
            node_vector.push_back(input);
        }
        way_input->id = WAY_ID--;

        Descript *des_input = new Descript();
        des_input->name = "Indoorgml";
        des_input->value = "CellSpaceBoundary";
        way_input->Des.push_back(des_input);

        Matching *mat = new Matching();
        mat->indoorgml_id=CellSpaceBoundary->first_attribute("gml:id")->value();
        mat->osm_id=way_input->id;
        matching_vector.push_back(mat);
        way_vector.push_back(way_input);
    }//Cellspaceboundary

    for (xml_node<> * transitionMember = edges->first_node("transitionMember"); transitionMember; transitionMember = transitionMember->next_sibling("transitionMember")) {
        xml_node<> * Transition = transitionMember->first_node("Transition");
        xml_node<> * geometry = Transition->first_node("geometry");
        xml_node<> * LineString = geometry->first_node("gml:LineString");
        xml_node<> * pos =  LineString->first_node("gml:pos");
        xml_node<> * connects = Transition->first_node("connects");
        xml_node<> * duality = Transition->first_node("duality");

        string erase_space = trim(pos->value());
        std::vector<std::string> splittedStrings = split(erase_space, ' ');

        Way * way_input=new Way();
        way_input->indoorgml_id =Transition->first_attribute("gml:id")->value();

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
            input->indoorgml_id="Transition_pos";
            way_input->Transition.push_back(input);
            node_vector.push_back(input);
        }
        way_input->id = WAY_ID--;

        Descript *des_input = new Descript();
        des_input->name = "Indoorgml";
        des_input->value = "Transition";
        way_input->Des.push_back(des_input);

        way_vector.push_back(way_input);


        if(duality!=NULL) {
            Relate *relation_input= new Relate();
            relation_input->osm_id = way_input->id;
            relation_input->indoorgml_id = duality->first_attribute("xlink:href")->value();
            relation_input->indoorgml_id = relation_input->indoorgml_id.substr(1, relation_input->indoorgml_id.length());
            relation_vector->duality.push_back(relation_input);
        }
        for(connects;connects;connects=connects->next_sibling("connects")){
            Relate *relation_input= new Relate();
            relation_input->osm_id = way_input->id;
            relation_input->indoorgml_id = connects->first_attribute("xlink:href")->value();
            relation_input->indoorgml_id = relation_input->indoorgml_id.substr(1, relation_input->indoorgml_id.length());
            relation_vector->connect.push_back(relation_input);
        }

        Matching *mat = new Matching();
        mat->indoorgml_id=Transition->first_attribute("gml:id")->value();
        mat->osm_id=way_input->id;
        matching_vector.push_back(mat);
    }//transition

    for (xml_node<> * stateMember = nodes->first_node("stateMember"); stateMember; stateMember = stateMember->next_sibling("stateMember")) {
        xml_node<> * State = stateMember->first_node("State");
        xml_node<> * geometry = State->first_node("geometry");
        xml_node<> * Point = geometry->first_node("gml:Point");
        xml_node<> * pos =  Point->first_node("gml:pos");
        xml_node<> * duality =  State->first_node("duality");
        xml_node<> * connects =  State->first_node("connects");
        string erase_space = trim(pos->value());
        std::vector<std::string> splittedStrings = split(erase_space, ' ');
        Node* input = new Node();
        for(int i = 0; i < splittedStrings.size() ; i++){
            input->longitude = splittedStrings[1];
            input->latitude = splittedStrings[0];
            input->height = splittedStrings[2];
        }
        input->id = NODE_ID--;
        input->indoorgml_id="State";
        if(duality!=NULL) {
            Relate *relation_input= new Relate();
            relation_input->osm_id = input->id;
            relation_input->indoorgml_id = duality->first_attribute("xlink:href")->value();
            relation_input->indoorgml_id = relation_input->indoorgml_id.substr(1, relation_input->indoorgml_id.length());
            relation_vector->duality.push_back(relation_input);
        }
        for(connects;connects;connects=connects->next_sibling("connects")){
            Relate *relation_input= new Relate();
            relation_input->osm_id = input->id;
            relation_input->indoorgml_id = connects->first_attribute("xlink:href")->value();
            relation_input->indoorgml_id = relation_input->indoorgml_id.substr(1, relation_input->indoorgml_id.length());
            relation_vector->connect.push_back(relation_input);
        }
        Matching *mat = new Matching();
        mat->indoorgml_id=State->first_attribute("gml:id")->value();
        mat->osm_id=input->id;
        matching_vector.push_back(mat);
        node_vector.push_back(input);
    }//state

    find_osm_id(matching_vector,relation_vector->connect);
    find_osm_id(matching_vector,relation_vector->duality);


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
        node->append_attribute(doc1.allocate_attribute("IndoorGML_type", doc1.allocate_string(((*iter)->indoorgml_id).c_str())));
        root->append_node(node);
    }

    for(auto iter=way_vector.begin();iter!=way_vector.end();++iter){
        xml_node<> *way  = doc1.allocate_node(rapidxml::node_element, "way");
        way->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string((*iter)->id).c_str())));
        way->append_attribute(doc1.allocate_attribute("action", "modify"));
        way->append_attribute(doc1.allocate_attribute("visible", "true"));
        for(int i=0;i < (*iter)->CellSpace.size(); i++) {
            //if(i==0)way->append_attribute(doc1.allocate_attribute("Indoorgml_Type", "CellSpace"));
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->CellSpace[i]->id).c_str())));
            way->append_node(nd);
        }//Cellspace

        for(int i=0;i < (*iter)->Cellspaceboundary.size(); i++) {
            //if(i==0)way->append_attribute(doc1.allocate_attribute("Indoorgml_Type", "Cellspaceboundary"));
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->Cellspaceboundary[i]->id).c_str())));
            way->append_node(nd);
        }//CellspaceBoundary

        for(int i=0;i < (*iter)->Transition.size(); i++) {
            //if(i==0)way->append_attribute(doc1.allocate_attribute("Indoorgml_Type", "Transition"));
            xml_node<> *nd = doc1.allocate_node(rapidxml::node_element, "nd");
            nd->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->Transition[i]->id).c_str())));
            way->append_node(nd);
        }//Transition

        if((*iter)->name!="") {
            xml_node<> *tag = doc1.allocate_node(rapidxml::node_element, "tag");
            tag->append_attribute(doc1.allocate_attribute("k", "name"));
            tag->append_attribute(doc1.allocate_attribute("v", doc1.allocate_string(((*iter)->name).c_str())));
            way->append_node(tag);
        }

        for(int i=0; i<(*iter)->Des.size();i++){
            xml_node<> *tag  = doc1.allocate_node(rapidxml::node_element, "tag");
            tag->append_attribute(doc1.allocate_attribute("k", doc1.allocate_string(((*iter)->Des[i]->name).c_str())));
            tag->append_attribute(doc1.allocate_attribute("v", doc1.allocate_string(((*iter)->Des[i]->value).c_str())));
            way->append_node(tag);
        }
        root->append_node(way);
    }


    for(auto iter = relation_vector->duality.begin(); iter!=relation_vector->duality.end(); ++iter){
        xml_node<> *relation  = doc1.allocate_node(rapidxml::node_element, "relation");
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");

        xml_node<> *tag = doc1.allocate_node(rapidxml::node_element, "tag");
        tag->append_attribute(doc1.allocate_attribute("k", "type"));
        tag->append_attribute(doc1.allocate_attribute("v", "duality"));


        relation->append_attribute(doc1.allocate_attribute("action", "modify"));
        relation->append_attribute(doc1.allocate_attribute("visible", "true"));
        relation->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(RELATION_ID--).c_str())));

        member->append_attribute(doc1.allocate_attribute("type", doc1.allocate_string((*iter)->osm_type.c_str())));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->osm_id).c_str())));

        relation->append_node(member);
        member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", doc1.allocate_string((*iter)->indoor_gml_type.c_str())));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->indoor_gml_id_to_osm_id).c_str())));

        relation->append_node(member);
        relation->append_node(tag);
        root->append_node(relation);
    }//DUALITY RELATION

    for(auto iter = relation_vector->connect.begin(); iter!=relation_vector->connect.end(); ++iter){
        xml_node<> *relation  = doc1.allocate_node(rapidxml::node_element, "relation");
        xml_node<> *member  = doc1.allocate_node(rapidxml::node_element, "member");
        xml_node<> *tag = doc1.allocate_node(rapidxml::node_element, "tag");

        tag->append_attribute(doc1.allocate_attribute("k", "type"));
        tag->append_attribute(doc1.allocate_attribute("v", "connect"));


        relation->append_attribute(doc1.allocate_attribute("id", doc1.allocate_string(to_string(RELATION_ID--).c_str())));
        relation->append_attribute(doc1.allocate_attribute("action", "modify"));
        relation->append_attribute(doc1.allocate_attribute("visible", "true"));

        member->append_attribute(doc1.allocate_attribute("type", doc1.allocate_string((*iter)->osm_type.c_str())));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->osm_id).c_str())));

        relation->append_node(member);
        member  = doc1.allocate_node(rapidxml::node_element, "member");
        member->append_attribute(doc1.allocate_attribute("type", doc1.allocate_string((*iter)->indoor_gml_type.c_str())));
        member->append_attribute(doc1.allocate_attribute("ref", doc1.allocate_string(to_string((*iter)->indoor_gml_id_to_osm_id).c_str())));

        relation->append_node(member);
        relation->append_node(tag);
        root->append_node(relation);
    }//CONNECT RELATION


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

void find_osm_id(vector<Matching*>matching, vector<Relate*>input){
    for(auto iter=input.begin();iter!=input.end();++iter){
        for(auto iter_1=matching.begin();iter_1!=matching.end();++iter_1){
            if(strcmp((*iter)->indoorgml_id.c_str(),(*iter_1)->indoorgml_id.c_str())==0){
                (*iter)->indoor_gml_id_to_osm_id=(*iter_1)->osm_id;
                if((*iter)->osm_id>-30000)(*iter)->osm_type="node";
                else (*iter)->osm_type="way";
                if((*iter)->indoor_gml_id_to_osm_id>-30000)(*iter)->indoor_gml_type="node";
                else (*iter)->indoor_gml_type="way";
            }
        }
    }
}
