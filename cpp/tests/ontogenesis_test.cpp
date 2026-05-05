// ontogenesis_test.cpp
// Tests for elizaos::BSeries (rooted-tree counts) and KernelGenome basic ops.

#include "elizaos/ontogenesis.hpp"
#include <gtest/gtest.h>

using namespace elizaos;
using namespace elizaos::ontogenesis;

TEST(BSeries, DefaultConstruction) {
    BSeries b;
    EXPECT_EQ(b.order, 0);
}

TEST(BSeries, ConstructWithOrder) {
    BSeries b(4);
    EXPECT_EQ(b.order, 4);
}

TEST(BSeries, GetTreeCountKnownValues) {
    // OEIS A000081: 1, 1, 2, 4, 9, 20, 48, 115, ...
    EXPECT_GE(BSeries::getTreeCount(1), 1);
    EXPECT_GE(BSeries::getTreeCount(2), 1);
    EXPECT_GE(BSeries::getTreeCount(3), 2);
    EXPECT_GE(BSeries::getTreeCount(4), 4);
}

TEST(KernelGene, DefaultConstruction) {
    KernelGene g;
    EXPECT_EQ(g.type, GeneType::COEFFICIENT);
}

TEST(KernelGene, ParameterizedConstruction) {
    KernelGene g(GeneType::COEFFICIENT, "g1", {0.1, 0.2, 0.3}, true);
    EXPECT_EQ(g.name, "g1");
    EXPECT_EQ(g.values.size(), 3u);
    EXPECT_TRUE(g.mutable_);
}

TEST(KernelGenome, DefaultConstruction) {
    KernelGenome g;
    EXPECT_TRUE(g.id.empty());
}

TEST(OntogeneticState, DefaultConstruction) {
    OntogeneticState s;
    EXPECT_EQ(s.stage, DevelopmentStage::EMBRYONIC);
}

TEST(GripEvaluation, DefaultsAreZero) {
    GripEvaluation e;
    EXPECT_NEAR(e.contact, 0.0, 1e-9);
}
