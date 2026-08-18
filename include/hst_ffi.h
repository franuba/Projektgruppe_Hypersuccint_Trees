/**
 * @file hst_ffi.h
 * @brief C-compatible Foreign Function Interface for the HyperSuccinct Tree library.
 *
 * This header provides a thin C wrapper around the C++ HST classes so that
 * languages like Rust (via bindgen + libloading) can call the HST queries
 * and construction routines safely through opaque handles.
 *
 * Design principles:
 * - All C++ types are exposed as opaque void* handles
 * - HstNode is a plain struct (trivially copyable across FFI boundary)
 * - Every allocated handle has a corresponding _free function
 * - Boolean results are returned as int (0 = false, 1 = true)
 */

#ifndef HST_FFI_H
#define HST_FFI_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Node identifier                                                    */
/* ------------------------------------------------------------------ */

/**
 * Identifies a node inside a HyperSuccinct Tree.
 *
 * A HST partitions a tree into MiniTrees, each MiniTree into MicroTrees,
 * and each MicroTree contains a small set of nodes.  The three-level
 * index (mini, micro, node) uniquely locates every node.
 */
typedef struct {
    uint32_t mini;   ///< Index of the MiniTree within the HST
    uint32_t micro;  ///< Index of the MicroTree within the MiniTree
    uint32_t node;   ///< Index of the node within the MicroTree
} HstNode;

/* ------------------------------------------------------------------ */
/*  Opaque handles                                                     */
/* ------------------------------------------------------------------ */

/** Opaque handle to a HypersuccinctTree instance. */
typedef void* HstTreeHandle;

/* ------------------------------------------------------------------ */
/*  Construction / Destruction                                         */
/* ------------------------------------------------------------------ */

/**
 * Create a HyperSuccinct Tree from a balanced-parenthesis bitvector.
 *
 * The BP encoding uses:  true  = opening parenthesis '(',
 *                        false = closing parenthesis ')'
 *
 * @param bp          Pointer to the bitvector (array of int: 0 or 1).
 * @param bp_len      Number of bits in the bitvector.
 * @param huffman     1 to enable Huffman encoding of MicroTrees, 0 otherwise.
 * @param size_mini   MiniTree size (0 = auto-calculated as log^2(n)).
 * @param size_micro  MicroTree size (0 = auto-calculated as log(n)/8).
 * @return            Handle to the newly created HST, or NULL on failure.
 */
HstTreeHandle hst_tree_create_from_bp(
    const int* bp, size_t bp_len,
    int huffman,
    uint32_t size_mini,
    uint32_t size_micro);

/**
 * Free a HyperSuccinct Tree handle.
 * @param handle  Handle returned by hst_tree_create_from_bp or hst_tree_deserialize.
 */
void hst_tree_free(HstTreeHandle handle);

/* ------------------------------------------------------------------ */
/*  Metadata queries                                                    */
/* ------------------------------------------------------------------ */

/**
 * Return the total number of nodes in the original tree.
 * @param handle  HST handle.
 * @return        Node count.
 */
uint32_t hst_tree_node_count(HstTreeHandle handle);

/**
 * Return the encoded size of the HST in bytes.
 * @param handle  HST handle.
 * @return        Size in bytes.
 */
uint64_t hst_tree_byte_size(HstTreeHandle handle);

/**
 * Check whether the HST was encoded with Huffman encoding.
 * @param handle  HST handle.
 * @return        1 if Huffman-encoded, 0 otherwise.
 */
int hst_tree_is_huffman(HstTreeHandle handle);

/**
 * Return the number of MiniTrees in the HST.
 * @param handle  HST handle.
 * @return        MiniTree count.
 */
uint32_t hst_tree_minitree_count(HstTreeHandle handle);

/* ------------------------------------------------------------------ */
/*  Navigation queries  (all O(1))                                     */
/* ------------------------------------------------------------------ */

/**
 * Return the root node of the tree.
 * @param handle  HST handle.
 * @return        HstNode for the root.
 */
HstNode hst_tree_root(HstTreeHandle handle);

/**
 * Return the direct parent of a node.
 * @param handle  HST handle.
 * @param node    Node whose parent to look up.
 * @return        Parent HstNode.  If the node IS the root, returns {0,0,0}.
 */
HstNode hst_tree_parent(HstTreeHandle handle, HstNode node);

/**
 * Return the i-th child of a parent node.
 * @param handle  HST handle.
 * @param parent  Parent node.
 * @param index   Zero-based child index.
 * @return        Child HstNode.  Returns {0,0,0} if index >= degree.
 */
HstNode hst_tree_child(HstTreeHandle handle, HstNode parent, uint32_t index);

/**
 * Return the child rank of a node (zero-based index among siblings).
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Child rank.
 */
uint32_t hst_tree_child_rank(HstTreeHandle handle, HstNode node);

/**
 * Return the degree (number of children) of a node.
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Degree.
 */
uint32_t hst_tree_degree(HstTreeHandle handle, HstNode node);

/* ------------------------------------------------------------------ */
/*  Structural queries  (all O(1))                                     */
/* ------------------------------------------------------------------ */

/**
 * Return the depth of a node (edges from root).
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Depth.
 */
uint32_t hst_tree_depth(HstTreeHandle handle, HstNode node);

/**
 * Return the height of a node (longest path to a leaf).
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Height.
 */
uint32_t hst_tree_height(HstTreeHandle handle, HstNode node);

/**
 * Return the subtree size of a node (total descendants + self).
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Subtree size.
 */
uint32_t hst_tree_subtree_size(HstTreeHandle handle, HstNode node);

/**
 * Return the number of leaves in the subtree rooted at a node.
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Leaf count.
 */
uint32_t hst_tree_leaf_size(HstTreeHandle handle, HstNode node);

/**
 * Return the leaf rank of a node (number of leaves before it in left-to-right order).
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        Leaf rank.
 */
uint32_t hst_tree_leaf_rank(HstTreeHandle handle, HstNode node);

/**
 * Return the leftmost leaf in the subtree of a node.
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        HstNode of the leftmost leaf.
 */
HstNode hst_tree_leftmost_leaf(HstTreeHandle handle, HstNode node);

/**
 * Return the rightmost leaf in the subtree of a node.
 * @param handle  HST handle.
 * @param node    Target node.
 * @return        HstNode of the rightmost leaf.
 */
HstNode hst_tree_rightmost_leaf(HstTreeHandle handle, HstNode node);

/* ------------------------------------------------------------------ */
/*  Ancestry query  (O(1))                                             */
/* ------------------------------------------------------------------ */

/**
 * Check whether `ancestor` is an ancestor of `descendant`.
 *
 * @param handle      HST handle.
 * @param ancestor    Potential ancestor node.
 * @param descendant  Potential descendant node.
 * @return            1 if ancestor is an ancestor of descendant (or they are equal),
 *                    0 otherwise.
 */
int hst_tree_is_ancestor(HstTreeHandle handle, HstNode ancestor, HstNode descendant);

/* ------------------------------------------------------------------ */
/*  Serialization / Deserialization                                     */
/* ------------------------------------------------------------------ */

/**
 * Serialize an HST to a byte buffer.
 *
 * @param handle       HST handle.
 * @param buf          Output buffer (caller-allocated).
 * @param buf_capacity Capacity of the buffer in bytes.
 * @return             Number of bytes written, or 0 on failure
 *                     (buffer too small; call hst_tree_byte_size first).
 */
uint32_t hst_tree_serialize(HstTreeHandle handle, uint8_t* buf, uint32_t buf_capacity);

/**
 * Deserialize an HST from a byte buffer.
 *
 * @param buf  Pointer to serialized bytes.
 * @param len  Number of bytes.
 * @return     Handle to the deserialized HST, or NULL on failure.
 */
HstTreeHandle hst_tree_deserialize(const uint8_t* buf, uint32_t len);

/* ------------------------------------------------------------------ */
/*  Utility helpers                                                    */
/* ------------------------------------------------------------------ */

/**
 * Check if two HstNodes are equal.
 * @param a First node.
 * @param b Second node.
 * @return  1 if equal, 0 otherwise.
 */
int hst_node_equal(HstNode a, HstNode b);

/**
 * Return a zero-initialized (invalid) HstNode.
 */
HstNode hst_node_invalid(void);

#ifdef __cplusplus
}
#endif

#endif /* HST_FFI_H */