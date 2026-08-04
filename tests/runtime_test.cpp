//
// Created by Christopher Pack on 30.07.2021.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"


//#define PHT_TEST
//#ifdef NDEBUG
//#define PHT_LOGGER_QUIET
//#endif
#define MEASURE_TIME(func, output) timer.start(); func; timer.stop(); output.emplace_back(name,timer.toString());

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"
#include "pht/logger.h"
#include <fstream>
#include <filesystem>

#define convertToBitvector pht::Bitvector_Utils::convertToBitvector

class RuntimeTest : public ::testing::Test {
protected:
    //Laufzeittests:
    /*
     * Queries
     * Factory
     * WriteToFile / ReadFromFile
     */
    std::vector<std::string> fileNames = {"TreeNath3.xml"};
            //{"DBLP.xml"};
            //{"TreeNath.xml","TreeNath2.xml","TreeNath3.xml","TreeNath4.xml","TreeNath5.xml"};
    std::string resultFileName = "testResultsFactoryOptimizedPrecompCheckARR.csv";
    std::vector<pair<std::string , std::string >> factoryTimes;
    std::vector<pair<std::string , std::string >> factoryTimesHuffman;
    std::vector<pair<std::string , std::string >> factoryTimesNoQueries;
    std::vector<pair<std::string , std::string >> factoryTimesHuffmanNoQueries;
    std::vector<pair<std::string , std::string >> childTimes;
    std::vector<pair<std::string , std::string >> degreeTimes;
    std::vector<pair<std::string , std::string >> leftmost_leafTimes;
    std::vector<pair<std::string , std::string >> rightmost_leafTimes;
    std::vector<pair<std::string , std::string >> sizeTimes;

    std::vector<pair<std::string , std::string >> childRankTimes;
    std::vector<pair<std::string , std::string >> subTreeTimes;
    std::vector<pair<std::string , std::string >> depthTimes;
    std::vector<pair<std::string , std::string >> heightTimes;
    std::vector<pair<std::string , std::string >> isDummyAncestorWithinMiniTreeTimes;
    std::vector<pair<std::string , std::string >> isDummyAncestorWithinMicroTreeTimes;
    std::vector<pair<std::string , std::string >> leafSizeTimes;
    std::vector<pair<std::string , std::string >> leafRankTimes;

    std::vector<pair<std::string , std::string >> writeToFileTimes;
    std::vector<pair<std::string , std::string >> readFromFileTimes;

    std::vector<pair<std::string , std::string >> normalFileSizes;
    std::vector<pair<std::string , std::string >> huffmanFileSizes;
    pht::Timer timer;
};

TEST_F(RuntimeTest, FullTestRaw) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test") << "Begin: " << name;
        PHT_LOGGER_DEBUG("Test") << "Reading " << name << "\n";
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test") << "Finished Reading File.";
        PHT_LOGGER_DEBUG("Test") << "Finished reading " << name << "\n";
        PHT_LOGGER_DEBUG("Test") << "Nodes: " << std::to_string(tree->getSize()) << "\n";
        timer.start();
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, false, 0, 0);
        timer.stop();
        tree.reset();
        factoryTimes.emplace_back(name, timer.toString());

        PHT_LOGGER_INFO("Runtime Test") << "Finished Creating HST";

        uint32_t size = hyperTree.subtreeSize({0,0,0});
        sizeTimes.emplace_back(name, to_string(size));

        std::vector<pht::HstNode > testNodes = {{0,0,0}};
        uint32_t rootdegree;
        timer.start();
        rootdegree = hyperTree.degree({0,0,0});
        timer.stop();
        degreeTimes.emplace_back(name,timer.toString());

        if(rootdegree > 1001) {
            rootdegree = 1001;
        }
        timer.stop();
        degreeTimes.emplace_back(name,timer.toString());
        for(uint32_t i = 0; i<rootdegree; i++) {
            timer.start();
            pht::HstNode child = hyperTree.child({0,0,0},i);
            timer.stop();
            childTimes.emplace_back(name,timer.toString());
            testNodes.push_back(child);
        }

        PHT_LOGGER_INFO("Runtime Test") << "Finished reading root children";

        timer.start();
        uint32_t rootSubSize = hyperTree.subtreeSize({0,0,0});
        timer.stop();
        subTreeTimes.emplace_back(name,timer.toString());
        uint32_t testSize = 1001;
        if(rootSubSize < testSize) {
            testSize = rootSubSize;
        }

        PHT_LOGGER_INFO("Runtime Test") << "Test Size: " << testSize;

        uint32_t i=1;
        while(testNodes.size() < testSize) {
            if(i >= testNodes.size()) {
                break;
            }
            if(i == 28) {
                uint32_t e = 1;
            }
            uint32_t degree;
            timer.start();
            degree = hyperTree.degree(testNodes.at(i));
            timer.stop();
            degreeTimes.emplace_back(name,timer.toString());
            for(uint32_t j = 0; j<degree; j++) {
                if(testNodes.size() >= testSize) {
                    break;
                }
                timer.start();
                pht::HstNode child = hyperTree.child(testNodes.at(i),j);
                PHT_LOGGER_DEBUG("Test") << i;
                PHT_LOGGER_DEBUG("Test") << "Current Node: " << testNodes.at(i).mini << ", " << testNodes.at(i).micro << ", " << testNodes.at(i).node << ";";
                PHT_LOGGER_DEBUG("Test") << "Child: " << child.mini << ", " << child.micro << ", " << child.node << ";";
                timer.stop();
                childTimes.emplace_back(name,timer.toString());
                testNodes.push_back(child);
            }
            i++;
        }

        PHT_LOGGER_INFO("Runtime Test") << "Added all children";


        for(pht::HstNode &node : testNodes) {
            timer.start();
            pht::HstNode left_leaf = hyperTree.leftmostLeaf(node);
            timer.stop();
            leftmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {left_leaf})) {
                    testNodes.push_back(left_leaf);
                }
            }

            timer.start();
            pht::HstNode right_leaf = hyperTree.rightmostLeaf(node);
            timer.stop();
            rightmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {right_leaf})) {
                    testNodes.push_back(right_leaf);
                }
            }
        }
        PHT_LOGGER_INFO("Runtime Test") << "Running query Tests...";

        for(pht::HstNode &node : testNodes) {
            MEASURE_TIME(hyperTree.childRank(node),childRankTimes);
            MEASURE_TIME(hyperTree.subtreeSize(node),subTreeTimes);
            MEASURE_TIME(hyperTree.depth(node),depthTimes);
            MEASURE_TIME(hyperTree.height(node),heightTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMiniTree(node),isDummyAncestorWithinMiniTreeTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMicroTree(node),isDummyAncestorWithinMicroTreeTimes);
            MEASURE_TIME(hyperTree.leafSize(node),leafSizeTimes);
            MEASURE_TIME(hyperTree.leafRank(node),leafRankTimes);
        }

        testNodes.clear();
    }


    PHT_LOGGER_INFO("Runtime Test") << "Begin File Output";
    std::ofstream file;
    file.open(resultFileName,std::iostream::app);
    for(std::pair<std::string,std::string> &value : factoryTimes) {
        file << value.first << ", Factory::create, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : childTimes) {
        file << value.first << ", child, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : degreeTimes) {
        file << value.first << ", degree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leftmost_leafTimes) {
        file << value.first << ", leftmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : rightmost_leafTimes) {
        file << value.first << ", rightmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : childRankTimes) {
        file << value.first << ", child_rank, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : subTreeTimes) {
        file << value.first << ", subtree_size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : depthTimes) {
        file << value.first << ", depth, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : heightTimes) {
        file << value.first << ", height, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : isDummyAncestorWithinMiniTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMiniTree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : isDummyAncestorWithinMicroTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMicroTree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leafSizeTimes) {
        file << value.first << ", leaf_size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leafRankTimes) {
        file << value.first << ", leaf_rank, " << value.second << "\n";
    }
    file.close();
}

TEST_F(RuntimeTest, FullTestHuffman) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test") << "Begin: " << name;
        PHT_LOGGER_DEBUG("Test") << "Reading " << name << "\n";
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test") << "Finished Reading File.";
        PHT_LOGGER_DEBUG("Test") << "Finished reading " << name << "\n";
        PHT_LOGGER_DEBUG("Test") << "Nodes: " << tree->getSize() << "\n";
        timer.start();
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, true, 0, 0);
        timer.stop();
        tree.reset();
        factoryTimesHuffman.emplace_back(name, timer.toString());

        uint32_t size = hyperTree.subtreeSize({0,0,0});
        sizeTimes.emplace_back(name, to_string(size));

        std::vector<pht::HstNode > testNodes = {{0,0,0}};
        uint32_t rootdegree;
        timer.start();
        rootdegree = hyperTree.degree({0,0,0});
        timer.stop();
        degreeTimes.emplace_back(name,timer.toString());

        if(rootdegree > 1001) {
            rootdegree = 1001;
        }

        for(uint32_t i = 0; i<rootdegree; i++) {
            timer.start();
            pht::HstNode child = hyperTree.child({0,0,0},i);
            timer.stop();
            childTimes.emplace_back(name,timer.toString());
            testNodes.push_back(child);
        }

        timer.start();
        uint32_t rootSubSize = hyperTree.subtreeSize({0,0,0});
        timer.stop();
        subTreeTimes.emplace_back(name,timer.toString());
        uint32_t testSize = 1001;
        if(rootSubSize < testSize) {
            testSize = rootSubSize;
        }

        PHT_LOGGER_INFO("Runtime Test") << "Test Size: " << testSize;

        uint32_t i=1;
        while(testNodes.size() < testSize) {
            if(i >= testNodes.size()) {
                break;
            }
            uint32_t degree;
            timer.start();
            degree = hyperTree.degree(testNodes.at(i));
            timer.stop();
            degreeTimes.emplace_back(name,timer.toString());
            for(uint32_t j = 0; j<degree; j++) {
                if(testNodes.size() >= testSize) {
                    break;
                }
                timer.start();
                pht::HstNode child = hyperTree.child(testNodes.at(i),j);
                /*PHT_LOGGER_DEBUG("Test") << i << std::endl;
                PHT_LOGGER_DEBUG("Test") << "Current Node: " << testNodes.at(i).mini << ", " << testNodes.at(i).micro << ", " << testNodes.at(i).node << ";" << std::endl;
                PHT_LOGGER_DEBUG("Test") << "Child: " << child.mini << ", " << child.micro << ", " << child.node << ";" << std::endl;*/
                timer.stop();
                childTimes.emplace_back(name,timer.toString());
                testNodes.push_back(child);
            }
            i++;
        }

        PHT_LOGGER_INFO("Runtime Test") << "Added all children";


        for(pht::HstNode &node : testNodes) {
            timer.start();
            pht::HstNode left_leaf = hyperTree.leftmostLeaf(node);
            timer.stop();
            leftmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {left_leaf})) {
                    testNodes.push_back(left_leaf);
                }
            }

            timer.start();
            pht::HstNode right_leaf = hyperTree.rightmostLeaf(node);
            timer.stop();
            rightmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {right_leaf})) {
                    testNodes.push_back(right_leaf);
                }
            }
        }
        PHT_LOGGER_INFO("Runtime Test") << "Running query Tests...";

        for(pht::HstNode &node : testNodes) {
            MEASURE_TIME(hyperTree.childRank(node),childRankTimes);
            MEASURE_TIME(hyperTree.subtreeSize(node),subTreeTimes);
            MEASURE_TIME(hyperTree.depth(node),depthTimes);
            MEASURE_TIME(hyperTree.height(node),heightTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMiniTree(node),isDummyAncestorWithinMiniTreeTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMicroTree(node),isDummyAncestorWithinMicroTreeTimes);
            MEASURE_TIME(hyperTree.leafSize(node),leafSizeTimes);
            MEASURE_TIME(hyperTree.leafRank(node),leafRankTimes);
        }

        testNodes.clear();
    }


    PHT_LOGGER_INFO("Runtime Test") << "Begin File Output";
    std::ofstream file;
    file.open(resultFileName,std::iostream::app);
    for(std::pair<std::string,std::string> &value : factoryTimesHuffman) {
        file << value.first << ", Factory::create Huffman, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : childTimes) {
        file << value.first << ", child, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : degreeTimes) {
        file << value.first << ", degree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leftmost_leafTimes) {
        file << value.first << ", leftmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : rightmost_leafTimes) {
        file << value.first << ", rightmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : childRankTimes) {
        file << value.first << ", child_rank, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : subTreeTimes) {
        file << value.first << ", subtree_size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : depthTimes) {
        file << value.first << ", depth, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : heightTimes) {
        file << value.first << ", height, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : isDummyAncestorWithinMiniTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMiniTree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : isDummyAncestorWithinMicroTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMicroTree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leafSizeTimes) {
        file << value.first << ", leaf_size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leafRankTimes) {
        file << value.first << ", leaf_rank, " << value.second << "\n";
    }
    file.close();
}

TEST_F(RuntimeTest, FactoryOnlyWithQueries) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test") << "Begin: " << name;
        PHT_LOGGER_DEBUG("Test") << "Reading " << name << "\n";
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test") << "Finished Reading File.";
        PHT_LOGGER_DEBUG("Test") << "Finished reading " << name << "\n";
        PHT_LOGGER_DEBUG("Test") << "Nodes: " << tree->getSize() << "\n";
        PHT_LOGGER_INFO("Runtime Test") << "Begin Create Raw.";
        timer.start();
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, false, 0, 0);
        timer.stop();
        tree.reset();
        factoryTimes.emplace_back(name, timer.toString());

        uint32_t size = hyperTree.subtreeSize({0,0,0});
        sizeTimes.emplace_back(name, to_string(size));

        PHT_LOGGER_INFO("Runtime Test") << "Begin Create Huffman.";
        tree = pht::XMLReader::readByName(name);
        timer.start();
        hyperTree = *pht::HypersuccinctTreeFactory::create(tree, true, 0, 0);
        timer.stop();
        tree.reset();
        factoryTimesHuffman.emplace_back(name, timer.toString());
    }


    PHT_LOGGER_INFO("Runtime Test") << "Begin File Output";
    std::ofstream file;
    file.open(resultFileName,std::iostream::app);
    for(std::pair<std::string,std::string> &value : factoryTimes) {
        file << value.first << ", Factory::create, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : factoryTimesHuffman) {
        file << value.first << ", Factory::create Huffman, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    file.close();
}

TEST_F(RuntimeTest, FactoryOnlyNoQueries) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test") << "Begin: " << name;
        PHT_LOGGER_DEBUG("Test") << "Reading " << name << "\n";
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test") << "Finished Reading File.";
        PHT_LOGGER_DEBUG("Test") << "Finished reading " << name << "\n";
        PHT_LOGGER_DEBUG("Test") << "Nodes: " << tree->getSize() << "\n";
        PHT_LOGGER_INFO("Runtime Test") << "Begin Create without Queries.";
        timer.start();
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, false, 0, 0,false);
        timer.stop();
        factoryTimesNoQueries.emplace_back(name, timer.toString());

        uint32_t size = hyperTree.subtreeSize({0,0,0});
        sizeTimes.emplace_back(name, to_string(size));

        PHT_LOGGER_INFO("Runtime Test") << "Begin Create Huffman without Queries.";
        timer.start();
        hyperTree = *pht::HypersuccinctTreeFactory::create(tree, true, 0, 0,false);
        timer.stop();
        tree.reset();
        factoryTimesHuffmanNoQueries.emplace_back(name, timer.toString());
    }


    PHT_LOGGER_INFO("Runtime Test") << "Begin File Output";
    std::ofstream file;
    file.open(resultFileName,std::iostream::app);
    for(std::pair<std::string,std::string> &value : factoryTimesNoQueries) {
        file << value.first << ", Factory::create NoQueries, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : factoryTimesHuffmanNoQueries) {
        file << value.first << ", Factory::create Huffman+NoQueries, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    file.close();
}

TEST_F(RuntimeTest, WriteToAndLoadFromFile) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test") << "Begin: " << name;
        PHT_LOGGER_DEBUG("Test") << "Reading " << name << "\n";
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test") << "Finished Reading File.";
        PHT_LOGGER_DEBUG("Test") << "Finished reading " << name << "\n";
        PHT_LOGGER_DEBUG("Test") << "Nodes: " << tree->getSize() << "\n";
        PHT_LOGGER_INFO("Runtime Test") << "Begin Create.";
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, false, 0, 0,true);


        PHT_LOGGER_INFO("Runtime Test") << "Begin writing to file.";

        cout << "Starting Write..." << endl;
        timer.start();
        pht::HypersuccinctTreeOutput::writeToFile(hyperTree);
        timer.stop();
        writeToFileTimes.emplace_back(name, timer.toString());

        uint32_t size = hyperTree.subtreeSize({0,0,0});
        sizeTimes.emplace_back(name, to_string(size));

        cout << "Starting Read..." << endl;
        PHT_LOGGER_INFO("Runtime Test") << "Begin reading from file.";
        timer.start();
        hyperTree = pht::HypersuccinctTreeOutput::readFromFile("tree.txt");
        timer.stop();
        readFromFileTimes.emplace_back(name, timer.toString());
        tree.reset();
    }

    PHT_LOGGER_INFO("Runtime Test") << "Begin File Output";
    std::ofstream file;
    file.open(resultFileName,std::iostream::app);
    for(std::pair<std::string,std::string> &value : writeToFileTimes) {
        file << value.first << ", Factory::create WriteToFile, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : readFromFileTimes) {
        file << value.first << ", Factory::create ReadFromFile, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : sizeTimes) {
        file << value.first << ", size, " << value.second << "\n";
    }
    file.close();
}

TEST_F(RuntimeTest, HuffmanSizeTest) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test") << "Begin: " << name;
        PHT_LOGGER_DEBUG("Test") << "Reading " << name << "\n";
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test") << "Finished Reading File.";
        PHT_LOGGER_DEBUG("Test") << "Finished reading " << name << "\n";
        PHT_LOGGER_DEBUG("Test") << "Nodes: " << tree->getSize() << "\n";
        PHT_LOGGER_INFO("Runtime Test") << "Begin Create without Queries.";
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, false, 0, 0,false);
        tree = pht::XMLReader::readByName(name);
        pht::HypersuccinctTree hyperTreeHuff = *pht::HypersuccinctTreeFactory::create(tree, true, 0, 0,false);


        PHT_LOGGER_INFO("Runtime Test") << "Writing Normal Tree to File.";
        timer.start();
        pht::HypersuccinctTreeOutput::writeHuffmanToFile(hyperTree,"hyperTreeNormal.txt");
        pht::HypersuccinctTreeOutput::writeHuffmanToFile(hyperTree,"hyperTreeNormalALL.txt",true);
        timer.stop();
        writeToFileTimes.emplace_back(name, timer.toString());

        PHT_LOGGER_INFO("Runtime Test") << "Writing Huffman Tree to File.";
        timer.start();
        pht::HypersuccinctTreeOutput::writeHuffmanToFile(hyperTreeHuff,"hyperTreeHuff.txt");
        pht::HypersuccinctTreeOutput::writeHuffmanToFile(hyperTreeHuff,"hyperTreeHuffALL.txt",true);
        timer.stop();
        readFromFileTimes.emplace_back(name, timer.toString());
        tree.reset();

        PHT_LOGGER_INFO("Runtime Test") << "Checking File Sizes";
        std::error_code ec;
        std::uintmax_t size = std::filesystem::file_size("hyperTreeNormal.txt",ec);
        if (ec) {
            std::cout << " : " << ec.message() << '\n';
        } else {
            normalFileSizes.emplace_back(name,std::to_string(size));
        }
        size = std::filesystem::file_size("hyperTreeNormalALL.txt",ec);
        if (ec) {
            std::cout << " : " << ec.message() << '\n';
        } else {
            normalFileSizes.emplace_back(name,std::to_string(size));
        }
        size = std::filesystem::file_size("hyperTreeHuff.txt",ec);
        if (ec) {
            std::cout << " : " << ec.message() << '\n';
        } else {
            huffmanFileSizes.emplace_back(name,std::to_string(size));
        }
        size = std::filesystem::file_size("hyperTreeHuffALL.txt",ec);
        if (ec) {
            std::cout << " : " << ec.message() << '\n';
        } else {
            huffmanFileSizes.emplace_back(name,std::to_string(size));
        }
    }

    PHT_LOGGER_INFO("Runtime Test") << "Begin File Output";
    std::ofstream file;
    file.open(resultFileName + "HuffTable.csv",std::iostream::app);
    for(std::pair<std::string,std::string> &value : normalFileSizes) {
        file << value.first << ", Tree::Normal Size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : huffmanFileSizes) {
        file << value.first << ", Tree::Huffman Size, " << value.second << "\n";
    }
    file.close();
}