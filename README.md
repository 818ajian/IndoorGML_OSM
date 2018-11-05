# IndoorGML_OSM
Converter OSM Data<->IndoorGML Data

The following image is available from [TM2IN-Viewer](https://github.com/cocoslime/TM2IN-Viewer).

![](doc/img/180920-tm2in-process.gif)

## Pipeline

![](doc/img/pipeline.png?raw=true)

## Author
Byeonggon Kim(byeonggon@pnu.edu), Pusan National University

## Dependencies
- InFactory (https://github.com/STEMLab/InFactory) : To generate IndoorGML
- OSM (https://wiki.openstreetmap.org/wiki/Main_Page): OSM wiki

## Building

Dependencies should be pre-installed in your environment.

```
mkdir build
cd build
cmake ..
make
```

## Usage

```
CONVERTER [inputfile_type] [outputfile_type] [inputfile_path] [outputfile_path]

```

For example,

```
CONVERTER OSM IndoorGML ./TEST.osm ./TEST.gml
CONVERTER IndoorGML OSM ./TEST.gml ./TEST.osm
```


## Development

