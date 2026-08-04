#ifndef PROJEKTSUCCINCTTREES_HUFFMAN_H
#define PROJEKTSUCCINCTTREES_HUFFMAN_H

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <iostream>

#include "list_utils.h"
#include "unordered_tree.h"

namespace pht {
    /**
     * Overrides the <<-operator to allow printing of std::pairs (of printables types combined with uint32_ts). 
     * @tparam T The printable type in the pair.
     * @param out The stream to print to. 
     * @param entry The entry to print. 
     * @return std::ostream& The stream, for call-chaining. 
     */
    template<class T> std::ostream& operator<<(std::ostream& out, std::pair<T,uint32_t> entry) {
        out << "(" << entry.first << "," << entry.second << ")";
        return out;
    }

    /**
     * Overrides the <<-operator to allow printing of std::vectors (of printables types). 
     * @tparam T The printable type in the vector. 
     * @param out The stream to print to. 
     * @param vec The vector to print. 
     * @return std::ostream& The stream, for call-chaining. 
     */
    template<class T> std::ostream& operator<<(std::ostream& out, std::vector<T> vec) {
        out << "[";
        for(uint32_t i = 0; i < vec.size(); i++) {
            if(i > 0) {
                out << ",";
            }
            out << vec.at(i);
        }
        out << "]";
        return out;
    }

    /**
     * Overrides the <<-operator to allow printing of std::maps (of printables types). 
     * @tparam T The printable key type in the map. 
     * @tparam T The printable value type in the map. 
     * @param out The stream to print to. 
     * @param map The map to print. 
     * @return std::ostream& The stream, for call-chaining. 
     */
    template<class K, class V> std::ostream& operator<<(std::ostream& out, std::map<K,V> map) {
        out << "{\n";
        bool first = true;
        for(std::pair<K,V> entry : map) {
            if(first) {
                first = false;
            } else {
                out << ",\n";
            }
            out << "\t" << entry.first << ": " << entry.second;
        }
        out << "}" << std::endl;
        return out;
    }

    /**
     * This class implements the encoding logic for a Huffman-Encoding. 
     */
    class Huffman {
    public:
        /**
         * Generates a mapping from objects to bitvectors to use in the encoding process. 
         * @tparam T The type of object in the list to encode. 
         * @param occurrences A map from objects to their count of occurrences (e.g. aaabbc -> (a, 3),(b, 2),(c, 1))
         * @return std::map<T,std::vector<bool>> The mapping of objects to a variable length, prefix-free bitvector to use during encoding.
         */
        template<class T> static std::map<T,std::vector<bool>> generateTable(std::map<T,uint32_t>& occurrences) {
            //Fill queue with the occurrences
            auto comparator = [](std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> a, std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> b) { return a->getRoot()->getValue().second > b->getRoot()->getValue().second; };
            std::priority_queue<std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>>, std::vector<std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>>>, decltype(comparator)> queue(comparator);
            for(std::pair<T,uint32_t> entry : occurrences) {
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree = std::make_shared<pht::UnorderedTree<std::pair<T,uint32_t>>>();
                std::shared_ptr<pht::Node<std::pair<T,uint32_t>>> root = std::make_shared<pht::Node<std::pair<T,uint32_t>>>(entry);
                tree->add(root, nullptr);
                queue.push(tree);
            }

            //Build tree by repeatedly combining the two smallest entries
            while(queue.size() > 1) {
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree1 = queue.top();
                queue.pop();
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree2 = queue.top();
                queue.pop();
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> newTree = std::make_shared<pht::UnorderedTree<std::pair<T,uint32_t>>>();
                std::shared_ptr<pht::Node<std::pair<T,uint32_t>>> root = std::make_shared<pht::Node<std::pair<T,uint32_t>>>(std::pair<T,uint32_t>(T(),tree1->getRoot()->getValue().second+tree2->getRoot()->getValue().second));
                newTree->add(root, nullptr);
                newTree->add(tree2, newTree->getRoot());
                newTree->add(tree1, newTree->getRoot());
                queue.push(newTree);
            }

            //PHT_LOGGER_DEBUG("Huffmann") << *queue.top() << std::endl;

            //Fill huffman table
            std::map<T,std::vector<bool>> table;
            fillTable(queue.top(), queue.top()->getRoot(), table, std::vector<bool>());
            //PHT_LOGGER_DEBUG("Huffmann") << table << std::endl;
            return table;
        }

    private:
        template<class T> static void fillTable(std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree, std::shared_ptr<pht::Node<std::pair<T,uint32_t>>> current, std::map<T,std::vector<bool>>& table, std::vector<bool> code) {
            if(tree->isLeaf(current)) {
                table.insert({current->getValue().first, code});
            } else {
                std::vector<std::shared_ptr<pht::Node<std::pair<T,uint32_t>>>> children = tree->getDirectDescendants(current);
                assert(children.size() == 2);
                fillTable(tree, children.at(0), table, ListUtils::combined(code, {false}));
                fillTable(tree, children.at(1), table, ListUtils::combined(code, {true}));
            }
        }
    };
}

#endif //PROJEKTSUCCINCTTREES_HUFFMAN_H
