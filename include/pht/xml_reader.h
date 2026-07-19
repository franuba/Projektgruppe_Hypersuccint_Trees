#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_

#include "unordered_tree.h"
#include "pht_export.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

namespace pht {
    /**
     * This class implements the reading and conversion of XML-File into UnorderedTree objects. 
     */
    class PHT_EXPORT XMLReader {
    public:
        /**
         * Read and convert the given file into an UnorderedTree. 
         * @param path The path to the file to read. 
         * @return std::shared_ptr<UnorderedTree<std::string>> The resulting tree. 
         */
        static std::shared_ptr<UnorderedTree<std::string>> read(const std::string& path);

        /**
         * A more convenient version of read.  
         * @param name The name of the file to read. The file has to be in project-directory/resources, and the project-directory must be named Projektgruppe_Hypersuccint_Trees or ProjektSuccinctTrees. 
         * @return std::shared_ptr<UnorderedTree<std::string>> The resulting tree. 
         */
        static std::shared_ptr<UnorderedTree<std::string>> readByName(const std::string& name);
    };
}
#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_
