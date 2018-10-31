cmake_minimum_required(VERSION 3.12)
project(IndoorGML2OSM)

set(CMAKE_CXX_STANDARD 14)
# include_directories("C:/Users/byeonggon/rapidxml-1.13")

add_executable(IndoorGML2OSM  rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp IndoorGML2OSM.cpp)
add_executable(OSM2Indoorgml OSM2IndoorGML.cpp rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp)

include_directories("../IndoorGML2OSM")