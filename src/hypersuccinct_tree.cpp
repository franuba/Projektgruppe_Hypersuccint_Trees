//
// Created by Christopher Pack on 23.04.2021.
//

#include "pht/hypersuccinct_tree.h"
#include "pht/logger.h"
#include <iomanip>
#include <cmath>

using namespace pht;

Bitvector HypersuccinctTree::getMicroTree(MiniTree& miniTree,uint32_t index) {
    if(huffmanFlag) {
        Bitvector huffman = miniTree.microTrees.at(index);
        return std::find_if(lookupTable.begin(),lookupTable.end(), [&huffman](const LookupTableEntry& microTreeData) {return microTreeData.index == huffman;})->bp;
    }
    else {
        return miniTree.microTrees.at(index);
    }
}

uint32_t HypersuccinctTree::getMicroTreeCount(MiniTree& miniTree) {
    return static_cast<uint32_t>(miniTree.microTrees.size());
}

Bitvector HypersuccinctTree::getMicroFID(MiniTree& miniTree,uint32_t index) {
    return miniTree.FIDs.at(index);
}

Bitvector HypersuccinctTree::getMicroTypeVector(MiniTree& miniTree , uint32_t index) {
    return miniTree.typeVectors.at(index);
}

Bitvector HypersuccinctTree::getMicroDummys(MiniTree& miniTree, uint32_t index) {
    return miniTree.dummys.at(index);
}

Bitvector HypersuccinctTree::getMicroDummyPointers(MiniTree& miniTree, uint32_t index) {
    return miniTree.microDummyPointers.at(index);
}

Bitvector HypersuccinctTree::getMiniDummy(uint32_t index) {
    return miniDummys.at(index);
}

LookupTableEntry HypersuccinctTree::getLookupTableEntry(Bitvector indexV) {
    if(huffmanFlag) {
        auto iter = std::find_if(lookupTable.begin(), lookupTable.end(),[&indexV](const LookupTableEntry &microTreeData) {return microTreeData.bp == indexV;});
        if (iter == lookupTable.end()) {

        }
        return *iter;
    } else {
        auto iter = std::find_if(lookupTable.begin(), lookupTable.end(),[&indexV](const LookupTableEntry &microTreeData) {return microTreeData.index == indexV;});
        if (iter == lookupTable.end()) {

        }
        return *iter;
    }
}

bool HypersuccinctTree::lookupTableAncestorMatrixComparison(const LookupTableEntry& entry, uint32_t anc, uint32_t node2Index) {
    auto sizeTable = static_cast<uint32_t>(sqrt(entry.ancestorMatrix.size()));
    return entry.ancestorMatrix.at(sizeTable * anc + node2Index);
}

uint64_t calculateMinitreeByteSize(const MiniTree& mt) {
    return
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.FIDs, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<succinct_bv::BitVector>)+ListUtils::fold<succinct_bv::BitVector,uint64_t>(mt.FIDsSupport, 0, [](uint64_t acc, const succinct_bv::BitVector& x){ return acc+x.n_bytes();})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.typeVectors, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<succinct_bv::BitVector>)+ListUtils::fold<succinct_bv::BitVector,uint64_t>(mt.typeVectorsSupport, 0, [](uint64_t acc, const succinct_bv::BitVector& x){ return acc+x.n_bytes();})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.dummys, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microTrees, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniTopFIDIndex.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniLowFIDIndex.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microTopFIDIndices, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microLowFIDIndices, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microFIDTopTrees, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microFIDLowTrees, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.rootAncestors.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.dummyAncestors.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDummyTree.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDummyIndex.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDummyPointer.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microDummyPointers, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniChildRank.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microChildRanks, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microExtendedChildRanks, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniParent.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microParents, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.subTree.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microSubTrees, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDepth.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniHeight.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDummyDepth.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDummyHeight.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.rootDepths, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.rootHeights, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniLeaves.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microLeaves, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniTreeLeftmostLeafPointer.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniTreeRightmostLeafPointer.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microTreeLeftmostLeafPointers, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microTreeRightmostLeafPointers, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniRootLeafRank.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(mt.miniDummyLeafRank.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microRootLeafRanks, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(mt.microExtendedLeafRanks, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));}); 
}

uint64_t calculateLookupTableEntryByteSize(const LookupTableEntry& lte) {
    return
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(lte.index.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(lte.bp.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(lte.ancestorMatrix.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(lte.childRanks.size())/8.0))+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.parentPointers, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.degree, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.subTrees, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.nodeDepths, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.nodeHeights, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.leaves, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.leftmost_leaf, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.rightmost_leaf, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(lte.leafRank, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<std::vector<Bitvector>>)
            +ListUtils::fold<std::vector<std::vector<bool>>,uint64_t>(lte.childMatrix, 0, [](uint64_t acc, const std::vector<std::vector<bool>>& x)
                {
                    return acc+sizeof(std::vector<std::vector<bool>>)
                    +ListUtils::fold<std::vector<bool>,uint64_t>(x, 0, [](uint64_t acc, const std::vector<bool>&y)
                        {
                            return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(y.size())/8.0));
                        });
                });
}



std::string formatByteSize(uint64_t bytes) {
    uint32_t magnitude = (static_cast<uint32_t>(floor(log10(bytes)))-(static_cast<uint32_t>(floor(log10(bytes)))%3));
    std::stringstream stream;
    stream << std::fixed << std::setprecision(bytes>=1000?2:0) << static_cast<double>(bytes)/pow(10, magnitude);
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

uint64_t HypersuccinctTree::getByteSize() {
    PHT_LOGGER_DEBUG("HST_SIZE") << "huffmanFlag: " << formatByteSize(sizeof(bool)) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "size: " << formatByteSize(sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(size.size())/8.0))) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "microSize: " << formatByteSize(sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(microSize.size())/8.0))) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniSize: " << formatByteSize(sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(miniSize.size())/8.0))) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniTrees: " << formatByteSize(sizeof(std::vector<MiniTree>)+ListUtils::fold<MiniTree,uint64_t>(miniTrees, 0, [](uint64_t acc, const MiniTree& x){ return acc+calculateMinitreeByteSize(x);})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniFIDs: " << formatByteSize(sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniFIDs, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniFIDsSupport: " << formatByteSize(sizeof(std::vector<succinct_bv::BitVector>)+ListUtils::fold<succinct_bv::BitVector,uint64_t>(miniFIDsSupport, 0, [](uint64_t acc, const succinct_bv::BitVector& x){ return acc+x.n_bytes();})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniFIDTopTree: " << formatByteSize(sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniFIDTopTree, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniFIDLowTree: " << formatByteSize(sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniFIDLowTree, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniTypeVectors: " << formatByteSize(sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniTypeVectors, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniTypeVectorsSupport: " << formatByteSize(sizeof(std::vector<succinct_bv::BitVector>)+ListUtils::fold<succinct_bv::BitVector,uint64_t>(miniTypeVectorsSupport, 0, [](uint64_t acc, const succinct_bv::BitVector& x){ return acc+x.n_bytes();})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "miniDummys: " << formatByteSize(sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniDummys, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})) << pht::Logger::endl();
    PHT_LOGGER_DEBUG("HST_SIZE") << "lookupTable: " << formatByteSize(sizeof(std::vector<LookupTableEntry>)+ListUtils::fold<LookupTableEntry,uint64_t>(lookupTable, 0, [](uint64_t acc, const LookupTableEntry& x){ return acc+calculateLookupTableEntryByteSize(x);})) << pht::Logger::endl();

    return
        sizeof(bool)+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(size.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(microSize.size())/8.0))+
        sizeof(std::vector<bool>)+static_cast<uint64_t>(static_cast<double>(ceil(miniSize.size())/8.0))+
        sizeof(std::vector<MiniTree>)+ListUtils::fold<MiniTree,uint64_t>(miniTrees, 0, [](uint64_t acc, const MiniTree& x){ return acc+calculateMinitreeByteSize(x);})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniFIDs, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<succinct_bv::BitVector>)+ListUtils::fold<succinct_bv::BitVector,uint64_t>(miniFIDsSupport, 0, [](uint64_t acc, const succinct_bv::BitVector& x){ return acc+x.n_bytes();})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniFIDTopTree, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniFIDLowTree, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniTypeVectors, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<succinct_bv::BitVector>)+ListUtils::fold<succinct_bv::BitVector,uint64_t>(miniTypeVectorsSupport, 0, [](uint64_t acc, const succinct_bv::BitVector& x){ return acc+x.n_bytes();})+
        sizeof(std::vector<Bitvector>)+ListUtils::fold<std::vector<bool>,uint64_t>(miniDummys, 0, [](uint64_t acc, const std::vector<bool>& x){ return acc+sizeof(std::vector<bool>)+static_cast<uint64_t>(ceil(static_cast<double>(x.size())/8.0));})+
        sizeof(std::vector<LookupTableEntry>)+ListUtils::fold<LookupTableEntry,uint64_t>(lookupTable, 0, [](uint64_t acc, const LookupTableEntry& x){ return acc+calculateLookupTableEntryByteSize(x);});
}