/* Standalone C harness to exercise the hst_ffi C API against the
 * already-built libhst_ffi.so. Confirms the known bugs:
 *   - hst_tree_node_count returns MicroTree count, not node count
 *   - hst_tree_root returns {0,0,0} which collides with the invalid sentinel
 */
#include <stdio.h>
#include <stdlib.h>
#include "hst_ffi.h"

int main(void) {
    /* Tree: (A (B) (C (D)))
     * BP:    (  (  )  (  (  )  )  )
     *         A  B    C  D
     * 8 bits, 4 nodes.
     */
    int bp[8] = {1, 1, 0, 1, 1, 0, 0, 0};

    HstTreeHandle h = hst_tree_create_from_bp(bp, 8, 0, 0, 0);
    if (h == NULL) {
        fprintf(stderr, "create_from_bp returned NULL\n");
        return 1;
    }

    printf("node_count        = %u  (expected 4)\n", hst_tree_node_count(h));
    printf("minitree_count    = %u\n", hst_tree_minitree_count(h));
    printf("byte_size         = %llu\n", (unsigned long long)hst_tree_byte_size(h));
    printf("is_huffman        = %d\n", hst_tree_is_huffman(h));

    HstNode root = hst_tree_root(h);
    printf("root = {mini=%u, micro=%u, node=%u}\n", root.mini, root.micro, root.node);
    printf("root == invalid?  = %d  (BUG if 1: root collides with sentinel)\n",
           hst_node_equal(root, hst_node_invalid()));

    printf("depth(root)       = %u  (expected 0)\n", hst_tree_depth(h, root));
    printf("degree(root)      = %u  (expected 2)\n", hst_tree_degree(h, root));

    HstNode b = hst_tree_child(h, root, 0);
    HstNode c = hst_tree_child(h, root, 1);
    HstNode d = hst_tree_child(h, c, 0);
    printf("child(root,0) B   = {mini=%u, micro=%u, node=%u}, degree=%u (expected 0)\n",
           b.mini, b.micro, b.node, hst_tree_degree(h, b));
    printf("child(root,1) C   = {mini=%u, micro=%u, node=%u}, degree=%u (expected 1)\n",
           c.mini, c.micro, c.node, hst_tree_degree(h, c));
    printf("child(C,0)    D   = {mini=%u, micro=%u, node=%u}, degree=%u (expected 0)\n",
           d.mini, d.micro, d.node, hst_tree_degree(h, d));

    printf("is_ancestor(root,D) = %d (expected 1)\n", hst_tree_is_ancestor(h, root, d));
    printf("is_ancestor(C,D)    = %d (expected 1)\n", hst_tree_is_ancestor(h, c, d));
    printf("is_ancestor(D,root) = %d (expected 0)\n", hst_tree_is_ancestor(h, d, root));

    printf("parent(root)     = {mini=%u, micro=%u, node=%u} (root's parent is invalid)\n",
           hst_tree_parent(h, root).mini, hst_tree_parent(h, root).micro, hst_tree_parent(h, root).node);

    /* Out-of-range child: should return invalid {0,0,0} per header contract */
    HstNode oob = hst_tree_child(h, root, 5);
    printf("child(root,5) OOB = {mini=%u, micro=%u, node=%u}, ==invalid? %d\n",
           oob.mini, oob.micro, oob.node, hst_node_equal(oob, hst_node_invalid()));

    /* Serialization roundtrip */
    uint64_t bs = hst_tree_byte_size(h);
    uint8_t *buf = (uint8_t *)malloc(bs ? bs : 1);
    uint32_t written = hst_tree_serialize(h, buf, (uint32_t)bs);
    printf("serialize wrote   = %u bytes (byte_size=%llu)\n", written, (unsigned long long)bs);
    HstTreeHandle h2 = hst_tree_deserialize(buf, written);
    printf("deserialize       = %p  (BUG if NULL: not implemented)\n", (void *)h2);
    if (h2) {
        printf("restored node_count = %u\n", hst_tree_node_count(h2));
        hst_tree_free(h2);
    }
    free(buf);
    hst_tree_free(h);
    return 0;
}