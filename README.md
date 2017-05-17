# text-processor
## General information
Tool for extracting useful information from official documents (agreements). Only documents written in russian language are supported.
## Requirements
Only GNU/Linux systems are supported.
In order to build this tool you will need:
* cmake >= 2.8
* GNU Flex
* GNU Bison >= 3.0
* Python 2.7 with cython and pymorphy2 modules installed
* icu4c
## Building steps
```
git clone https://github.com/voyagerok/text-processor.git && cd text-processor/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make
```
