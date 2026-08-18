/**
 * @file hst_ffi.cpp
 * @brief C-compatible FFI implementation for HyperSuccinct Tree.
 *
 * Wraps the C++ HST API so that foreign languages (Rust via bindgen,
 * Python via cffi, etc.) can construct and query HST instances.
 */

#include "hst_ffi.h"

#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/hst_output.h"
#include "pht/unordered_tree.h"
#include "pht/node.h"
#include "pht/xml_reader.h"

#include <memory>
#include <cstring>
#include <cmath>
#include <algorithm>

/* ================================================================== */
/*  Internal helpers                                                   */
/* ================================================================== */

/**
 * Build an UnorderedTree<std::string> from a balanced-parenthesis bitvector.
 *
 * The BP encoding:  true  = '(',  false = ')'
 * We perform a single DFS pass: every 'true' pushes a new node, every
 * 'false' pops back to the parent.
 */
static std::shared_ptr<pht::UnorderedTree<std::string>>
bp_to_unordered_tree(const std::vector<bool>& bp) {
    auto tree = std::make_shared<pht::UnorderedTree<std::string>>();
    if (bp.empty()) {
        return tree;
    }

    // Stack of (node, children_processed) for DFS reconstruction
    struct Frame {
        std::shared_ptr<pht::Node<std::string>> node;
    };
    std::vector<Frame> stack;

    size_t idx = 0;
    // First bit must be an opening parenthesis (root)
    {
        auto root = std::make_shared<pht::Node<std::string>>(
            std::to_string(idx));
        tree->add(root);
        stack.push_back({root});
        idx++;
    }

    while (idx < bp.size()) {
        if (bp[idx]) {
            // Opening parenthesis: new child of current top
            auto parent = stack.back().node;
            auto child = std::make_shared<pht::Node<std::string>>(
                std::to_string(idx));
            tree->add(child, parent);
            stack.push_back({child});
        } else {
            // Closing parenthesis: pop
            if (!stack.empty()) {
                stack.pop_back();
            }
        }
        idx++;
    }

    return tree;
}

/* ================================================================== */
/*  Construction / Destruction                                         */
/* ================================================================== */

extern "C" {

HstTreeHandle hst_tree_create_from_bp(
    const int* bp, size_t bp_len,
    int huffman,
    uint32_t size_mini,
    uint32_t size_micro)
{
    if (bp == nullptr || bp_len == 0) {
        return nullptr;
    }

    /* Convert int array to std::vector<bool> */
    std::vector<bool> bp_vec(bp_len);
    for (size_t i = 0; i < bp_len; i++) {
        bp_vec[i] = (bp[i] != 0);
    }

    /* Build intermediate UnorderedTree */
    auto tree = bp_to_unordered_tree(bp_vec);
    if (tree->isEmpty()) {
        return nullptr;
    }

    /* Create HST via factory */
    auto hst = pht::HypersuccinctTreeFactory::create(
        tree,
        huffman != 0,
        size_mini,
        size_micro,
        true  /* doQueries */
    );

    return static_cast<HstTreeHandle>(hst.release());
}

void hst_tree_free(HstTreeHandle handle) {
    if (handle == nullptr) return;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    delete hst;
}

/* ================================================================== */
/*  Metadata queries                                                    */
/* ================================================================== */

uint32_t hst_tree_node_count(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    /* Decode the size bitvector (binary encoding of node count) */
    const auto& size_bv = hst->getSize();
    if (size_bv.empty()) return 0;
    // Binary-encoded uint32: reconstruct from bits (MSB first)
    uint32_t result = 0;
    for (bool bit : size_bv) {
        result = (result << 1) | (bit ? 1u : 0u);
    }
    return result;
}

uint64_t hst_tree_byte_size(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    return hst->getByteSize();
}

int hst_tree_is_huffman(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    return hst->isHuffman() ? 1 : 0;
}

uint32_t hst_tree_minitree_count(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    return static_cast<uint32_t>(hst->getMiniTrees().size());
}

/* ================================================================== */
/*  Navigation queries                                                  */
/* ================================================================== */

HstNode hst_tree_root(HstTreeHandle handle) {
    if (handle == nullptr) return {0, 0, 0};
    /* The root is always at MiniTree 0, MicroTree 0, node 0 */
    return {0, 0, 0};
}

HstNode hst_tree_parent(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return {0, 0, 0};
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    pht::HstNode parent = hst->getParent(cpp_node);
    return {parent.mini, parent.micro, parent.node};
}

HstNode hst_tree_child(HstTreeHandle handle, HstNode parent, uint32_t index) {
    if (handle == nullptr) return {0, 0, 0};
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_parent{parent.mini, parent.micro, parent.node};
    pht::HstNode child = hst->child(cpp_parent, index);
    return {child.mini, child.micro, child.node};
}

uint32_t hst_tree_child_rank(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->childRank(cpp_node);
}

uint32_t hst_tree_degree(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->degree(cpp_node);
}

/* ================================================================== */
/*  Structural queries                                                  */
/* ================================================================== */

uint32_t hst_tree_depth(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->depth(cpp_node);
}

uint32_t hst_tree_height(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->height(cpp_node);
}

uint32_t hst_tree_subtree_size(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->subtreeSize(cpp_node);
}

uint32_t hst_tree_leaf_size(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->leafSize(cpp_node);
}

uint32_t hst_tree_leaf_rank(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->leafRank(cpp_node);
}

HstNode hst_tree_leftmost_leaf(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return {0, 0, 0};
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    pht::HstNode leaf = hst->leftmostLeaf(cpp_node);
    return {leaf.mini, leaf.micro, leaf.node};
}

HstNode hst_tree_rightmost_leaf(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return {0, 0, 0};
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    pht::HstNode leaf = hst->rightmostLeaf(cpp_node);
    return {leaf.mini, leaf.micro, leaf.node};
}

/* ================================================================== */
/*  Ancestry query                                                      */
/* ================================================================== */

int hst_tree_is_ancestor(HstTreeHandle handle, HstNode ancestor, HstNode descendant) {
    if (handle == nullptr) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);

    pht::HstNode anc{ancestor.mini, ancestor.micro, ancestor.node};
    pht::HstNode desc{descendant.mini, descendant.micro, descendant.node};

    /* Strategy: walk from descendant toward root and check equality.
     * HST does not expose a direct is_ancestor method, so we implement
     * it by traversing the parent chain.  For true O(1) ancestry we
     * would need to use the ancestor matrix in the lookup table, but
     * that only works within a single MicroTree.  The parent-chain walk
     * is correct for all cases. */

    pht::HstNode current = desc;
    while (true) {
        if (current.mini == anc.mini &&
            current.micro == anc.micro &&
            current.node == anc.node) {
            return 1;
        }
        /* Stop if we've reached the root and it didn't match */
        if (current.mini == 0 && current.micro == 0 && current.node == 0) {
            break;
        }
        pht::HstNode parent = hst->getParent(current);
        /* Detect cycles / invalid state */
        if (parent.mini == current.mini &&
            parent.micro == current.micro &&
            parent.node == current.node) {
            break;
        }
        current = parent;
    }
    return 0;
}

/* ================================================================== */
/*  Serialization / Deserialization                                     */
/* ================================================================== */

uint32_t hst_tree_serialize(HstTreeHandle handle, uint8_t* buf, uint32_t buf_capacity) {
    if (handle == nullptr || buf == nullptr || buf_capacity == 0) return 0;
    auto* hst = static_cast<pht::HypersuccinctTree*>(handle);
    uint64_t expected = hst->getByteSize();
    if (expected > buf_capacity) return 0;

    /* Use the existing write-to-file format but write to a raw buffer.
     * For simplicity we write a length-prefixed binary blob:
     *   [4 bytes: magic "HST\x00"]
     *   [4 bytes: total payload length]
     *   [payload: bitvector-based serialization]
     *
     * We reuse HypersuccinctTreeOutput::writeToFile format by writing
     * to a temporary file and reading it back, OR we implement a
     * direct binary serialization here.
     *
     * For now, implement a simple direct serialization:
     * Write the huffman flag, sizes, and all bitvectors.
     */

    uint32_t offset = 0;

    /* Magic */
    buf[offset++] = 'H';
    buf[offset++] = 'S';
    buf[offset++] = 'T';
    buf[offset++] = 0;

    /* We'll compute the actual size and write a placeholder */
    uint32_t header_start = offset;
    offset += 4;  /* placeholder for payload length */

    uint32_t payload_start = offset;

    /* Huffman flag */
    buf[offset++] = hst->isHuffman() ? 1 : 0;

    /* Size bitvectors (binary-encoded uint32) */
    auto write_bitvector = [&buf, &offset](const std::vector<bool>& bv) {
        /* Write length as uint32 (little-endian) then bits packed into bytes */
        uint32_t len = static_cast<uint32_t>(bv.size());
        buf[offset++] =  static_cast<uint8_t>((len      ) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((len >>  8) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((len >> 16) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((len >> 24) & 0xFF);

        for (uint32_t i = 0; i < len; i += 8) {
            uint8_t byte = 0;
            for (int j = 0; j < 8 && (i + j) < len; j++) {
                byte |= (bv[i + j] ? (1u << (7 - j)) : 0u);
            }
            buf[offset++] = byte;
        }
    };

    auto write_bitvector_vec = [&buf, &offset, &write_bitvector](const std::vector<std::vector<bool>>& vec) {
        uint32_t count = static_cast<uint32_t>(vec.size());
        buf[offset++] =  static_cast<uint8_t>((count      ) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >>  8) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >> 16) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >> 24) & 0xFF);
        for (const auto& bv : vec) {
            write_bitvector(bv);
        }
    };

    /* Write top-level sizes */
    write_bitvector(hst->getSize());
    write_bitvector(hst->getMicroSize());
    write_bitvector(hst->getMiniSize());

    /* Write interconnections */
    write_bitvector_vec(hst->getMiniFIDs());
    write_bitvector_vec(hst->getFIDTopTrees());
    write_bitvector_vec(hst->getFIDLowTrees());
    write_bitvector_vec(hst->getMiniTypeVectors());
    write_bitvector_vec(hst->getMiniDummys());

    /* Write MiniTrees */
    {
        const auto& mini_trees = hst->getMiniTrees();
        uint32_t count = static_cast<uint32_t>(mini_trees.size());
        buf[offset++] =  static_cast<uint8_t>((count      ) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >>  8) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >> 16) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >> 24) & 0xFF);

        for (const auto& mt : mini_trees) {
            write_bitvector_vec(mt.FIDs);
            write_bitvector_vec(mt.FIDsSupport);  /* succinct bv - skip for now, write empty */
            write_bitvector_vec(mt.typeVectors);
            write_bitvector_vec(mt.typeVectorsSupport);
            write_bitvector_vec(mt.dummys);
            write_bitvector_vec(mt.microTrees);

            write_bitvector(mt.miniTopFIDIndex);
            write_bitvector(mt.miniLowFIDIndex);
            write_bitvector_vec(mt.microTopFIDIndices);
            write_bitvector_vec(mt.microLowFIDIndices);
            write_bitvector_vec(mt.microFIDTopTrees);
            write_bitvector_vec(mt.microFIDLowTrees);

            write_bitvector(mt.rootAncestors);
            write_bitvector(mt.dummyAncestors);
            write_bitvector(mt.miniDummyTree);
            write_bitvector(mt.miniDummyIndex);
            write_bitvector(mt.miniDummyPointer);
            write_bitvector_vec(mt.microDummyPointers);

            write_bitvector(mt.miniChildRank);
            write_bitvector_vec(mt.microChildRanks);
            write_bitvector_vec(mt.microExtendedChildRanks);

            write_bitvector(mt.miniParent);
            write_bitvector_vec(mt.microParents);

            write_bitvector(mt.subTree);
            write_bitvector_vec(mt.microSubTrees);
            write_bitvector(mt.miniDepth);
            write_bitvector(mt.miniHeight);
            write_bitvector(mt.miniDummyDepth);
            write_bitvector(mt.miniDummyHeight);
            write_bitvector_vec(mt.rootDepths);
            write_bitvector_vec(mt.rootHeights);
            write_bitvector(mt.miniLeaves);
            write_bitvector_vec(mt.microLeaves);
            write_bitvector(mt.miniTreeLeftmostLeafPointer);
            write_bitvector(mt.miniTreeRightmostLeafPointer);
            write_bitvector_vec(mt.microTreeLeftmostLeafPointers);
            write_bitvector_vec(mt.microTreeRightmostLeafPointers);
            write_bitvector(mt.miniRootLeafRank);
            write_bitvector(mt.miniDummyLeafRank);
            write_bitvector_vec(mt.microRootLeafRanks);
            write_bitvector_vec(mt.microExtendedLeafRanks);
        }
    }

    /* Write Lookup Table */
    {
        const auto& lt = hst->getLookupTable();
        uint32_t count = static_cast<uint32_t>(lt.size());
        buf[offset++] =  static_cast<uint8_t>((count      ) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >>  8) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >> 16) & 0xFF);
        buf[offset++] =  static_cast<uint8_t>((count >> 24) & 0xFF);

        for (const auto& entry : lt) {
            write_bitvector(entry.index);
            write_bitvector(entry.bp);
            write_bitvector(entry.ancestorMatrix);
            write_bitvector_vec(entry.childMatrix);
            write_bitvector_vec(entry.childRanks);
            write_bitvector_vec(entry.parentPointers);
            write_bitvector_vec(entry.degree);
            write_bitvector_vec(entry.subTrees);
            write_bitvector_vec(entry.nodeDepths);
            write_bitvector_vec(entry.nodeHeights);
            write_bitvector_vec(entry.leaves);
            write_bitvector_vec(entry.leftmost_leaf);
            write_bitvector_vec(entry.rightmost_leaf);
            write_bitvector_vec(entry.leafRank);
        }
    }

    /* Fill in payload length */
    uint32_t payload_len = offset - payload_start;
    header_start = offset - (payload_start - header_start) - 4;
    buf[header_start]     =  static_cast<uint8_t>((payload_len      ) & 0xFF);
    buf[header_start + 1] =  static_cast<uint8_t>((payload_len >>  8) & 0xFF);
    buf[header_start + 2] =  static_cast<uint8_t>((payload_len >> 16) & 0xFF);
    buf[header_start + 3] =  static_cast<uint8_t>((payload_len >> 24) & 0xFF);

    return offset;
}

HstTreeHandle hst_tree_deserialize(const uint8_t* buf, uint32_t len) {
    if (buf == nullptr || len < 8) return nullptr;

    /* Verify magic */
    if (buf[0] != 'H' || buf[1] != 'S' || buf[2] != 'T' || buf[3] != 0) {
        return nullptr;
    }

    /* For now, deserialization is a TODO - the format is complex.
     * Return nullptr to indicate "not yet implemented".
     * In production, this would mirror the serialization logic. */
    (void)len;  /* suppress unused warning */
    return nullptr;
}

/* ================================================================== */
/*  Utility helpers                                                     */
/* ================================================================== */

int hst_node_equal(HstNode a, HstNode b) {
    return (a.mini == b.mini && a.micro == b.micro && a.node == b.node) ? 1 : 0;
}

HstNode hst_node_invalid(void) {
    return {0, 0, 0};
}

} /* extern "C" */