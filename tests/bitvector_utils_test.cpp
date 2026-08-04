#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"

TEST(BitvectorUtilsTest, stupidTest) {
    std::vector<bool> b = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};
    std::vector<bool> b2 = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1};
    PHT_LOGGER_DEBUG("Test") << b.size() << "\n";
    uint32_t c = pht::BitvectorUtils::decodeNumber(b,pht::BitvectorUtils::NumberEncoding::BINARY);
    uint32_t c2 = pht::BitvectorUtils::decodeNumber(b2,pht::BitvectorUtils::NumberEncoding::BINARY);
    PHT_LOGGER_DEBUG("Test") << c << "\n";
    PHT_LOGGER_DEBUG("Test") << c2 << "\n";
    PHT_LOGGER_DEBUG("Test") << UINT32_MAX << "\n";
}

TEST(BitvectorUtilsTest, encodeBinaryTest) {
    std::vector<bool> b;
    pht::BitvectorUtils::encodeNumber(b, 2, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(1,0));
    b.clear();
    pht::BitvectorUtils::encodeNumber(b, 5, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(1,0,1));
    b.clear();
    pht::BitvectorUtils::encodeNumber(b, 3, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(1,1));
    b.clear();
    pht::BitvectorUtils::encodeNumber(b, 0, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(0));
}

TEST(BitvectorUtilsTest, decodeBinaryTest) {
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0}, pht::BitvectorUtils::NumberEncoding::BINARY), 2);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0,1}, pht::BitvectorUtils::NumberEncoding::BINARY), 5);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,1}, pht::BitvectorUtils::NumberEncoding::BINARY), 3);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1}, pht::BitvectorUtils::NumberEncoding::BINARY), 1);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0,0}, pht::BitvectorUtils::NumberEncoding::BINARY), 4);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,1,0}, pht::BitvectorUtils::NumberEncoding::BINARY), 6);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0,0,1,1,0,1,1,1,1,0,1,1,1}, pht::BitvectorUtils::NumberEncoding::BINARY), 9975);
}

TEST(BitvectorUtilsTest, decodeEliasGammaTest) {
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,0,1,1,0}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 6);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 9975);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 1);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,1,0}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 2);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,0,1,0,1}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 5);
}

TEST(BitvectorUtilsTest, encodeEliasGammaTest) {
    std::vector<bool> bitvector;
    pht::BitvectorUtils::encodeNumber(bitvector, 6, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,1,1,0));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 1, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(1));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 5, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,1,0,1));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 2, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,1,0));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 9975, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1));
}

TEST(BitvectorUtilsTest, convertToBitvectorTest){
    pht::Bitvector bitvector = {0,1,0,1,1,0};
    std::string string = "010110";
    EXPECT_EQ(bitvector, pht::BitvectorUtils::convertToBitvector(string));
}