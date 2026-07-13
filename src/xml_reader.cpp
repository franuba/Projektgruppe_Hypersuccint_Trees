#ifdef _WIN32
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif

#include <iostream>
#include <filesystem>
#include <direct.h>
//#define PHT_TEST
#include <irrXML.h>

using namespace std::filesystem;
#include "pht/xml_reader.h"

std::shared_ptr<pht::UnorderedTree<std::string>> pht::XMLReader::read(const std::string& path) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = std::make_shared<pht::UnorderedTree<std::string>>();
    std::shared_ptr<pht::Node<std::string>> current = nullptr;

    std::unique_ptr<irr::io::IrrXMLReader> reader = std::unique_ptr<irr::io::IrrXMLReader>(irr::io::createIrrXMLReader(path.c_str()));
    if(!reader || !reader->read())
        throw std::runtime_error("Failed to read xml file");

    uint32_t element = 0;
    do {
        element++;
        if(reader->getNodeType() == irr::io::EXN_ELEMENT) {
            #ifdef PHT_TEST
            std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>(reader->getNodeName());
            #else
            std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>("x");
            #endif
            xmlTree->add(node, current);
            if(!reader->isEmptyElement())
                current = node;
            for(int i = 0; i < reader->getAttributeCount(); i++) {
                #ifdef PHT_TEST
                std::shared_ptr<pht::Node<std::string>> attributeName = std::make_shared<pht::Node<std::string>>(std::string(reader->getAttributeName(i)));
                #else
                std::shared_ptr<pht::Node<std::string>> attributeName = std::make_shared<pht::Node<std::string>>("x");
                #endif
                xmlTree->add(attributeName, current);
                #ifdef PHT_TEST
                std::shared_ptr<pht::Node<std::string>> attributeValue = std::make_shared<pht::Node<std::string>>(std::string(reader->getAttributeValue(i)));
                #else
                std::shared_ptr<pht::Node<std::string>> attributeValue = std::make_shared<pht::Node<std::string>>("x");
                #endif
                xmlTree->add(attributeValue, attributeName);
            }
        } else if(reader->getNodeType() == irr::io::EXN_ELEMENT_END) {
            if(!xmlTree->isRoot(current)) {
                current = xmlTree->getDirectAncestor(current);
            }
        } else if(reader->getNodeType() == irr::io::EXN_TEXT) {
            std::string text = std::string(reader->getNodeData());
            if(!std::all_of(text.begin(), text.end(), isspace)) {
                #ifdef PHT_TEST
                std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>(text);
                #else
                std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>("x");//text);
                #endif
                xmlTree->add(node, current);
            }
        }
    } while(reader->read());

    return xmlTree;
}

std::shared_ptr<pht::UnorderedTree<std::string>> pht::XMLReader::readByName(const std::string &name) {

    char temp[256];
    _getcwd( temp, 256); //der Programmpfad ist jetzt in 'temp' gespeichert
    path myRoot(temp);
    path directory = myRoot;
    while ((directory.stem() != "Projektgruppe_Hypersuccint_Trees" && directory.stem() != "ProjektSuccinctTrees") && directory.root_path() != directory.parent_path()){
        directory = directory.parent_path();
    }

    directory /= "resources";


    std::string xml = ".xml";
    if (!std::equal(xml.rbegin(), xml.rend(), name.rbegin())){
        return read(directory.string() + name + xml);
    }

    return read(directory.string() + name);

}
