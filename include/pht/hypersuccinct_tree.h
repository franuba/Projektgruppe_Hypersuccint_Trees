#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H

#include <iostream>
#include <functional>
#include <utility>

#include "unordered_tree.h"
#include "list_utils.h"
#include "farzan_munro.h"
#include "bitvector_utils.h"
#include "bit_vector.h"
#include "hst_node.h"
#include "pht_export.h"

namespace pht {
    typedef std::vector<bool> Bitvector;
    //node identification by: Minitree, MicroTree, NodeInMicroTree

    /**
     * MiniTree represents MiniTree of the HypersuccinctTree
     * It contains all information needed to query a single MiniTree.
     */
    struct PHT_EXPORT MiniTree {
        #ifdef _MSC_VER
        #pragma warning(disable:4251)
        #endif
        std::vector<Bitvector> FIDs; ///MicroFIDs
        std::vector<succinct_bv::BitVector> FIDsSupport; ///Rank + Select for MicroFIDs
        std::vector<Bitvector> typeVectors; ///MicroTypeVectors
        std::vector<succinct_bv::BitVector> typeVectorsSupport; ///Rank + Select for typeVectors
        std::vector<Bitvector> dummys; ///MicroDummys: Static Size Encoding
        std::vector<Bitvector> microTrees; ///MicroTrees as encoded (BP if no encoding, huffman code if huffman encoding)

        Bitvector miniTopFIDIndex; ///MiniTree Top FID Index + 1
        Bitvector miniLowFIDIndex; ///MiniTree Low FID Index + 1
        std::vector<Bitvector> microTopFIDIndices; ///MicroTree Top FID Indices + 1
        std::vector<Bitvector> microLowFIDIndices; ///MicroTree Low FID Indices + 1
        std::vector<Bitvector> microFIDTopTrees; ///First Top Tree of MicroFIDs
        std::vector<Bitvector> microFIDLowTrees; ///First Low Tree of MicroFIDs
        
        Bitvector rootAncestors; ///Is MicroTree root ancestor of MiniTreeDummy? empty/0 if no MiniDummy exists
        Bitvector dummyAncestors; ///Is MicroTreeDummy ancestor of MiniTreeDummy? 0 per entry if no MicroDummy exists, empty/0 if no MiniDummy exists
        Bitvector miniDummyTree; ///If MiniTree has Dummy: Which MicroTree contains this Dummy?
        Bitvector miniDummyIndex; ///If MiniTree has Dummy: Which Index within the MicroTree is this Dummy?
        Bitvector miniDummyPointer; ///If MiniTree has Dummy: To which Tree does the pointer lead?
        std::vector<Bitvector> microDummyPointers; ///MicroTree Dummy Pointer: Static Size Encoding

        Bitvector miniChildRank; ///Child Rank of MiniTrees
        std::vector<Bitvector> microChildRanks; ///Child Ranks of MicroTrees EXCEPT AT INDEX 0: ChildRank of displaced MiniTree
        std::vector<Bitvector> microExtendedChildRanks; ///Child Ranks of displaced MicroTrees

        Bitvector miniParent; ///Pointer to direct parent MiniTree
        std::vector<Bitvector> microParents; ///Pointer to direct parent MicroTree for all MicroTrees

        Bitvector subTree; ///SubTree Size MiniTree
        std::vector<Bitvector> microSubTrees; ///SubTree Size for MicroTree roots
        Bitvector miniDepth; ///Depth of the MiniTree root
        Bitvector miniHeight; ///Height of the MiniTree root
        Bitvector miniDummyDepth; ///Depth of the MiniTree Dummy
        Bitvector miniDummyHeight; ///Height of the MiniTree Dummy
        std::vector<Bitvector> rootDepths; ///Depths for MicroTree roots + 1
        std::vector<Bitvector> rootHeights; ///Heights for MicroTree roots + 1
        Bitvector miniLeaves; ///Amount of Leaves in MiniTree
        std::vector<Bitvector> microLeaves; ///Amount of Leaves within MicroTrees
        Bitvector miniTreeLeftmostLeafPointer; ///Leftmost Leaf Pointer for MiniTree
        Bitvector miniTreeRightmostLeafPointer; ///Rightmost Leaf Pointer for MiniTree
        std::vector<Bitvector> microTreeLeftmostLeafPointers; ///Leftmost Leaf Pointers for MicroTrees
        std::vector<Bitvector> microTreeRightmostLeafPointers; ///Rightmost Leaf Pointers for MicroTrees
        Bitvector miniRootLeafRank; ///Leaf Rank of MiniTree Root
        Bitvector miniDummyLeafRank; ///Leaf Rank of MiniTree Dummy
        std::vector<Bitvector> microRootLeafRanks; ///Leaf Ranks of MicroTree Roots + 1 EXCEPT AT INDEX 0: There it is the displaced Leaf Rank of the MiniTree
        std::vector<Bitvector> microExtendedLeafRanks; ///Special Leaf Ranks for displaced microTrees + 1
        #ifdef _MSC_VER
        #pragma warning(default:4251)
        #endif
    };

    /**
     * LookupTableEntry represents a single Entry of the Hypersuccinct Tree's Lookp Table.
     * It is indexed by the MicroTrees Balanced Parenthesis form (if no encoding is chosen) or by their Huffman code (if Huffman encoding is chosen)
     * It contains all fields necessary to satisfy the query's need for structural information
     */
    struct PHT_EXPORT LookupTableEntry {
        #ifdef _MSC_VER
        #pragma warning(disable:4251)
        #endif
        Bitvector index; ///Index of the LookupTableEntry
        Bitvector bp; ///BP of the Entry. Empty if index is BP
        Bitvector ancestorMatrix;//Ancestor Matrix
        std::vector<std::vector<Bitvector>> childMatrix; ///Child Matrix
        std::vector<Bitvector> childRanks; ///Child Ranks
        std::vector<Bitvector> parentPointers; ///ParentPointers + 1
        std::vector<Bitvector> degree; ///degree for every node + 1
        std::vector<Bitvector> subTrees; ///subTree for every node within MicroTree (at least 1)
        std::vector<Bitvector> nodeDepths; ///Depths of nodes + 1
        std::vector<Bitvector> nodeHeights; ///Heights of nodes + 1
        std::vector<Bitvector> leaves; ///Amount of Leaves for every node within MicroTree (at least 1)
        std::vector<Bitvector> leftmost_leaf; ///Rightmost leaves for every node within MicroTree
        std::vector<Bitvector> rightmost_leaf; ///Leftmose leaves for every node within MicroTree
        std::vector<Bitvector> leafRank; ///Leaf Rank for every node within MicroTree + 1
        #ifdef _MSC_VER
        #pragma warning(default:4251)
        #endif

        /**
         * Creates new LookupTableEntry with an ID that is otherwise empty
         * @param index the ID of the entry
         */
        explicit LookupTableEntry(Bitvector  index) : index(std::move(index)) {}
        
        /**
         * Creates new LookupTableEntry with an ID and BP-form that is otherwise empty
         * @param index the ID of the entry
         * @param bp balanced parenthesis form of the entry
         */
        LookupTableEntry(Bitvector  index, Bitvector  bp) : index(std::move(index)), bp(std::move(bp)) {}

        /**
         * Compares if index of both LookupTableEntries is equal
         * @param mtd LookupTableEntry to compare This to
         * @return if index == mtd.index
         */
        bool operator==(const LookupTableEntry& mtd) const {
            return index == mtd.index;
        }
    };


    /**
     * This class represents the full hypersuccinct code for any given tree, created by HypersuccinctTreeFactory
     * as specified in 'A Uniform Paradigm to Succinctly Encode Various Families of Trees' by Arash Farzan; J. Ian Munro.
     * All code is represented as Bitvectors
     * It can be encoded with huffman encoding for MicroTrees
     *
     * It contains:
     * The FarzanMunro Algorithm Sizes of MiniTrees and MicroTrees
     * The MiniTrees as a vector of MiniTree
     * The MiniTree FIDs, TypeVectors, and Dummys
     * The LookupTable as a vector of LookUpTableEntry
     *
     * This class implements:
     * Get functions for all Bitvectors
     * Queries as specified in 'A Uniform Paradigm to Succinctly Encode Various Families of Trees' by Arash Farzan; J. Ian Munro
     */
    class PHT_EXPORT HypersuccinctTree {
        friend class HypersuccinctTreeFactory;
    public:

        /**
         * Returns the MiniTree at the given index
         * O(1)
         *
         * @param index The Index of the MiniTree as integer
         * @return MiniTree as MiniTree
         */
        MiniTree& getMiniTree(uint32_t index ) {
            return miniTrees[index];
        }

        /**
         * Checks if the Tree is encoded with Huffman encoding
         * O(1)
         *
         * @return value of huffmanFlag
         */
        [[nodiscard]] bool isHuffman() const {
            return huffmanFlag;
        }

        Bitvector getSize() {
            return size;
        }

        Bitvector getMicroSize() {
            return microSize;
        }

        Bitvector getMiniSize() {
            return miniSize;
        }

        std::vector<MiniTree>& getMiniTrees() {
            return miniTrees;
        }

        std::vector<Bitvector> getMiniFIDs() {
            return miniFIDs;
        }

        std::vector<Bitvector> getFIDTopTrees() {
            return miniFIDTopTree;
        }

        std::vector<Bitvector> getFIDLowTrees() {
            return miniFIDLowTree;
        }

        std::vector<Bitvector> getMiniTypeVectors() {
            return miniTypeVectors;
        }

        std::vector<Bitvector> getMiniDummys() {
            return miniDummys;
        }

        std::vector<LookupTableEntry> getLookupTable() {
            return lookupTable;
        }

        /**
         * Returns the MicroTree Entry of the given MiniTree at the given MicroTree Index
         * Always returns Balanced parenthesis form of the MicroTree, regardless of encoding (Huffman or not)
         *
         * @param miniTree The MiniTree of the MicroTree
         * @param index The index of the MicroTree as int
         * @return the MicroTree in Balanced Parenthesis form as bitvector
         */
        Bitvector getMicroTree(MiniTree& miniTree,uint32_t index);

        /**
         * Returns the amount of MicroTrees in a MiniTree
         * @param miniTree the Minitree to evaluate
         * @return amount of Microtrees as int
         */
        static uint32_t getMicroTreeCount(MiniTree& miniTree);

        /**
         * Returns the MicroFID Entry of the given MiniTree at the given MicroTree Index
         *
         * @param miniTree The MiniTree of the MicroFID
         * @param index The index of the MicroTree as int
         * @return the FID Entry as bitvector
         */
        static Bitvector getMicroFID(MiniTree& miniTree,uint32_t index);

        /**
         * Returns the TypeVector Entry of the given MiniTree at the given MicroTree Index
         *
         * @param miniTree The MiniTree of the TypeVector
         * @param index The index of the MicroTree es int
         * @return the Typevector Entry as bitvector
         */
        static Bitvector getMicroTypeVector(MiniTree& miniTree , uint32_t index);

        /**
         * Returns the MicroDummy Entry from the perspective of the MicroTree
         *
         * @param miniTree The MiniTree of the Dummy
         * @param index The index of the MicroTree as int
         * @return the Dummy Entry as bitvector
         */
        static Bitvector getMicroDummys(MiniTree& miniTree, uint32_t index);

        /**
         * Returns the MicroDummy Pointer Entry for a given MicroTree
         *
         * @param miniTree The MiniTree of the Dummy
         * @param index The index of the MicroTree
         * @return The Dummy Pointed MicroTree as int
         */
        static Bitvector getMicroDummyPointers(MiniTree& miniTree, uint32_t index);

        /**
         * Returns the MiniDummy Entry from the perspective of the MiniTree
         *
         * @param index The index of the MiniTree as int
         * @return The Dummy Entry as bitvector
         */
        Bitvector getMiniDummy(uint32_t index);

        /**
         * Returns the LookupTable Entry at the given index
         *
         * @param index The index as int
         * @return LookupTable Entry as LookupTableEntry
         */
        LookupTableEntry& getLookupTableEntry(uint32_t index) {
            return lookupTable.at(index);
        }



        /**
         * Returns the LookupTable Entry of the given MicroTree
         *
         * @param indexV The MicroTree as bitvector from a miniTree
         * @return LookupTable Entry of the MicroTree as LookupTableEntry
         */
        LookupTableEntry getLookupTableEntry(Bitvector indexV);

        /**
         * Returns if node1 is ancestor of node2 with the given LookupTable Entry
         * Both nodes only need their index inside the MicroTree (HstNode index 2)
         *
         * @param entry The LookupTable Entry
         * @param anc Index of Node 1
         * @param node2Index Index of Node 2
         * @return if node1 is ancestor of node2 as bool
         */
        static bool lookupTableAncestorMatrixComparison(const LookupTableEntry& entry, uint32_t anc, uint32_t node2Index);

        /**
         * Returns if given Node is ancestor of Dummy within the Node's MiniTree
         *
         * @param node The Node as HstNode
         * @return if Node is ancestor of MiniDummy as bool
         */
        bool isDummyAncestorWithinMiniTree(HstNode node);

        /**
         * Returns if  given Node is ancestor of Dummy within the Node's MicroTree
         *
         * @param node  The Node as HstNode
         * @return if Node is ancestor of MicroDummy as bool
         */
        bool isDummyAncestorWithinMicroTree(HstNode node);

        /**
         * Returns the ith child of a given Node, if it exists
         *
         * @param parent The parent Node as HStNode
         * @param index The index as unint32_t
         * @return The ith child as HstNode
         */
        HstNode child(HstNode parent, uint32_t index);

        /**
         * Returns the Child Rank of a given Node
         *
         * @param node The Node as HstNode
         * @return The Child Rank as int
         */
        uint32_t childRank(HstNode node);

        /**
         * Finds the direct Parent of the given Node
         *
         * @param node The node as HSTNode
         * @return The parent as HstNode
         */
        HstNode getParent(HstNode node);

        /**
         * Returns the degree of a given Node
         *
         * @param node The Node as HstNode
         * @return the degree as int
         */
        uint32_t degree(HstNode node);

        /**
         * Returns the Subtree size of a given Node
         *
         * @param node The Node as HstNode
         * @return Subtree size as int
         */
        uint32_t subtreeSize(HstNode node);

        /**
         * Returns the Depth of a given Node
         *
         * @param node The Node as HstNode
         * @return The Depth as uint32_t
         */
        uint32_t depth(HstNode node);

        /**
         * Returns the Depth of a given Node
         *
         * @param node The Node as HstNode
         * @return The Depth as uint32_t
         */
        uint32_t height(HstNode node);

        /**
         * Returns the leftmost leaf of a given Node
         *
         * @param node The Node as HstNode
         * @return The leftmost leaf as HstNode
         */
        HstNode leftmostLeaf(HstNode node);

        /**
         * Returns the rightmost leaf of a given Node
         *
         * @param node The Node as HstNode
         * @return The rightmost leaf as HstNode
         */
        HstNode rightmostLeaf(HstNode node);

        /**
         * Returns the Leaf size of a given Node
         *
         * @param node The Node as HstNode
         * @return the Leaf Size as uint32_t
         */
        uint32_t leafSize(HstNode node);

        /**
         * Returns the Leaf Rank of a given Node
         * Leaf Rank is the amount of Leaves coming before the Node, in node order
         *
         * @param node The Node as HstNode
         * @return the Leaf Size as uint32_t
         */
         uint32_t leafRank(HstNode node);

        /**
         * Returns the size of this Hypersuccinct Tree in bytes. 
         * @return uint64_t The size in bytes. 
         */
        uint64_t getByteSize();

    private:
        HypersuccinctTree() = default;
        bool huffmanFlag = false;
        //sizes
        #ifdef _MSC_VER
        #pragma warning(disable:4251)
        #endif
        std::vector<bool> size;
        std::vector<bool> microSize;
        std::vector<bool> miniSize;
        //miniTrees
        std::vector<MiniTree> miniTrees;
        std::vector<Bitvector> miniFIDs;
        std::vector<succinct_bv::BitVector> miniFIDsSupport;
        //First Top Tree of FIDs +1
        std::vector<Bitvector> miniFIDTopTree;
        //First Low Tree of FIDs +1
        std::vector<Bitvector> miniFIDLowTree;
        std::vector<Bitvector> miniTypeVectors;
        std::vector<succinct_bv::BitVector> miniTypeVectorsSupport;
        std::vector<Bitvector> miniDummys;
        //LookupTable
        std::vector<LookupTableEntry> lookupTable;
        #ifdef _MSC_VER
        #pragma warning(default:4251)
        #endif

        /*
         * Finds the direct Parent of the given Node, ignoring dummies
         * Important helper function
         *
         * @param node The node as HSTNode
         * @return The parent as HstNode
         */
        HstNode getParentForQuery(HstNode node);

        //uint64_t calculateMinitreeByteSize(const MiniTree& mt);

        //uint64_t calculateLookupTableEntryByteSize(const LookupTableEntry& lte);

        //uint64_t succinctBitvectorByteSize(const succinct_bv::BitVector& sbv);

    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
