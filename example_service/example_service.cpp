#include <iostream>
#include <string>
#include <filesystem>
#include "pht/logger.h"

#define PHT_TEST
#include "pht/unordered_tree.h"
#include "pht/farzan_munro.h"
#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "bit_vector.h"

using namespace pht;

std::shared_ptr<pht::UnorderedTree<char>> createTestTree();
std::shared_ptr<pht::UnorderedTree<std::string>> createExampleTree();

std::string formatByteSize(uint64_t bytes) {
    uint32_t magnitude = (static_cast<uint32_t>(floor(log10(bytes)))-(static_cast<uint32_t>(floor(log10(bytes)))%3));
    std::stringstream stream;
    stream << std::fixed << std::setprecision(bytes>=1000?2:0) << static_cast<double>(bytes)/std::pow(10, magnitude);
    switch(magnitude) {
        case 0:                 break;
        case 3:  stream << "k"; break;
        case 6:  stream << "M"; break;
        case 9:  stream << "G"; break;
        case 12: stream << "T"; break;
        case 15: stream << "P"; break;
        case 18: stream << "E"; break;
        case 21: stream << "Z"; break;
        case 24: stream << "Y"; break;
        default: stream << "?"; break;
    }
    stream << "B";
    return stream.str();
}

int main() {
    pht::Logger::setLogLevel(pht::Logger::LogLevel::PHT_INFO);
    pht::Logger::setStdOutEnabled(true);
    PHT_LOGGER_INFO("MAIN") << "Executing example service" << pht::Logger::endl();
    PHT_LOGGER_INFO("MAIN") << "Current Filesystem: " << std::filesystem::current_path().string() << pht::Logger::endl();
    pht::Timer globalTimer;
    PHT_LOGGER_INFO("MAIN") << "Reading File..." << pht::Logger::endl();
    pht::Timer localTimer;
    std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName("TreeNath");
    PHT_LOGGER_INFO("MAIN") << tree->getSize() << pht::Logger::endl();
    localTimer.stop();
    PHT_LOGGER_INFO("MAIN") << std::string("File read in ") << localTimer.toString() << pht::Logger::endl();





    PHT_LOGGER_INFO("MAIN") << "Creating HST..." << pht::Logger::endl();
    localTimer.start();
    std::unique_ptr<pht::HypersuccinctTree> hst = pht::HypersuccinctTreeFactory::create(tree, true, 0, 0);
    localTimer.stop();
    PHT_LOGGER_INFO("MAIN") << "HST created in " << localTimer.toString() << pht::Logger::endl();
    std::string size = formatByteSize(hst->getByteSize());
    PHT_LOGGER_INFO("MAIN") << "HST uses " << size << " RAM" << pht::Logger::endl();
    PHT_LOGGER_INFO("MAIN") << "Saving tree to file..." << pht::Logger::endl();
    localTimer.start();
    HypersuccinctTreeOutput::writeToFile(*hst);
    localTimer.stop();
    PHT_LOGGER_INFO("MAIN") << "Tree saved in " << localTimer.toString() << pht::Logger::endl();

    PHT_LOGGER_DEBUG("MAIN") << "Printing original Tree data:" << pht::Logger::endl();
    PHT_LOGGER_DEBUG("TREE") << "Size: " << to_string(tree->getSize());
    PHT_LOGGER_DEBUG("MAIN") << tree->toNewickString() << "\n" << pht::Logger::endl();
    HypersuccinctTreeOutput::printTree(*hst);
    pht::HypersuccinctTree fileHst = HypersuccinctTreeOutput::readFromFile("tree.txt");
    PHT_LOGGER_DEBUG("TREE") << "FileTree:" << pht::Logger::endl();
    HypersuccinctTreeOutput::printTree(fileHst);

    globalTimer.stop();
    PHT_LOGGER_INFO("MAIN") << "Example service executed in " << globalTimer.toString() << pht::Logger::endl();
    return 0;
}


std::shared_ptr<pht::UnorderedTree<char>> createTestTree() {
    std::shared_ptr<pht::UnorderedTree<char>> tree = std::make_shared<pht::UnorderedTree<char>>();
    std::shared_ptr<pht::Node<char>> a = std::make_shared<pht::Node<char>>('a');
    std::shared_ptr<pht::Node<char>> b = std::make_shared<pht::Node<char>>('b');
    std::shared_ptr<pht::Node<char>> c = std::make_shared<pht::Node<char>>('c');
    std::shared_ptr<pht::Node<char>> d = std::make_shared<pht::Node<char>>('d');
    std::shared_ptr<pht::Node<char>> e = std::make_shared<pht::Node<char>>('e');
    std::shared_ptr<pht::Node<char>> f = std::make_shared<pht::Node<char>>('f');
    std::shared_ptr<pht::Node<char>> g = std::make_shared<pht::Node<char>>('g');
    std::shared_ptr<pht::Node<char>> h = std::make_shared<pht::Node<char>>('h');
    std::shared_ptr<pht::Node<char>> i = std::make_shared<pht::Node<char>>('i');
    std::shared_ptr<pht::Node<char>> j = std::make_shared<pht::Node<char>>('j');
    std::shared_ptr<pht::Node<char>> k = std::make_shared<pht::Node<char>>('k');
    std::shared_ptr<pht::Node<char>> l = std::make_shared<pht::Node<char>>('l');
    std::shared_ptr<pht::Node<char>> m = std::make_shared<pht::Node<char>>('m');
    std::shared_ptr<pht::Node<char>> n = std::make_shared<pht::Node<char>>('n');
    std::shared_ptr<pht::Node<char>> o = std::make_shared<pht::Node<char>>('o');
    std::shared_ptr<pht::Node<char>> p = std::make_shared<pht::Node<char>>('p');
    std::shared_ptr<pht::Node<char>> q = std::make_shared<pht::Node<char>>('q');
    std::shared_ptr<pht::Node<char>> r = std::make_shared<pht::Node<char>>('r');
    std::shared_ptr<pht::Node<char>> s = std::make_shared<pht::Node<char>>('s');
    std::shared_ptr<pht::Node<char>> t = std::make_shared<pht::Node<char>>('t');
    std::shared_ptr<pht::Node<char>> u = std::make_shared<pht::Node<char>>('u');
    std::shared_ptr<pht::Node<char>> v = std::make_shared<pht::Node<char>>('v');
    std::shared_ptr<pht::Node<char>> w = std::make_shared<pht::Node<char>>('w');
    std::shared_ptr<pht::Node<char>> x = std::make_shared<pht::Node<char>>('x');
    std::shared_ptr<pht::Node<char>> y = std::make_shared<pht::Node<char>>('y');
    std::shared_ptr<pht::Node<char>> z = std::make_shared<pht::Node<char>>('z');
    std::shared_ptr<pht::Node<char>> A = std::make_shared<pht::Node<char>>('A');
    std::shared_ptr<pht::Node<char>> B = std::make_shared<pht::Node<char>>('B');

    tree->add(a);
    tree->add(b, a);
    tree->add(c, a);
    tree->add(d, a);
    tree->add(e, a);
    tree->add(f, a);
    tree->add(g, b);
    tree->add(h, b);
    tree->add(i, b);
    tree->add(j, b);
    tree->add(k, d);
    tree->add(l, g);
    tree->add(m, i);
    tree->add(n, k);
    tree->add(o, k);
    tree->add(p, k);
    tree->add(q, o);
    tree->add(r, o);
    tree->add(s, q);
    tree->add(t, q);
    tree->add(u, q);
    tree->add(v, q);
    tree->add(w, q);
    tree->add(x, r);
    tree->add(y, r);
    tree->add(z, r);
    tree->add(A, t);
    tree->add(B, x);

    return tree;
}

std::shared_ptr<pht::UnorderedTree<std::string>> createExampleTree() {
    std::shared_ptr<pht::UnorderedTree<std::string>> tree = std::make_shared<pht::UnorderedTree<std::string>>();
    std::shared_ptr<pht::Node<std::string>> a0 = std::make_shared<pht::Node<std::string>>("0");
    std::shared_ptr<pht::Node<std::string>> a1 = std::make_shared<pht::Node<std::string>>("1");
    std::shared_ptr<pht::Node<std::string>> a2 = std::make_shared<pht::Node<std::string>>("2");
    std::shared_ptr<pht::Node<std::string>> a3 = std::make_shared<pht::Node<std::string>>("3");
    std::shared_ptr<pht::Node<std::string>> a4 = std::make_shared<pht::Node<std::string>>("4");
    std::shared_ptr<pht::Node<std::string>> a5 = std::make_shared<pht::Node<std::string>>("5");
    std::shared_ptr<pht::Node<std::string>> a6 = std::make_shared<pht::Node<std::string>>("6");
    std::shared_ptr<pht::Node<std::string>> a7 = std::make_shared<pht::Node<std::string>>("7");
    std::shared_ptr<pht::Node<std::string>> a8 = std::make_shared<pht::Node<std::string>>("8");
    std::shared_ptr<pht::Node<std::string>> a9 = std::make_shared<pht::Node<std::string>>("9");
    std::shared_ptr<pht::Node<std::string>> a10 = std::make_shared<pht::Node<std::string>>("10");
    std::shared_ptr<pht::Node<std::string>> a11 = std::make_shared<pht::Node<std::string>>("11");
    std::shared_ptr<pht::Node<std::string>> a12 = std::make_shared<pht::Node<std::string>>("12");
    std::shared_ptr<pht::Node<std::string>> a13 = std::make_shared<pht::Node<std::string>>("13");
    std::shared_ptr<pht::Node<std::string>> a14 = std::make_shared<pht::Node<std::string>>("14");
    std::shared_ptr<pht::Node<std::string>> a15 = std::make_shared<pht::Node<std::string>>("15");
    std::shared_ptr<pht::Node<std::string>> a16 = std::make_shared<pht::Node<std::string>>("16");
    std::shared_ptr<pht::Node<std::string>> a17 = std::make_shared<pht::Node<std::string>>("17");
    std::shared_ptr<pht::Node<std::string>> a18 = std::make_shared<pht::Node<std::string>>("18");
    std::shared_ptr<pht::Node<std::string>> a19 = std::make_shared<pht::Node<std::string>>("19");
    std::shared_ptr<pht::Node<std::string>> a20 = std::make_shared<pht::Node<std::string>>("20");
    std::shared_ptr<pht::Node<std::string>> a21 = std::make_shared<pht::Node<std::string>>("21");
    std::shared_ptr<pht::Node<std::string>> a22 = std::make_shared<pht::Node<std::string>>("22");
    std::shared_ptr<pht::Node<std::string>> a23 = std::make_shared<pht::Node<std::string>>("23");
    std::shared_ptr<pht::Node<std::string>> a24 = std::make_shared<pht::Node<std::string>>("24");
    std::shared_ptr<pht::Node<std::string>> a25 = std::make_shared<pht::Node<std::string>>("25");
    std::shared_ptr<pht::Node<std::string>> a26 = std::make_shared<pht::Node<std::string>>("26");
    std::shared_ptr<pht::Node<std::string>> a27 = std::make_shared<pht::Node<std::string>>("27");
    std::shared_ptr<pht::Node<std::string>> a28 = std::make_shared<pht::Node<std::string>>("28");
    std::shared_ptr<pht::Node<std::string>> a29 = std::make_shared<pht::Node<std::string>>("29");
    std::shared_ptr<pht::Node<std::string>> a30 = std::make_shared<pht::Node<std::string>>("30");
    std::shared_ptr<pht::Node<std::string>> a31 = std::make_shared<pht::Node<std::string>>("31");
    std::shared_ptr<pht::Node<std::string>> a32 = std::make_shared<pht::Node<std::string>>("32");
    std::shared_ptr<pht::Node<std::string>> a33 = std::make_shared<pht::Node<std::string>>("33");
    std::shared_ptr<pht::Node<std::string>> a34 = std::make_shared<pht::Node<std::string>>("34");
    std::shared_ptr<pht::Node<std::string>> a35 = std::make_shared<pht::Node<std::string>>("35");
    std::shared_ptr<pht::Node<std::string>> a36 = std::make_shared<pht::Node<std::string>>("36");
    std::shared_ptr<pht::Node<std::string>> a37 = std::make_shared<pht::Node<std::string>>("37");
    std::shared_ptr<pht::Node<std::string>> a38 = std::make_shared<pht::Node<std::string>>("38");
    std::shared_ptr<pht::Node<std::string>> a39 = std::make_shared<pht::Node<std::string>>("39");
    std::shared_ptr<pht::Node<std::string>> a40 = std::make_shared<pht::Node<std::string>>("40");
    std::shared_ptr<pht::Node<std::string>> a41 = std::make_shared<pht::Node<std::string>>("41");
    std::shared_ptr<pht::Node<std::string>> a42 = std::make_shared<pht::Node<std::string>>("42");
    std::shared_ptr<pht::Node<std::string>> a43 = std::make_shared<pht::Node<std::string>>("43");
    std::shared_ptr<pht::Node<std::string>> a44 = std::make_shared<pht::Node<std::string>>("44");
    std::shared_ptr<pht::Node<std::string>> a45 = std::make_shared<pht::Node<std::string>>("45");
    std::shared_ptr<pht::Node<std::string>> a46 = std::make_shared<pht::Node<std::string>>("46");
    std::shared_ptr<pht::Node<std::string>> a47 = std::make_shared<pht::Node<std::string>>("47");
    std::shared_ptr<pht::Node<std::string>> a48 = std::make_shared<pht::Node<std::string>>("48");
    std::shared_ptr<pht::Node<std::string>> a49 = std::make_shared<pht::Node<std::string>>("49");
    std::shared_ptr<pht::Node<std::string>> a50 = std::make_shared<pht::Node<std::string>>("50");
    std::shared_ptr<pht::Node<std::string>> a51 = std::make_shared<pht::Node<std::string>>("51");
    std::shared_ptr<pht::Node<std::string>> a52 = std::make_shared<pht::Node<std::string>>("52");
    std::shared_ptr<pht::Node<std::string>> a53 = std::make_shared<pht::Node<std::string>>("53");
    std::shared_ptr<pht::Node<std::string>> a54 = std::make_shared<pht::Node<std::string>>("54");
    std::shared_ptr<pht::Node<std::string>> a55 = std::make_shared<pht::Node<std::string>>("55");
    std::shared_ptr<pht::Node<std::string>> a56 = std::make_shared<pht::Node<std::string>>("56");
    std::shared_ptr<pht::Node<std::string>> a57 = std::make_shared<pht::Node<std::string>>("57");
    std::shared_ptr<pht::Node<std::string>> a58 = std::make_shared<pht::Node<std::string>>("58");
    std::shared_ptr<pht::Node<std::string>> a59 = std::make_shared<pht::Node<std::string>>("59");
    std::shared_ptr<pht::Node<std::string>> a60 = std::make_shared<pht::Node<std::string>>("60");


    tree->add(a0);
    //add(node,ancestor)
    tree->add(a1, a0);
    tree->add(a2, a0);
    tree->add(a3, a0);
    tree->add(a4, a0);
    tree->add(a5, a0);
    tree->add(a6, a1);
    tree->add(a7, a1);
    tree->add(a8, a2);
    tree->add(a9, a3);
    tree->add(a10, a3);
    tree->add(a11, a4);
    tree->add(a12, a6);
    tree->add(a13, a7);
    tree->add(a14, a7);
    tree->add(a15, a8);
    tree->add(a16, a8);
    tree->add(a17, a8);
    tree->add(a18, a9);
    tree->add(a19, a11);
    tree->add(a20, a11);
    tree->add(a21, a11);
    tree->add(a22, a11);
    tree->add(a23, a12);
    tree->add(a24, a13);
    tree->add(a25, a14);
    tree->add(a26, a14);
    tree->add(a27, a14);
    tree->add(a28, a16);
    tree->add(a29, a18);
    tree->add(a30, a18);
    tree->add(a31, a18);
    tree->add(a32, a19);
    tree->add(a33, a19);
    tree->add(a34, a21);
    tree->add(a35, a22);
    tree->add(a36, a22);
    tree->add(a37, a22);
    tree->add(a38, a23);
    tree->add(a39, a24);
    tree->add(a40, a25);
    tree->add(a41, a26);
    tree->add(a42, a26);
    tree->add(a43, a28);
    tree->add(a44, a28);
    tree->add(a45, a28);
    tree->add(a46, a30);
    tree->add(a47, a30);
    tree->add(a48, a32);
    tree->add(a49, a33);
    tree->add(a50, a35);
    tree->add(a51, a35);
    tree->add(a52, a36);
    tree->add(a53, a37);
    tree->add(a54, a37);
    tree->add(a55, a43);
    tree->add(a56, a44);
    tree->add(a57, a45);
    tree->add(a58, a52);
    tree->add(a59, a57);
    tree->add(a60, a58);

    return tree;
}