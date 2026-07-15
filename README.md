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

#### Windows
This project was originally developed and tested using:
 - Win10 x64
 - MSVC 19.27
 - CMake 3.20
 - Python 3.9
 - doxygen (optional)

#### Linux
For Linux, the following tools and libraries are required:
 - GCC/Clang with C++17 support
 - CMake 3.17 or higher
 - Python 3.8+ (for Python bindings)
 - doxygen (optional, for documentation)

### Dependencies
All external dependencies are included as Git submodules:
 - googletest
 - irrxml
 - succinct\_bv
 - thread-pool 2.0.0

### Building

#### Linux
To build on Linux, execute the following commands:
```bash
git clone https://github.com/franuba/Projektgruppe_Hypersuccint_Trees
cd Projektgruppe_Hypersuccint_Trees
git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

**Note:** The example service executable will be located at `./example_service/example_service` (no `.exe` extension).

#### Windows (Original)
To build on Windows, execute the following commands:
```bash
git clone https://github.com/ChristopherPack/Projektgruppe_Hypersuccint_Trees
cd Projektgruppe_Hypersuccint_Trees
git submodule update --init --recursive
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -H. -B./build -G "Visual Studio 16 2019" -T host=x64 -A x64
cmake --build ./build --config Release
```

### Post-Build Setup
After building, create a `resources` folder in the project root directory and add some XML files:
```bash
mkdir resources
# Add your XML test files to the resources folder
# Modify the paths in example_service/example_service.cpp if necessary
```

## Usage
### Documentation
Execute `doxygen doxyfile` to generate documentation of the C++ Code.
It can then be found at `build/docs/index.html`.

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

For a more complete example, see `example_service/example_service.cpp`.

To execute the C++ example:
- **Linux**: `./build/example_service/example_service`
- **Windows**: `./build/example_service/Release/example_service.exe`

### Python
Copy the compiled Python module (`pht_hst.*.so` on Linux or `pht_hst.cp39-win_amd64.pyd` on Windows) to your Python directory, then use as follows:
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

## Troubleshooting

### Linux Build Issues
- **Missing dependencies**: Ensure GCC/Clang and build essentials are installed:
  ```bash
  sudo apt-get install build-essential cmake python3-dev
  ```
- **Submodule errors**: If submodules fail to update, try:
  ```bash
  git submodule update --init --recursive --depth=1
  ```
- **Python extension build failures**: Install Python development headers:
  ```bash
  sudo apt-get install python3-dev
  ```
