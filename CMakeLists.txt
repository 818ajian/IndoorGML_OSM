cmake_minimum_required(VERSION 3.12)
project(IndoorGML2OSM)

set(CMAKE_CXX_STANDARD 14)
# include_directories("C:/Users/byeonggon/rapidxml-1.13")
add_executable(CONVERTER  rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp IndoorGML_Reader/IndoorGML_Reader.cpp IndoorGML_Reader/IndoorGML_Reader.h OSM_Reader/OSM_Reader.cpp OSM_Reader/OSM_Reader.h OSM_Writer/OSM_Writer.cpp OSM_Writer/OSM_Writer.h IndoorGML_Writer/IndoorGML_Writer.cpp IndoorGML_Writer/IndoorGML_Writer.h CONVERTER.cpp)
include_directories("../IndoorGML2OSM")