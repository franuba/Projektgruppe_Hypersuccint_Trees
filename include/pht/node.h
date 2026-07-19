#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_

#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cstdint>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

namespace pht {
    /**
     * Stores information for use in a tree.
     * 
     * This class stores the information in a tree. It functions as a simple wrapper
     * and is necessary to allow duplicate values in the tree. 
     * 
     * @tparam T The type of data stored. Has to be comparable with < and printable via <<.
     */
    template<class T> class Node {
    public:
        /**
         * Constructs a new node with the given value. 
         * 
         * @param[in] value The value to store in the node. 
         */
        explicit Node(const T value) : value(value) {
        }

        ~Node() = default;

        /**
         * Changes the value to the new value. 
         * 
         * @param[in] value The new value. 
         */
        void setValue(const T value_) {
            this->value = value_;
        }

        /**
         * Returns the stored value. 
         * 
         * @return The value. 
         */
        T getValue() const {
            return value;
        }

        /**
         * Checks if this node is a minidummy.
         * @return true If this node is a minidummy. 
         * @return false If this node is a minidummy. 
         */
        bool isMiniDummy() {
            return miniDummy;
        }

        /**
         * Marks this node as minidummy. 
         * @param val To mark or "un-"mark this node. 
         */
        void setMiniDummy(bool val = true) {
            miniDummy = val;
        }

        /**
         * Sets the minitree-ID of this node. 
         * @param val The new ID. 
         */
        void setMiniTree(uint32_t val = -1) {
            miniTree = val;
        }

        /**
         * Returns the ID of the minitree this node belongs to, or -1. 
         * @return uint32_t The ID of the minitree this node belongs to. 
         */
        uint32_t getMiniTree() {
            return miniTree;
        }

        /**
         * Sets the microtree-ID of this node. 
         * @param val The new ID. 
         */
        void setMicroTree(uint32_t val = -1) {
            microTree = val;
        }

        /**
         * Returns the ID of the microtree this node belongs to, or -1. 
         * @return uint32_t The ID of the microtree this node belongs to. 
         */
        uint32_t getMicroTree() {
            return microTree;
        }

    private:
        T value;
        bool miniDummy = false;
        uint32_t miniTree = -1;
        uint32_t microTree = -1;
    };
}

#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_