# IndoorGML_OSM

Converter OSM Data<->IndoorGML Data


## Author
Byeonggon Kim(byeonggon@pnu.edu), Pusan National University

## Dependencies
- InFactory (https://github.com/STEMLab/InFactory) : To generate IndoorGML
- OSM (https://wiki.openstreetmap.org/wiki/Main_Page) : OSM wiki

## Building

Dependencies should be pre-installed in your environment.
Linux
```
mkdir build
cd build
cmake ..
make
```
Windows
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```
## Usage

```
CONVERTER [inputfile_type] [outputfile_type] [inputfile_path] [outputfile_path]
```

For example,

```
./CONVERTER OSM IndoorGML ./TEST.osm ./TEST.gml
./CONVERTER IndoorGML OSM ./TEST.gml ./TEST.osm
```


## Development

