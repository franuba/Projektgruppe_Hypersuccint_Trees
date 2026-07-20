#ifndef PROJEKTSUCCINCTTREES_HST_OUTPUT_H
#define PROJEKTSUCCINCTTREES_HST_OUTPUT_H

#include "pht/hypersuccinct_tree.h"
#include "pht/pht_export.h"

using namespace std;

extern "C" void PHT_EXPORT hello();

namespace pht {
    /**
     * This class provides various output-methods to debug/check contents of HypersuccinctTrees. 
     */
    class PHT_EXPORT HypersuccinctTreeOutput {

    public:
        /**
         * Prints a given hypersuccinct tree into the console
         * @param tree the hypersuccinct tree to be printed
         */
        static void printTree(HypersuccinctTree &tree);

        /**
         * Prints a given bitvector into the console
         * @param bitvector the bitvector to be printed
         */
        static void printBitvector(const vector<bool>& bitvector);

        /**
         * Prints a given bitvector into the console
         * @param bitvector the bitvector to be printed
         */
        static void printBitvector(const vector<vector<bool>>& bitvector);

        /**
         * Prints a given bitvector into the console
         * @param bitvector the bitvector to be printed
         */
        static void printBitvector(const vector<vector<vector<bool>>> &bitvector);

        /**
         * Writes an entire hypersuccinct tree to a file
         * Add a 0 for all empty Bitvectors
         * @param tree the hypersuccinct tree to be written
         * @param filename the Filename
         */
        static void writeToFile(HypersuccinctTree &tree, const std::string& filename = "tree.txt");

        /**
         * Writes vectors important for the huffman encoding to a file.
         * @param tree the hypersuccinct tree to be written
         * @param filename The filename as String
         * @param allData bool if all relevant data should be written.
         */
        static void writeHuffmanToFile(HypersuccinctTree &tree, const std::string& filename = "tree.txt", bool allData = false);

        /**
         * Reads an entire hypersuccinct tree from a file
         * Has to conform to WriteToFile encoding
         * @param path the path to the file as string
         * @return hypersuccinct tree
         */
        static HypersuccinctTree readFromFile(const string& path);

    private:
        /**
         * Writes a Bitvector to a file in a space efficient way
         *
         * @param file File to write to
         * @param bitvector the bitvector to write
         */
        static void writeBitvectorToFile(std::ofstream &file, Bitvector& bitvector);

        /**
         * Writes Content of a small Bitvector into a larger Bitvector
         * Encodes small Bitvector with Elias Gamma
         * @param bitvector The Bitvector to write
         * @param target The Bitvector to write into
         */
        static void createFileBitvector(const Bitvector& bitvector, Bitvector& target);

        /**
         * Writes Content of a small Bitvector into a larger Bitvector
         * Encodes small Bitvector with Elias Gamma
         * @param bitvector The Bitvector to write
         * @param target The Bitvector to write into
         */
        static void createFileBitvector(const std::vector<Bitvector>& bitvector, Bitvector& target);

        /**
         * Writes Content of a small Bitvector into a larger Bitvector
         * Encodes small Bitvector with Elias Gamma
         * @param bitvector The Bitvector to write
         * @param target The Bitvector to write into
         */
        static void createFileBitvector(const vector<std::vector<Bitvector>> &bitvector, Bitvector &target);

        /**
         * Reads a space efficient Bitvector from a file
         *
         * @param file the file to read from
         * @return the read bitvector
         */
        static Bitvector readBitvectorFromFile(std::ifstream &file);
    };
}

#endif //PROJEKTSUCCINCTTREES_HST_OUTPUT_H
