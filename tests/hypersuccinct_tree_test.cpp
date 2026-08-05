#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST
#ifdef NDEBUG
#define PHT_LOGGER_QUIET
#endif

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"

#define convertToBitvector pht::Bitvector_Utils::convertToBitvector

class HypersuccinctTreeTest : public ::testing::Test {
protected:
    std::shared_ptr<pht::UnorderedTree<std::string>> treeNathX  = pht::XMLReader::readByName("TreeNathX.xml");
    pht::HypersuccinctTree hyperNathX = *pht::HypersuccinctTreeFactory::create(treeNathX, false, 12, 4);
    std::shared_ptr<pht::UnorderedTree<std::string>> treeNath  = pht::XMLReader::readByName("treeNath.xml");
    pht::HypersuccinctTree hyperNath = *pht::HypersuccinctTreeFactory::create(treeNath, false, 12, 4);
    std::shared_ptr<pht::UnorderedTree<std::string>> treeAlex  = pht::XMLReader::readByName("treeAlex.xml");
    pht::HypersuccinctTree hyperAlex = *pht::HypersuccinctTreeFactory::create(treeAlex, false, 12, 4);
    std::shared_ptr<pht::UnorderedTree<char>> example = std::make_shared<pht::UnorderedTree<char>>();
    std::shared_ptr<pht::Node<char>> a = std::make_shared<pht::Node<char>>('a');
    std::shared_ptr<pht::Node<char>> b = std::make_shared<pht::Node<char>>('b');
    std::shared_ptr<pht::Node<char>> c = std::make_shared<pht::Node<char>>('c');
    std::shared_ptr<pht::Node<char>> d = std::make_shared<pht::Node<char>>('d');
    std::shared_ptr<pht::Node<char>> e = std::make_shared<pht::Node<char>>('e');
    std::shared_ptr<pht::Node<char>> f = std::make_shared<pht::Node<char>>('f');
    std::shared_ptr<pht::Node<char>> g = std::make_shared<pht::Node<char>>('g');
    std::shared_ptr<pht::Node<char>> h = std::make_shared<pht::Node<char>>('h');
    std::shared_ptr<pht::Node<char>> i = std::make_shared<pht::Node<char>>('i');
    std::shared_ptr<pht::Node<char>> j = std::make_shared<pht::Node<char>>('j');
    std::shared_ptr<pht::Node<char>> k = std::make_shared<pht::Node<char>>('k');
    std::shared_ptr<pht::Node<char>> l = std::make_shared<pht::Node<char>>('l');
    std::shared_ptr<pht::Node<char>> m = std::make_shared<pht::Node<char>>('m');
    std::shared_ptr<pht::Node<char>> n = std::make_shared<pht::Node<char>>('n');
    std::shared_ptr<pht::Node<char>> o = std::make_shared<pht::Node<char>>('o');
    std::shared_ptr<pht::Node<char>> p = std::make_shared<pht::Node<char>>('p');
    std::shared_ptr<pht::Node<char>> q = std::make_shared<pht::Node<char>>('q');
    std::shared_ptr<pht::Node<char>> r = std::make_shared<pht::Node<char>>('r');
    std::shared_ptr<pht::Node<char>> s = std::make_shared<pht::Node<char>>('s');
    std::shared_ptr<pht::Node<char>> t = std::make_shared<pht::Node<char>>('t');
    std::shared_ptr<pht::Node<char>> u = std::make_shared<pht::Node<char>>('u');
    std::shared_ptr<pht::Node<char>> v = std::make_shared<pht::Node<char>>('v');
    std::shared_ptr<pht::Node<char>> w = std::make_shared<pht::Node<char>>('w');
    std::shared_ptr<pht::Node<char>> x = std::make_shared<pht::Node<char>>('x');
    std::shared_ptr<pht::Node<char>> y = std::make_shared<pht::Node<char>>('y');
    std::shared_ptr<pht::Node<char>> z = std::make_shared<pht::Node<char>>('z');
    std::shared_ptr<pht::Node<char>> A = std::make_shared<pht::Node<char>>('A');
    std::shared_ptr<pht::Node<char>> B = std::make_shared<pht::Node<char>>('B');

    void SetUp() override {
        example->add(a);
        example->add(b, a);
        example->add(c, a);
        example->add(d, a);
        example->add(e, a);
        example->add(f, a);
        example->add(g, b);
        example->add(h, b);
        example->add(i, b);
        example->add(j, b);
        example->add(k, d);
        example->add(l, g);
        example->add(m, i);
        example->add(n, k);
        example->add(o, k);
        example->add(p, k);
        example->add(q, o);
        example->add(r, o);
        example->add(s, q);
        example->add(t, q);
        example->add(u, q);
        example->add(v, q);
        example->add(w, q);
        example->add(x, r);
        example->add(y, r);
        example->add(z, r);
        example->add(A, t);
        example->add(B, x);
    }

};

TEST_F(HypersuccinctTreeTest, MiniTreesTest){
    std::vector<pht::Bitvector> miniFIDs = {{true,true,true},{true,false},{true,true},{true,true},{true},{true,false},{true,false,false,true}};
    std::vector<pht::Bitvector> miniConnectionTypeVectors = {{0,0,1},{1},{0,0},{0,1},{1},{1},{1,1}};
    std::vector<pht::Bitvector> miniDummys = {{0},{0},{0},{0},{1,0,0},{0},{0},{0}};
    EXPECT_EQ(8, hyperNath.getMiniTrees().size());
    EXPECT_EQ(miniFIDs, hyperNath.getMiniFIDs());
    EXPECT_EQ(miniConnectionTypeVectors, hyperNath.getMiniTypeVectors());
    EXPECT_EQ(miniDummys, hyperNath.getMiniDummys());
}

TEST_F(HypersuccinctTreeTest, SupportTest) {
    try {
        pht::MiniTree miniTree = hyperNath.getMiniTree(1);
        miniTree.FIDsSupport.at(0).Rank(0);
        miniTree.FIDsSupport.at(0).Select(0);
    }
    catch (runtime_error) {
        EXPECT_EQ(0,1);
    }
}

TEST_F(HypersuccinctTreeTest, getMicroTreeTest) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::readByName("treeNath.xml");
    pht::HypersuccinctTree hst = *pht::HypersuccinctTreeFactory::create(xmlTree,true, 12, 4);
    pht::MiniTree mini = hst.getMiniTree(4);
    std::vector<bool> res = hst.getMicroTree(mini, 0);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = hst.getMicroTree(mini, 1);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1,0,1,0,0,1,0,0));
    res = hst.getMicroTree(mini, 2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0,1,0,1,0,0));
}

TEST_F(HypersuccinctTreeTest, getFIDforMiniTreeTest) {
    std::vector<bool> res = hyperNath.getMiniTree(0).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1));
    res = hyperNath.getMiniTree(1).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = hyperNath.getMiniTree(2).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
    res = hyperNath.getMiniTree(3).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,0));
    res = hyperNath.getMiniTree(4).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,1));
    res = hyperNath.getMiniTree(5).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0));
    res = hyperNath.getMiniTree(6).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1));
    res = hyperNath.getMiniTree(7).miniTopFIDIndex;
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1));
}

TEST_F(HypersuccinctTreeTest, getTreesForFIDTest) {
    uint32_t res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    uint32_t res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);

    res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, -1);

    res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(2),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(2),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 3);

    res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(3),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(3),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 3);
    EXPECT_EQ(res2, 5);

    res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(4),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(4),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 4);
    EXPECT_EQ(res2, -1);

    res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(5),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(5),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 5);
    EXPECT_EQ(res2, -1);

    res1 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDTopTrees().at(6),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(hyperNath.getFIDLowTrees().at(6),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 6);
    EXPECT_EQ(res2, -1);
}

TEST_F(HypersuccinctTreeTest, getTreesForMicroFIDTest) {
    pht::MiniTree miniTree = hyperNath.getMiniTree(4);
    uint32_t res1 = pht::BitvectorUtils::decodeNumber(miniTree.microFIDTopTrees.at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    uint32_t res2 = pht::BitvectorUtils::decodeNumber(miniTree.microFIDLowTrees.at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);
}

TEST_F(HypersuccinctTreeTest, TreeToFIDIndexConversionTest) {
    pht::MiniTree miniTree = hyperNath.getMiniTree(0);
    uint32_t res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    uint32_t res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, -1);

    miniTree = hyperNath.getMiniTree(1);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);

    miniTree = hyperNath.getMiniTree(2);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 0);

    miniTree = hyperNath.getMiniTree(3);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 3);
    EXPECT_EQ(res2, 2);

    miniTree = hyperNath.getMiniTree(4);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 4);
    EXPECT_EQ(res2, 2);

    miniTree = hyperNath.getMiniTree(5);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 5);
    EXPECT_EQ(res2, 3);

    miniTree = hyperNath.getMiniTree(6);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 6);
    EXPECT_EQ(res2, -1);

    miniTree = hyperNath.getMiniTree(7);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.miniTopFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 6);
    EXPECT_EQ(res2, -1);
}

TEST_F(HypersuccinctTreeTest, MicroTreeToFIDIndexConversionTest) {
    pht::MiniTree miniTree = hyperNath.getMiniTree(1);
    uint32_t res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    uint32_t res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, -1);

    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);

    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(2),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(2),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 0);

    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(3),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(3),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 3);
    EXPECT_EQ(res2, 2);

    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(4),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(4),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 4);
    EXPECT_EQ(res2, -1);

    miniTree = hyperNath.getMiniTree(0);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);

    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(0),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, -1);

    miniTree = hyperNath.getMiniTree(4);
    res1 = pht::BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    res2 = pht::BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(1),pht::BitvectorUtils::NumberEncoding::BINARY) - 1;
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
}

TEST_F(HypersuccinctTreeTest, isDummyAncestorWithinMiniTreeTest) {
    pht::HstNode node = {4,1,1};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,0,0};
    EXPECT_TRUE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,2,0};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,2,2};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,1,2};
    EXPECT_TRUE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,1,4};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::readByName("treeNath.xml");
    pht::HypersuccinctTree hst = *pht::HypersuccinctTreeFactory::create(xmlTree,true, 12, 4);
    node = {4,0,0};
    EXPECT_TRUE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,2,0};
    EXPECT_FALSE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,2,2};
    EXPECT_FALSE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,1,2};
    EXPECT_TRUE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,1,4};
    EXPECT_FALSE(hst.isDummyAncestorWithinMiniTree(node));
}

TEST_F(HypersuccinctTreeTest, treeAlexDoubleDummyTest) {
    pht::HstNode node = {6,0,0};
    uint32_t res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(12, res);

    node = {3,0,4};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(12, res);

    node = {3,0,3};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(6, res);

    node = {3,1,0};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(6, res);

    node = {3,0,2};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(19, res);

    node = {3,0,1};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(1, res);

    node = {3,0,0};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(21, res);

    node = {3,0,2};
    pht::HstNode resN = hyperAlex.child(node,0);
    EXPECT_EQ(pht::HstNode(3,1,0), resN);

    node = {3,0,2};
    resN = hyperAlex.child(node,1);
    EXPECT_EQ(pht::HstNode(6,0,0), resN);

    node = {3,0,3};
    resN = hyperAlex.child(node,0);
    EXPECT_EQ(pht::HstNode(3,1,1), resN);

    node = {3,0,4};
    resN = hyperAlex.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), resN);
}

TEST_F(HypersuccinctTreeTest, treeNathXFIDTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode resN = hyperNathX.child(node,0);
    EXPECT_EQ(pht::HstNode(1,0,0), resN);

    node = {0,0,0};
    resN = hyperNathX.child(node,1);
    EXPECT_EQ(pht::HstNode(3,0,0), resN);

    node = {0,0,0};
    resN = hyperNathX.child(node,2);
    EXPECT_EQ(pht::HstNode(0,1,0), resN);

    node = {1,0,0};
    resN = hyperNathX.child(node,0);
    EXPECT_EQ(pht::HstNode(1,1,0), resN);

    node = {1,0,0};
    resN = hyperNathX.child(node,1);
    EXPECT_EQ(pht::HstNode(1,0,1), resN);

    node = {1,0,0};
    resN = hyperNathX.child(node,2);
    EXPECT_EQ(pht::HstNode(1,2,0), resN);

    node = {1,0,0};
    resN = hyperNathX.child(node,3);
    EXPECT_EQ(pht::HstNode(2,0,1), resN);

    node = {3,0,0};
    resN = hyperNathX.child(node,0);
    EXPECT_EQ(pht::HstNode(4,0,0), resN);
}

TEST_F(HypersuccinctTreeTest, childTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,0,0), res);

    node = {1,1,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,1,1), res);

    node = {0,1,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(0,1,1), res);

    node = {4,1,2};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), res);

    node = {4,2,3};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(), res);

    node = {4,1,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(4,2,0), res);

    node = {4,1,0};
    res = hyperNath.child(node,1);
    EXPECT_EQ(pht::HstNode(4,1,1), res);

    node = {4,1,0};
    res = hyperNath.child(node,2);
    EXPECT_EQ(pht::HstNode(7,0,0), res);

    node = {1,1,2};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,4,1), res);

    node = {1,1,1};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,4,0), res);

    node = {1,2,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,3,0), res);

    node = {1,2,0};
    res = hyperNath.child(node,1);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {1,2,1};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(), res);

    node = {1,4,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,4,1), res);

    node = {3,0,1};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(3,0,2), res);

    node = {6,0,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), res);

    node = {7,0,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), res);

    node = {6,0,0};
    res = hyperNath.child(node,1);
    EXPECT_EQ(pht::HstNode(6,0,1), res);

    node = {6,0,0};
    res = hyperNath.child(node,2);
    EXPECT_EQ(pht::HstNode(6,2,0), res);

    node = {6,0,0};
    res = hyperNath.child(node,3);
    EXPECT_EQ(pht::HstNode(7,0,1), res);

    node = {6,0,0};
    res = hyperNath.child(node,4);
    EXPECT_EQ(pht::HstNode(), res);

    node = {6,2,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,2,1), res);

    node = {6,2,0};
    res = hyperNath.child(node,1);
    EXPECT_EQ(pht::HstNode(6,4,0), res);

    node = {6,2,0};
    res = hyperNath.child(node,2);
    EXPECT_EQ(pht::HstNode(6,2,2), res);

    node = {6,2,0};
    res = hyperNath.child(node,3);
    EXPECT_EQ(pht::HstNode(), res);

}

TEST_F(HypersuccinctTreeTest, child_rankTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {0,1,6};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {0,1,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {0,1,5};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {4,1,2};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {4,1,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {4,2,3};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {1,1,2};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {1,2,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {1,1,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {1,2,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {1,3,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {3,0,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {1,4,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {6,0,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {6,0,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {7,0,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {6,1,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {6,2,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {7,0,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(3, res);
}

TEST_F(HypersuccinctTreeTest, getParentTest) {
    pht::HstNode node = {6,1,0};
    pht::HstNode resNode = hyperNath.getParent(node);
    EXPECT_EQ(pht::HstNode(6,0,0),resNode);

    node = {6,0,0};
    resNode = hyperNath.getParent(node);
    EXPECT_EQ(pht::HstNode(4,1,0),resNode);

    node = {1,3,3};
    resNode = hyperNath.getParent(node);
    EXPECT_EQ(pht::HstNode(1,3,1),resNode);

    node = {1,3,0};
    resNode = hyperNath.getParent(node);
    EXPECT_EQ(pht::HstNode(1,2,0),resNode);

    node = {1,4,0};
    resNode = hyperNath.getParent(node);
    EXPECT_EQ(pht::HstNode(1,1,1),resNode);
}

TEST_F(HypersuccinctTreeTest, degreeTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.degree(node);
    EXPECT_EQ(3, res);

    node = {0,1,6};
    res = hyperNath.degree(node);
    EXPECT_EQ(0, res);

    node = {2,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {7,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {4,1,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(3, res);

    node = {4,1,1};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {4,1,2};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {1,1,2};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {6,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {2,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {3,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {1,1,1};
    res = hyperNath.degree(node);
    EXPECT_EQ(1, res);

    node = {7,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {0,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(3, res);

    node = {0,1,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {0,1,1};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

}

TEST_F(HypersuccinctTreeTest, subtree_sizeTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.subtreeSize(node);
    EXPECT_EQ(77, res);

    node = {1,1,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(8, res);

    node = {0,1,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(7, res);

    node = {4,1,2};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(23, res);

    node = {4,1,1};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(3, res);

    node = {4,2,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(4, res);

    node = {4,1,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(31, res);

    node = {6,0,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(23, res);

    node = {1,1,2};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(6, res);

    node = {1,4,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(6, res);

    node = {1,1,1};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(7, res);

    node = {6,0,0};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(12, res);

    node = {3,0,4};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(12, res);

    node = {3,0,2};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(19, res);

    node = {3,0,1};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(1, res);

    node = {3,0,0};
    res = hyperAlex.subtreeSize(node);
    EXPECT_EQ(21, res);

}

TEST_F(HypersuccinctTreeTest, depthTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.depth(node);
    EXPECT_EQ(0, res);

    node = {1,1,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(2, res);

    node = {0,1,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(1, res);

    node = {4,1,2};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

    node = {4,1,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(3, res);

    node = {6,0,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

    node = {1,1,2};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

    node = {1,1,1};
    res = hyperNath.depth(node);
    EXPECT_EQ(3, res);

    node = {1,4,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

}

TEST_F(HypersuccinctTreeTest, heightTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.height(node);
    EXPECT_EQ(8, res);

    node = {1,1,0};
    res = hyperNath.height(node);
    EXPECT_EQ(4, res);

    node = {0,1,0};
    res = hyperNath.height(node);
    EXPECT_EQ(2, res);

    node = {4,1,2};
    res = hyperNath.height(node);
    EXPECT_EQ(3, res);

    node = {4,1,0};
    res = hyperNath.height(node);
    EXPECT_EQ(4, res);

    node = {6,0,0};
    res = hyperNath.height(node);
    EXPECT_EQ(3, res);

    node = {1,1,2};
    res = hyperNath.height(node);
    EXPECT_EQ(2, res);

    node = {1,4,0};
    res = hyperNath.height(node);
    EXPECT_EQ(2, res);

    node = {1,1,1};
    res = hyperNath.height(node);
    EXPECT_EQ(3, res);

}

TEST_F(HypersuccinctTreeTest, leaf_sizeTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.leafSize(node);
    EXPECT_EQ(39, res);

    node = {1,1,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {0,1,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(4, res);

    node = {4,1,2};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(14, res);

    node = {4,1,1};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(2, res);

    node = {4,2,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {4,1,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(19, res);

    node = {6,0,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(14, res);

    node = {4,2,3};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(1, res);

    node = {1,1,2};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {1,4,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {1,1,1};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

}

TEST_F(HypersuccinctTreeTest, rightmost_leafTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(0,1,6), res);
    //EXPECT_THAT(res, ::testing::ElementsAre(1,0,0));

    node = {1,1,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,5), res);

    node = {0,1,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(0,1,6), res);

    node = {4,1,2};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(7,0,2), res);

    node = {6,0,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(7,0,2), res);

    node = {4,2,3};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(4,2,3), res);

    node = {4,1,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(7,0,2), res);

    node = {1,1,2};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,5), res);

    node = {1,1,1};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,5), res);

    node = {1,2,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {1,2,1};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {3,0,1};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(3,0,3), res);

}

TEST_F(HypersuccinctTreeTest,leftmost_leafTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {1,1,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {0,1,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(0,1,2), res);

    node = {4,1,2};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(6,3,1), res);

    node = {6,0,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(6,3,1), res);

    node = {4,2,3};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(4,2,3), res);

    node = {4,1,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(4,2,1), res);

    node = {1,1,2};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {1,1,1};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {1,2,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,3,2), res);

    node = {1,2,1};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {3,0,1};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(3,0,2), res);

}

TEST_F(HypersuccinctTreeTest,leaf_rankTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {1,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {2,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {3,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {4,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(16, res);

    node = {5,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {6,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

    node = {7,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

    node = {1,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {0,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(35, res);

    node = {0,1,6};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(39, res);

    node = {4,2,3};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(19, res);

    node = {4,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(16, res);

    node = {1,4,5};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(3, res);

    node = {1,1,2};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {1,1,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {1,2,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(3, res);

    node = {1,2,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {1,3,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(3, res);

    node = {3,0,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(14, res);

    node = {4,1,2};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

    node = {4,1,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(19, res);

    node = {6,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

    node = {6,0,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(26, res);

    node = {6,2,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(28, res);

    node = {7,0,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(34, res);

}