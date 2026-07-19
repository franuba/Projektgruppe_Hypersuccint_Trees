//
// Created by User on 29.04.2021.
//

#include <pht/bitvector_utils.h>
#include <cmath>

using namespace pht;

bool BitvectorUtils::HuffmanComparator::operator()(const Bitvector &a, const Bitvector &b) const {
    return ((a.size() != b.size()) ? a.size() < b.size() : a < b);
}

uint32_t BitvectorUtils::encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding) {
    return encodeNumber(std::inserter(bitvector, bitvector.end()), num, encoding);
}

uint32_t BitvectorUtils::encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding) {
    switch (encoding) {
        case NumberEncoding::BINARY:
            return encodeBinary(iterator, num);
        case NumberEncoding::ELIAS_GAMMA:
            return encodeEliasGamma(iterator, num);
        default:
            assert(false);
            return 0;
    }
}

Bitvector BitvectorUtils::encodeNumberReturn(uint32_t num) {
    Bitvector res;
    uint32_t i = 0;
    if (num == 0) {
        res.push_back(false);
        return res;
    }
    uint32_t size = static_cast<uint32_t>(floor(std::log2(num))) + 1;
    for(; i < size; i++) {
        res.push_back((num>>(size-1-i))&1);
    }
    return res;
}

uint32_t BitvectorUtils::decodeNumber(const Bitvector& bitvector, NumberEncoding encoding) {
    auto iter = bitvector.cbegin();
    return decodeNumber(iter, bitvector.cend(), encoding);
}

uint32_t BitvectorUtils::decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding) {
    assert(iterator <= end);
    switch (encoding) {
        case NumberEncoding::BINARY:
            return decodeBinary(iterator, end);
        case NumberEncoding::ELIAS_GAMMA:
            return decodeEliasGamma(iterator, end);
        default:
            assert(false);
            return 0;
    }
}

Bitvector BitvectorUtils::convertToBitvector(const std::string& input) {
    Bitvector result;
    for(char i : input){
        result.push_back(i=='1');
    }
    return result;
}

uint32_t BitvectorUtils::decodeEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
    if(iterator == end) {
        return 0;
    } else if(*iterator) {
        iterator++;
        return 1;
    }

    uint32_t size = 0;
    for(; !*iterator && iterator <= end; iterator++, size++);
    if(iterator == end) {
        throw std::runtime_error("Invalid Elias Gamma Code");
    }
    size++;

    uint32_t res = 0;
    for(uint32_t i = 0; i < size; i++, iterator++) {
        res <<= 1;
        res = res|(*iterator?1:0);
        if(iterator == end) {
            if(i < size-1) {
                throw std::runtime_error("Invalid Elias Gamma Code");
            }
            break;
        }
    }
    return res;
}

uint32_t BitvectorUtils::decodeBinary(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
    uint32_t res = 0;
    auto start = iterator;
    for(; iterator != end && (iterator-start < 32) ; iterator++) {
        res <<= 1;
        res = res|(*iterator?1:0);
    }
    return res;
}

uint32_t BitvectorUtils::encodeBinary(std::insert_iterator<Bitvector>& iterator, uint32_t num) {
    uint32_t i = 0;
    if (num == 0) {
        iterator = false;
        return 1;
    }
    uint32_t size = static_cast<uint32_t>(floor(std::log2(num))) + 1;
    for(; i < size; i++) {
        iterator = ((num>>(size-1-i))&1);
    }
    return i;
}

uint32_t BitvectorUtils::encodeEliasGamma(std::insert_iterator<Bitvector>& iterator, uint32_t num) {
    uint32_t count = 0;
    auto logSize = static_cast<uint32_t>(floor((std::log2(num))));
    for(uint32_t i = 0; i < logSize; i++, count++) {
        iterator = false;
    }
    for(uint32_t i = 0; i < logSize+1; i++, count++) {
        iterator = (num>>(logSize-i))%2==1;
    }
    return count;
}

uint32_t BitvectorUtils::countOccurences(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, bool countZeros) {
    int occurences = 0;
    for(Bitvector::const_iterator indexIterator = iterator; indexIterator < end; indexIterator++) {
        if(*indexIterator != countZeros) {
            occurences++;
        }
    }
    return occurences;
}