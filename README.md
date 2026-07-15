# PHT - Projektgruppe Hypersuccinct Trees

Linux port (WIP) of ChristopherPack's Windows implementation.

## Features
 - Conversion from XML-Files to Hypersuccinct Trees
 - Queries in O(1) on Hypersuccint Trees
 - Saving/Loading of Hypersuccinct Trees to/from file
 - Python integration
 - A simple GUI in Python

## Building
### Prerequisites
This project was originally developed and tested using:
 - Win10 x64
 - MSVC 19.27
 - CMake 3.20
 - Python 3.9
 - doxygen (optional)

Used libraries (see .gitmodules and extern):
 - googletest
 - irrxml
 - succinct_bv
 - thread-pool 2.0.0

### Building
To build execute the following commands:
```bash
git clone https://github.com/ChristopherPack/Projektgruppe_Hypersuccint_Trees
git submodule update --remote --merge
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -H. -B./build -G "Visual Studio 16 2019" -T host=x64 -A x64
cmake --build ./build --config Release
```
Then create a resources folder in the project root directory and add some xml-files (and modify the paths in example_service/example_service.cpp).

## Usage
### Documenation
Execute `doxygen doxyfile` to generate documentation of the C++ Code.
It can then be found at build/docs/index.html.

### C++
```cpp
#include <memory>

#include "pht/unordered_tree.h"
#include "pht/farzan_munro.h"
#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree_factory.h"

int main() {
	//Convert from XML
	std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName("tree");
    std::unique_ptr<pht::HypersuccinctTree> hst = pht::HypersuccinctTreeFactory::create(tree, false, 0, 0); //Set to true to use huffman encoding

	//Saving/Loading
    HypersuccinctTreeOutput::writeToFile(*hst);
    hst = HypersuccinctTreeOutput::readFromFile("tree.txt");

	//Queries
	pht::HstNode result = tree.__query_name_here__(__query_arguments_here__);
	return 0;
}
```

For a more complete example, see example_service/example_service.cpp.
To execute the C++ example use the following command:
```bash
./build/example_service/Release/example_service.exe
```

### Python
Copy python/pht_hst.cp39-win_amd64.pyd (or similar) to your python directory, then use as follows:
```python
import pht_hst
tree = pht_hst.PyHST(pathValue.get(), False) #Set to True to use huffman encoding
minitree, microtree, node = tree.__query_name_here__(__query_arguments_here__)
```

To execute the Python GUI use the following commands:
```bash
cd python
python gui.py
```
