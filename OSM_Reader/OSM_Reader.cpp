//
// Created by byeonggon on 2018-11-02.
//
#include "OSM_Reader.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "string.h"

using namespace std;
namespace OSM{
    template <class T1>
    T1 matching_id(std::vector<T1> &a, int b) {
        T1 result;
        for(auto iter=a.begin();iter!=a.end();++iter){
            if((*iter)->osm_id==b){
                result=*iter;
            }
        }
        return result;
    }
    std::vector<CONVERTER::IC*> Read(std::string PATH){
        vector <CONVERTER::CellSpace*> CellSpace_vector;
        vector <CONVERTER::CellSpaceBoundary*>CellSpaceBoundary_vector;
        vector <CONVERTER::State*>State_vector;
        vector <CONVERTER::Transition*>Transition_vector;
        vector <CONVERTER::Pos*> node_vector;
        vector <CONVERTER::IC*> IC_vector;
        cout << "OSM to IndoorGML..."<<endl ;
        xml_document<> doc;//input
        xml_node<> * root_node;
        ifstream theFile (PATH);
        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        doc.parse<0>(&buffer[0]);
        root_node = doc.first_node("osm");
        for (xml_node<> * xml_node = root_node->first_node("node"); xml_node; xml_node = xml_node->next_sibling("node")) {
            CONVERTER::Pos *node_input = new CONVERTER::Pos();
            node_input->osm_id=atoi(xml_node->first_attribute("id")->value());
            node_input->latitude=xml_node->first_attribute("lat")->value();
            node_input->longitude=xml_node->first_attribute("lon")->value();
            node_vector.push_back(node_input);
            IC_vector.push_back(node_input);
        }//node input

        for (xml_node<> * xml_relation = root_node->first_node("relation"); xml_relation; xml_relation = xml_relation->next_sibling("relation")) {
            xml_node<> * xml_tag = xml_relation->first_node("tag");
            if (strcmp(xml_tag->first_attribute("v")->value(),"CellSpace") == 0){
                for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                    CONVERTER::CellSpace *cellspace_input = new CONVERTER::CellSpace();
                    cellspace_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                    cellspace_input->gml_id="C"+to_string(CellSpace_ID++);
                    CellSpace_vector.push_back(cellspace_input);
                    IC_vector.push_back(cellspace_input);
                }
            }
            else if (strcmp(xml_tag->first_attribute("v")->value(), "CellSpaceBoundary") == 0) {
                for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                    CONVERTER::CellSpaceBoundary *cellspaceboundary_input = new CONVERTER::CellSpaceBoundary();
                    cellspaceboundary_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                    cellspaceboundary_input->gml_id="B"+to_string(CellSpaceBoundary_ID++);
                    CellSpaceBoundary_vector.push_back(cellspaceboundary_input);
                    IC_vector.push_back(cellspaceboundary_input);
                }
            }
            else if (strcmp(xml_tag->first_attribute("v")->value(), "State") == 0) {
                for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                    CONVERTER::State *state_input = new CONVERTER::State();
                    state_input->pos=matching_id(node_vector,atoi(xml_member->first_attribute("ref")->value()));
                    state_input->osm_id=state_input->pos->osm_id;
                    state_input->gml_id="S"+to_string(State_id++);
                    State_vector.push_back(state_input);
                    IC_vector.push_back(state_input);
                }
            }
            else if (strcmp(xml_tag->first_attribute("v")->value(), "Transition") == 0) {
                for(xml_node<> * xml_member = xml_relation->first_node("member"); xml_member; xml_member = xml_member->next_sibling("member")){
                    CONVERTER::Transition *transition_input=new CONVERTER::Transition();
                    transition_input->osm_id=atoi(xml_member->first_attribute("ref")->value());
                    transition_input->gml_id="T"+to_string(Transition_id++);
                    Transition_vector.push_back(transition_input);
                    IC_vector.push_back(transition_input);
                }
            }
        }//cellspace, cellspaceboundary,state,transition push


        for (xml_node<> * xml_way = root_node->first_node("way"); xml_way; xml_way = xml_way->next_sibling("way")) {
            if(matching_id(CellSpace_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
                CONVERTER::CellSpace * CellSpace_Pointer=matching_id(CellSpace_vector,atoi(xml_way->first_attribute("id")->value()));
                for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                    CellSpace_Pointer->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
                }
                for(xml_node<>*xml_tag=xml_way->first_node("tag");xml_tag;xml_tag=xml_tag->next_sibling("tag")){
                    if(strcmp(xml_tag->first_attribute("k")->value(),"name")==0){
                        CellSpace_Pointer->name=xml_tag->first_attribute("v")->value();
                        continue;
                    }
                    CellSpace_Pointer->Description.append(xml_tag->first_attribute("k")->value());
                    CellSpace_Pointer->Description.append("=");
                    CellSpace_Pointer->Description.append(xml_tag->first_attribute("v")->value());
                    CellSpace_Pointer->Description.append(";");
                }
            }//Cellspace_way
            if(matching_id(CellSpaceBoundary_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
                CONVERTER::CellSpaceBoundary * CellSpaceBoundary_Pointer=matching_id(CellSpaceBoundary_vector,atoi(xml_way->first_attribute("id")->value()));
                for(xml_node<>*xml_nd=xml_way->first_node("nd");xml_nd;xml_nd=xml_nd->next_sibling("nd")){
                    CellSpaceBoundary_Pointer->pos_vector.push_back(matching_id(node_vector,atoi(xml_nd->first_attribute("ref")->value())));
                }
            }//CellSpaceBoundary_way
            if(matching_id(Transition_vector,atoi(xml_way->first_attribute("id")->value()))!=0){
                CONVERTER::Transition * Transition_Pointer=matching_id(Transition_vector,atoi(xml_way->first_attribute("id")->value()));
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
                matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()))->duality=matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value()));
                matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value()))->duality=matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()));
                //cout<<matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()))->gml_id<<" "<<matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()))->duality->gml_id<<endl;
            }
            if(strcmp(xml_tag->first_attribute("v")->value(),"connects")==0){
                xml_node<> *xml_member=xml_relation->first_node("member");
                xml_node<> *xml_member_1=xml_member->next_sibling("member");
                matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value()))->connects.push_back(matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value())));
                matching_id(IC_vector,atoi(xml_member_1->first_attribute("ref")->value()))->connects.push_back(matching_id(IC_vector,atoi(xml_member->first_attribute("ref")->value())));
            }
        }

        doc.clear();
        return IC_vector;
    }
}
