cmake_minimum_required(VERSION 3.12)
project(IndoorGML2OSM)

set(CMAKE_CXX_STANDARD 14)
# include_directories("C:/Users/byeonggon/rapidxml-1.13")

add_executable(IndoorGML2OSM main.cpp rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp)
#add_executable(IndoorGML2OSM2 main2.cpp rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp)

include_directories("../IndoorGML2OSM")