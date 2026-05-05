// evolutionary_test.cpp
// End-to-end tests for elizaos::Population, Individual, ProgramNode, and
// FitnessResult.

#include "elizaos/evolutionary.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace elizaos;

TEST(FitnessResult, OverallScoreCombinesComponents) {
    FitnessResult r(0.8, 0.2, 0.1);
    EXPECT_GT(r.getOverallScore(), 0.0);
}

TEST(ProgramNode, BasicConstruction) {
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k0");
    EXPECT_EQ(n->name, "k0");
}

TEST(ProgramNode, CloneIndependent) {
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k1");
    auto c = n->clone();
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(c->name, "k1");
    EXPECT_NE(c.get(), n.get());
}

TEST(Individual, ConstructionAndAge) {
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k");
    Individual ind(n);
    EXPECT_EQ(ind.getAge(), 0);
    ind.incrementAge();
    EXPECT_EQ(ind.getAge(), 1);
}

TEST(Individual, FitnessRoundtrip) {
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k");
    Individual ind(n);
    ind.setFitness(FitnessResult(0.9, 0.1, 0.05));
    EXPECT_NEAR(ind.getFitness().fitness, 0.9, 1e-9);
}

TEST(Population, AddAndRemove) {
    Population p(10);
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k");
    p.addIndividual(Individual(n));
    p.addIndividual(Individual(n));
    EXPECT_FALSE(p.empty());
    p.removeIndividual(0);
    EXPECT_FALSE(p.empty());
}

TEST(Population, ClearEmpties) {
    Population p(10);
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k");
    p.addIndividual(Individual(n));
    p.clear();
    EXPECT_TRUE(p.empty());
}

TEST(Population, AgeIndividuals) {
    Population p(10);
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "k");
    p.addIndividual(Individual(n));
    p.addIndividual(Individual(n));
    EXPECT_NO_THROW(p.ageIndividuals());
}
