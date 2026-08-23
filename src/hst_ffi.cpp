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
#include <cstdint>

/* ================================================================== */
/*  Handle wrapper                                                     */
/* ================================================================== */

/**
 * The FFI handle owns the HypersuccinctTree plus the balanced-parenthesis
 * input it was built from.  Keeping the BP around makes serialization a
 * lossless roundtrip: serialize() emits the BP + construction flags and
 * deserialize() rebuilds an equivalent tree through the factory.
 */
struct HstTreeWrapper {
    std::unique_ptr<pht::HypersuccinctTree> tree;
    std::vector<bool> bp;
    uint8_t huffman;
    uint32_t size_mini;
    uint32_t size_micro;
};

namespace {

inline HstTreeWrapper* wrapper_of(HstTreeHandle handle) {
    return static_cast<HstTreeWrapper*>(handle);
}

inline pht::HypersuccinctTree* unwrap_tree(HstTreeHandle handle) {
    return wrapper_of(handle)->tree.get();
}

/* Sentinel node meaning "no such node" (parent of root, OOB child...).
 * Must NOT collide with the root, which lives at {0, 0, 0}. */
constexpr HstNode kInvalidNode{UINT32_MAX, UINT32_MAX, UINT32_MAX};

}  // namespace


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

/* Required capacity in bytes for serializing `handle`.
 * Format v2 stores the construction inputs (BP bitvector + flags):
 *   [0..3]   magic "HST" + version byte
 *   [4]      huffman flag
 *   [5..8]   size_mini (LE)
 *   [9..12]  size_micro (LE)
 *   [13..16] bp_len (LE)
 *   [17..]   BP bits packed MSB-first into bytes
 *
 * It is intentionally NOT the compressed HST representation; keeping the
 * source BP makes the serialize/deserialize roundtrip lossless while
 * hst_tree_byte_size() remains the measure of encoded structure size.
 */
static uint32_t serialized_size(const HstTreeWrapper* w) {
    return 17u + static_cast<uint32_t>((w->bp.size() + 7) / 8);
}

/* Shared construction path used by both hst_tree_create_from_bp() and
 * hst_tree_deserialize(). Returns a fresh wrapper or nullptr. */
static HstTreeHandle build_hst(const std::vector<bool>& bp_vec,
                               uint8_t huffman,
                               uint32_t size_mini,
                               uint32_t size_micro) {
    if (bp_vec.empty()) return nullptr;

    /* Build intermediate UnorderedTree */
    auto tree = bp_to_unordered_tree(bp_vec);
    if (tree->isEmpty()) return nullptr;

    auto hst = pht::HypersuccinctTreeFactory::create(
        tree,
        huffman != 0,
        size_mini,
        size_micro,
        true  /* doQueries */
    );

    auto* wrapper = new HstTreeWrapper{
        std::move(hst),
        bp_vec,
        huffman,
        size_mini,
        size_micro
    };
    return static_cast<HstTreeHandle>(wrapper);
}

/* The FFI target is built with CXX_VISIBILITY_PRESET hidden; re-enable
 * default visibility so the C API below is exported from libhst_ffi.so. */
#pragma GCC visibility push(default)
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

    return build_hst(bp_vec, huffman != 0 ? 1u : 0u, size_mini, size_micro);
}

void hst_tree_free(HstTreeHandle handle) {
    if (handle == nullptr) return;
    delete wrapper_of(handle);
}

/* ================================================================== */
/*  Metadata queries                                                    */
/* ================================================================== */

uint32_t hst_tree_node_count(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
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
    auto* hst = unwrap_tree(handle);
    return hst->getByteSize();
}

int hst_tree_is_huffman(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    return hst->isHuffman() ? 1 : 0;
}

uint32_t hst_tree_minitree_count(HstTreeHandle handle) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    return static_cast<uint32_t>(hst->getMiniTrees().size());
}

/* ================================================================== */
/*  Navigation queries                                                  */
/* ================================================================== */

HstNode hst_tree_root(HstTreeHandle handle) {
    if (handle == nullptr) return kInvalidNode;
    /* The root is always at MiniTree 0, MicroTree 0, node 0.
     * This does NOT collide with kInvalidNode (all-UINT32_MAX). */
    return {0, 0, 0};
}

HstNode hst_tree_parent(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return kInvalidNode;
    /* The root has no parent.  Querying it would underflow inside
     * HypersuccinctTree::getParent (miniParent = 0 - 1), so guard here. */
    if (node.mini == 0 && node.micro == 0 && node.node == 0) {
        return kInvalidNode;
    }
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    pht::HstNode parent = hst->getParent(cpp_node);
    return {parent.mini, parent.micro, parent.node};
}

HstNode hst_tree_child(HstTreeHandle handle, HstNode parent, uint32_t index) {
    if (handle == nullptr) return kInvalidNode;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_parent{parent.mini, parent.micro, parent.node};
    if (index >= hst->degree(cpp_parent)) {
        return kInvalidNode;
    }
    pht::HstNode child = hst->child(cpp_parent, index);
    return {child.mini, child.micro, child.node};
}

uint32_t hst_tree_child_rank(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->childRank(cpp_node);
}

uint32_t hst_tree_degree(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->degree(cpp_node);
}

/* ================================================================== */
/*  Structural queries                                                  */
/* ================================================================== */

uint32_t hst_tree_depth(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->depth(cpp_node);
}

uint32_t hst_tree_height(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->height(cpp_node);
}

uint32_t hst_tree_subtree_size(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->subtreeSize(cpp_node);
}

uint32_t hst_tree_leaf_size(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->leafSize(cpp_node);
}

uint32_t hst_tree_leaf_rank(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    return hst->leafRank(cpp_node);
}

HstNode hst_tree_leftmost_leaf(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return kInvalidNode;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    pht::HstNode leaf = hst->leftmostLeaf(cpp_node);
    return {leaf.mini, leaf.micro, leaf.node};
}

HstNode hst_tree_rightmost_leaf(HstTreeHandle handle, HstNode node) {
    if (handle == nullptr) return kInvalidNode;
    auto* hst = unwrap_tree(handle);
    pht::HstNode cpp_node{node.mini, node.micro, node.node};
    pht::HstNode leaf = hst->rightmostLeaf(cpp_node);
    return {leaf.mini, leaf.micro, leaf.node};
}

/* ================================================================== */
/*  Ancestry query                                                      */
/* ================================================================== */

int hst_tree_is_ancestor(HstTreeHandle handle, HstNode ancestor, HstNode descendant) {
    if (handle == nullptr) return 0;
    auto* hst = unwrap_tree(handle);

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
    if (handle == nullptr) return 0;
    auto* w = wrapper_of(handle);

    uint32_t needed = serialized_size(w);
    if (buf == nullptr) return needed;      /* capacity query */
    if (buf_capacity < needed) return 0;

    uint32_t offset = 0;

    /* Magic + version */
    buf[offset++] = 'H';
    buf[offset++] = 'S';
    buf[offset++] = 'T';
    buf[offset++] = 0x02;

    /* Construction parameters */
    buf[offset++] = w->huffman;

    auto write_u32 = [&buf, &offset](uint32_t v) {
        buf[offset++] = static_cast<uint8_t>((v      ) & 0xFF);
        buf[offset++] = static_cast<uint8_t>((v >>  8) & 0xFF);
        buf[offset++] = static_cast<uint8_t>((v >> 16) & 0xFF);
        buf[offset++] = static_cast<uint8_t>((v >> 24) & 0xFF);
    };
    write_u32(w->size_mini);
    write_u32(w->size_micro);

    uint32_t n = static_cast<uint32_t>(w->bp.size());
    write_u32(n);

    for (uint32_t i = 0; i < n; i += 8) {
        uint8_t byte = 0;
        for (int j = 0; j < 8 && (i + j) < n; j++) {
            byte |= (w->bp[i + j] ? static_cast<uint8_t>(1u << (7 - j)) : 0u);
        }
        buf[offset++] = byte;
    }

    return offset;
}

HstTreeHandle hst_tree_deserialize(const uint8_t* buf, uint32_t len) {
    if (buf == nullptr || len < 17) return nullptr;

    /* Verify magic + version */
    if (buf[0] != 'H' || buf[1] != 'S' || buf[2] != 'T' || buf[3] != 0x02) {
        return nullptr;
    }

    uint32_t offset = 4;
    uint8_t huffman = buf[offset++];

    auto read_u32 = [&buf, &offset]() {
        uint32_t v = static_cast<uint32_t>(buf[offset])
                   | (static_cast<uint32_t>(buf[offset + 1]) << 8)
                   | (static_cast<uint32_t>(buf[offset + 2]) << 16)
                   | (static_cast<uint32_t>(buf[offset + 3]) << 24);
        offset += 4;
        return v;
    };
    uint32_t size_mini = read_u32();
    uint32_t size_micro = read_u32();
    uint32_t n = read_u32();

    if (n == 0 || len < offset + (n + 7) / 8) return nullptr;

    std::vector<bool> bp_vec(n);
    for (uint32_t i = 0; i < n; i++) {
        uint8_t byte = buf[offset + i / 8];
        bp_vec[i] = ((byte >> (7 - (i % 8))) & 1u) != 0;
    }

    return build_hst(bp_vec, huffman, size_mini, size_micro);
}


/* ================================================================== */
/*  Utility helpers                                                     */
/* ================================================================== */

int hst_node_equal(HstNode a, HstNode b) {
    return (a.mini == b.mini && a.micro == b.micro && a.node == b.node) ? 1 : 0;
}

HstNode hst_node_invalid(void) {
    return kInvalidNode;
}

} /* extern "C" */
#pragma GCC visibility pop