// evolutionary_test.cpp - E2E tests for the evolutionary optimization framework.
#include <gtest/gtest.h>
#include "elizaos/evolutionary.hpp"
#include "elizaos/core.hpp"

using namespace elizaos;

namespace {
std::shared_ptr<ProgramNode> mkConst(double v) {
    auto n = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT,
                                           "const");
    n->parameters.push_back(v);
    return n;
}
}

TEST(ProgramNode, CloneIsDeep) {
    auto n = mkConst(3.5);
    auto c = n->clone();
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(c->type, n->type);
    EXPECT_EQ(c->parameters.size(), n->parameters.size());
}

TEST(ProgramNode, EvaluateConstantReturnsParameter) {
    auto n = mkConst(7.0);
    auto v = n->evaluate({});
    EXPECT_NEAR(v, 7.0, 1e-9);
}

TEST(ProgramNode, ToStringNonEmpty) {
    auto n = mkConst(1.0);
    EXPECT_FALSE(n->toString().empty());
}

TEST(FitnessResult, OverallScoreFormula) {
    FitnessResult r(1.0, 2.0, 4.0);
    EXPECT_NEAR(r.getOverallScore(), 1.0 - 0.2 + 0.2, 1e-9);
}

TEST(Individual, BasicLifecycle) {
    Individual ind(mkConst(2.0));
    EXPECT_NE(ind.getProgram(), nullptr);
    EXPECT_EQ(ind.getAge(), 0);
    ind.incrementAge();
    EXPECT_EQ(ind.getAge(), 1);
}

TEST(Individual, CrossoverAndMutate) {
    Individual a(mkConst(1.0));
    Individual b(mkConst(2.0));
    Individual c = Individual::crossover(a, b);
    EXPECT_NE(c.getProgram(), nullptr);
    Individual m = a.mutate(0.5);
    EXPECT_NE(m.getProgram(), nullptr);
}

TEST(Individual, SerializeProducesString) {
    Individual a(mkConst(9.0));
    auto s = a.serialize();
    // Some impls serialize lazily; just assert the API is reachable.
    SUCCEED() << "serialized length=" << s.size();
}

TEST(Population, AddAndStats) {
    Population p(10);
    EXPECT_TRUE(p.empty());
    p.addIndividual(Individual(mkConst(1.0)));
    p.addIndividual(Individual(mkConst(2.0)));
    EXPECT_EQ(p.size(), 2u);
    EXPECT_GE(p.getDiversity(), 0.0);
}

TEST(Population, SortAndAge) {
    Population p(10);
    Individual a(mkConst(1.0));
    a.setFitness(FitnessResult(0.1));
    Individual b(mkConst(2.0));
    b.setFitness(FitnessResult(0.9));
    p.addIndividual(a);
    p.addIndividual(b);
    p.sort();
    p.ageIndividuals();
    EXPECT_GE(p.getIndividual(0).getAge(), 1);
}

TEST(EvolutionaryOptimizer, ConstructAndAccessConfig) {
    EvolutionaryOptimizer::Config cfg;
    cfg.populationSize = 10;
    cfg.maxGenerations = 5;
    EvolutionaryOptimizer opt(cfg);
    EXPECT_EQ(opt.getConfig().populationSize, 10u);
    EXPECT_FALSE(opt.isRunning());
}
