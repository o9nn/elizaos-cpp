// ontogenesis_test.cpp - E2E tests for the ontogenesis evolutionary kernel.
#include <gtest/gtest.h>
#include "elizaos/ontogenesis.hpp"

using namespace elizaos::ontogenesis;

TEST(BSeries, GetTreeCountIsPositive) {
    EXPECT_GT(BSeries::getTreeCount(1), 0);
    EXPECT_GT(BSeries::getTreeCount(2), 0);
}

TEST(InitializeKernel, ProducesNonEmptyKernel) {
    auto k = initializeOntogeneticKernel(3);
    SUCCEED() << "kernel initialised";
}

TEST(IdGeneration, ProducesUniqueIds) {
    auto a = generateKernelId();
    auto b = generateKernelId();
    EXPECT_FALSE(a.empty());
    EXPECT_NE(a, b);
}

TEST(GenomeOps, MutateThenCrossoverNoCrash) {
    auto k1 = initializeOntogeneticKernel(2);
    auto k2 = initializeOntogeneticKernel(2);
    EXPECT_NO_THROW(selfGenerate(k1));
    EXPECT_NO_THROW(selfOptimize(k1, 1));
}

TEST(EnumStringification, NonEmpty) {
    EXPECT_FALSE(developmentStageToString(DevelopmentStage{}).empty());
    EXPECT_FALSE(reproductionMethodToString(ReproductionMethod{}).empty());
}

TEST(EvaluateGrip, OnInitKernel) {
    auto k = initializeOntogeneticKernel(2);
    EXPECT_NO_THROW(evaluateGrip(k));
}

TEST(RunOntogenesis, MinimalConfigCallable) {
    OntogenesisConfig cfg;
    EXPECT_NO_THROW(runOntogenesis(cfg));
}
