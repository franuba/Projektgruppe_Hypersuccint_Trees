#ifndef PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
#define PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H

#include <iostream>
#include <iterator>
#include <set>
#include <vector>

#include "unordered_tree.h"
#include "list_utils.h"
#include "pht_export.h"


namespace pht{
    typedef std::vector<bool> Bitvector;
    /**
     * This class provides functionality for the std::vector<bool> construct
     * which is not implemented or very verbose in the standard library.
     * Functionality concerns specific encodings for the implemented Farzan Munro Algorithm.
     */
    class PHT_EXPORT BitvectorUtils {
        
    private:
        struct PHT_EXPORT HuffmanComparator {
            bool operator()(const Bitvector &a, const Bitvector &b)const;
        };

    public:

        /**
         * This enum defines possible encodings for numbers which BitvectorUtils is able to encode into.
         */
        enum class /*__declspec(dllexport)*/ NumberEncoding {
            BINARY, ///A normal binary encoding
            ELIAS_GAMMA ///Elias-Gamma encoding, consisting of the length of the size of the entry - 1 in unary (as zeros) and the size of the entry in binary + the entry
        };

        /**
         * Encodes the given number with the given encoding. 
         * @param bitvector The bitvector to insert the encoded number into. 
         * @param num The number to encode. 
         * @param encoding The encoding to use. 
         * @return uint32_t The length of the encoded number inserted into the bitvector. 
         */
        static uint32_t encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding);

        /**
         * A version of encodeNumber which encodes the number in binary and returns the result instead of inserting it.
         * @param num The number to encode. 
         * @return uint32_t The resulting binary encoding bitvector. 
         */
        static Bitvector encodeNumberReturn(uint32_t num);

        /**
         * Encodes a number with the given encoding into a bitvector. 
         * @param iterator The starting position in the bitvector. 
         * @param num The number to encode. 
         * @param encoding The encoding to use. 
         * @return uint32_t The length of the encoded number inserted into the bitvector. 
         */
        static uint32_t encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding);

        /**
         * Decodes a number in the give encoding from the COMPLETE given bitvector. 
         * @param bitvector The bitvector from which to decode. 
         * @param encoding The encoding to use. 
         * @return uint32_t The resulting number. 
         */
        static uint32_t decodeNumber(const Bitvector& bitvector, NumberEncoding encoding);

        /**
         * Decodes a number in the give encoding starting from at the given iterator. 
         * @param iterator The starting position in the bitvector. 
         * @param end The ending position in the bitvector. 
         * @param encoding The encoding to use. 
         * @return uint32_t The resulting number. 
         */
        static uint32_t decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding);

        /**
         * Converts a string of type 0100110 into a bitvector. 
         * @param input The (ascii) 0s-and-1s-string to convert. 
         * @return bitvector The binary result. 
         */
        static Bitvector convertToBitvector(const std::string& input);

        /**
         * Counts the occurences of (un-)set bits in the bitvector. 
         * @param iterator The position in the bitvector from which to start counting. 
         * @param end The position in the bitvector where the counting should stop. 
         * @param countZeros Set to true if the unset bits should be counted instead of the set bits. 
         * @return uint32_t The amount of (un-)set bits. 
         */
        static uint32_t countOccurences(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, bool countZeros = false);

    private:
        static uint32_t decodeEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t decodeBinary(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t encodeBinary(std::insert_iterator<Bitvector>& iterator, uint32_t num);

        static uint32_t encodeEliasGamma(std::insert_iterator<Bitvector>& iterator, uint32_t num);
    };
}

#undef DLL_API
#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
