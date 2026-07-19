#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_HST_NODE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_HST_NODE_H_

#include <cstdint>

#include <chrono>
#include <ostream>

#include "pht_export.h"

namespace pht {
    class HstNode;
}

/**
 * Overrides the <<-operator to allow printing of HstNode. 
 * @param out The stream to print to. 
 * @param node The node to print. 
 * @return std::ostream& The stream, for call-chaining. 
 */
std::ostream &operator<<(std::ostream &out, const pht::HstNode &node);

namespace pht {
    /**
     * This class defines a combination of three IDs to NOT uniquely identify nodes in a Hypersuccinct tree.
     */
    class PHT_EXPORT HstNode {
    public:
        /**
         * Construct a new Hst Node object with IDs 0,0,0. 
         */
        HstNode();

        /**
         * Construct a new Hst Node object with the given IDs.
         * @param mini The ID of the minitree of the node in the whole tree.
         * @param micro The ID of the microtree of the node in the nodes minitree.
         * @param node The ID of of the node in the nodes microtree.
         */
        HstNode(uint32_t mini, uint32_t micro, uint32_t node);

        /**
         * Compares to HstNodes for (numerical) equivalence. 
         * @param b The node to compare to. 
         * @return true If the IDs in both nodes are the same. 
         * @return false If the IDs in both nodes are not the same. 
         */
        bool operator==(const HstNode &b) const;

        /**
         * Compares to HstNodes for (numerical) discrepancy. 
         * @param b The node to compare to. 
         * @return true If the IDs in both nodes are not the same. 
         * @return false If the IDs in both nodes are the same. 
         */
        bool operator!=(const HstNode &b) const;

        uint32_t mini; ///The ID of the minitree of the node in the whole tree.
        uint32_t micro; ///The ID of the microtree of the node in the nodes minitree.
        uint32_t node; ///The ID of of the node in the nodes microtree.
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_HST_NODE_H_
