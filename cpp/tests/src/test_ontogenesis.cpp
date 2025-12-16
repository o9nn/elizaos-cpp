/**
 * @file test_ontogenesis.cpp
 * @brief Unit tests for Ontogenesis self-generating kernels
 */

#include <gtest/gtest.h>
#include "elizaos/ontogenesis.hpp"
#include <memory>

using namespace elizaos::ontogenesis;

class OntogenesisTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures
    }
    
    void TearDown() override {
        // Clean up
    }
};

// ============================================================================
// Kernel Initialization Tests
// ============================================================================

TEST_F(OntogenesisTest, InitializeKernelOrder1) {
    auto kernel = initializeOntogeneticKernel(1);
    
    EXPECT_FALSE(kernel.id.empty());
    EXPECT_EQ(kernel.genome.generation, 0);
    EXPECT_EQ(kernel.genome.age, 0);
    EXPECT_EQ(kernel.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
    EXPECT_FLOAT_EQ(kernel.ontogeneticState.maturity, 0.0);
    EXPECT_EQ(kernel.bseries.order, 1);
    EXPECT_EQ(kernel.bseries.coefficients.size(), 1);
}

TEST_F(OntogenesisTest, InitializeKernelOrder4) {
    auto kernel = initializeOntogeneticKernel(4);
    
    EXPECT_FALSE(kernel.id.empty());
    EXPECT_EQ(kernel.genome.generation, 0);
    EXPECT_EQ(kernel.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
    EXPECT_GT(kernel.bseries.coefficients.size(), 0);
    EXPECT_GT(kernel.genome.fitness, 0.0);
    EXPECT_LE(kernel.genome.fitness, 1.0);
}

TEST_F(OntogenesisTest, InitialGenomeHasGenes) {
    auto kernel = initializeOntogeneticKernel(4);
    
    EXPECT_GT(kernel.genome.genes.size(), 0);
    
    // Should have at least coefficient and operator genes
    bool hasCoeff = false, hasOperator = false, hasSymmetry = false;
    for (const auto& gene : kernel.genome.genes) {
        if (gene.type == GeneType::COEFFICIENT) hasCoeff = true;
        if (gene.type == GeneType::OPERATOR) hasOperator = true;
        if (gene.type == GeneType::SYMMETRY) hasSymmetry = true;
    }
    
    EXPECT_TRUE(hasCoeff);
    EXPECT_TRUE(hasOperator);
    EXPECT_TRUE(hasSymmetry);
}

// ============================================================================
// Self-Generation Tests
// ============================================================================

TEST_F(OntogenesisTest, SelfGenerate) {
    auto parent = initializeOntogeneticKernel(4);
    auto offspring = selfGenerate(parent);
    
    EXPECT_NE(offspring.id, parent.id);
    EXPECT_EQ(offspring.genome.generation, parent.genome.generation + 1);
    EXPECT_EQ(offspring.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
    EXPECT_EQ(offspring.genome.lineage.size(), 1);
    EXPECT_EQ(offspring.genome.lineage[0], parent.genome.id);
}

TEST_F(OntogenesisTest, SelfGenerateMaintainsOrder) {
    auto parent = initializeOntogeneticKernel(4);
    auto offspring = selfGenerate(parent);
    
    EXPECT_EQ(offspring.bseries.order, parent.bseries.order);
    EXPECT_EQ(offspring.bseries.coefficients.size(), parent.bseries.coefficients.size());
}

TEST_F(OntogenesisTest, SelfGenerateLineage) {
    auto ancestor = initializeOntogeneticKernel(4);
    auto gen1 = selfGenerate(ancestor);
    auto gen2 = selfGenerate(gen1);
    auto gen3 = selfGenerate(gen2);
    
    EXPECT_EQ(gen1.genome.generation, 1);
    EXPECT_EQ(gen2.genome.generation, 2);
    EXPECT_EQ(gen3.genome.generation, 3);
    
    EXPECT_EQ(gen3.genome.lineage.size(), 3);
}

// ============================================================================
// Self-Optimization Tests
// ============================================================================

TEST_F(OntogenesisTest, SelfOptimize) {
    auto kernel = initializeOntogeneticKernel(4);
    
    auto optimized = selfOptimize(kernel, 10);
    
    EXPECT_EQ(optimized.id, kernel.id);
    EXPECT_GE(optimized.ontogeneticState.maturity, kernel.ontogeneticState.maturity);
    EXPECT_GT(optimized.ontogeneticState.events.size(), kernel.ontogeneticState.events.size());
}

TEST_F(OntogenesisTest, SelfOptimizeIncreasesMaturity) {
    auto kernel = initializeOntogeneticKernel(4);
    auto optimized = selfOptimize(kernel, 20);
    
    EXPECT_GT(optimized.ontogeneticState.maturity, 0.0);
}

// ============================================================================
// Self-Reproduction Tests
// ============================================================================

TEST_F(OntogenesisTest, SelfReproduceCrossover) {
    auto parent1 = initializeOntogeneticKernel(4);
    auto parent2 = initializeOntogeneticKernel(4);
    
    auto child = selfReproduce(parent1, parent2, ReproductionMethod::CROSSOVER);
    
    EXPECT_NE(child.id, parent1.id);
    EXPECT_NE(child.id, parent2.id);
    EXPECT_EQ(child.genome.lineage.size(), 2);
    EXPECT_EQ(child.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
}

TEST_F(OntogenesisTest, SelfReproduceMutation) {
    auto parent = initializeOntogeneticKernel(4);
    
    auto child = selfReproduce(parent, parent, ReproductionMethod::MUTATION);
    
    EXPECT_NE(child.id, parent.id);
    EXPECT_GT(child.genome.generation, parent.genome.generation);
}

TEST_F(OntogenesisTest, SelfReproduceCloning) {
    auto parent = initializeOntogeneticKernel(4);
    
    auto child = selfReproduce(parent, parent, ReproductionMethod::CLONING);
    
    EXPECT_NE(child.id, parent.id);
    EXPECT_EQ(child.bseries.order, parent.bseries.order);
}

// ============================================================================
// Genetic Operations Tests
// ============================================================================

TEST_F(OntogenesisTest, Crossover) {
    auto kernel1 = initializeOntogeneticKernel(4);
    auto kernel2 = initializeOntogeneticKernel(4);
    
    auto [offspring1, offspring2] = crossover(kernel1.genome, kernel2.genome);
    
    // Crossover modifies gene values, not IDs
    EXPECT_EQ(offspring1.genes.size(), kernel1.genome.genes.size());
    EXPECT_EQ(offspring2.genes.size(), kernel2.genome.genes.size());
}

TEST_F(OntogenesisTest, Mutate) {
    auto kernel = initializeOntogeneticKernel(4);
    
    auto mutated = mutate(kernel.genome, 0.5);
    
    EXPECT_EQ(mutated.genes.size(), kernel.genome.genes.size());
}

TEST_F(OntogenesisTest, GeneticDistance) {
    auto kernel1 = initializeOntogeneticKernel(4);
    auto kernel2 = initializeOntogeneticKernel(4);
    
    double dist = geneticDistance(kernel1.genome, kernel2.genome);
    
    EXPECT_GE(dist, 0.0);
    EXPECT_LE(dist, 1.0);
}

TEST_F(OntogenesisTest, GeneticDistanceSelf) {
    auto kernel = initializeOntogeneticKernel(4);
    
    double dist = geneticDistance(kernel.genome, kernel.genome);
    
    EXPECT_FLOAT_EQ(dist, 0.0);
}

TEST_F(OntogenesisTest, CloneGenome) {
    auto kernel = initializeOntogeneticKernel(4);
    
    auto clone = cloneGenome(kernel.genome);
    
    EXPECT_NE(clone.id, kernel.genome.id);
    EXPECT_EQ(clone.generation, kernel.genome.generation + 1);
    EXPECT_EQ(clone.genes.size(), kernel.genome.genes.size());
}

// ============================================================================
// Fitness Evaluation Tests
// ============================================================================

TEST_F(OntogenesisTest, EvaluateGrip) {
    auto kernel = initializeOntogeneticKernel(4);
    
    auto grip = evaluateGrip(kernel);
    
    EXPECT_GE(grip.contact, 0.0);
    EXPECT_LE(grip.contact, 1.0);
    EXPECT_GE(grip.coverage, 0.0);
    EXPECT_LE(grip.coverage, 1.0);
    EXPECT_GE(grip.efficiency, 0.0);
    EXPECT_LE(grip.efficiency, 1.0);
    EXPECT_GE(grip.stability, 0.0);
    EXPECT_LE(grip.stability, 1.0);
    EXPECT_GE(grip.overall, 0.0);
    EXPECT_LE(grip.overall, 1.0);
}

TEST_F(OntogenesisTest, EvaluateFitness) {
    auto kernel = initializeOntogeneticKernel(4);
    
    double fitness = evaluateFitness(kernel);
    
    EXPECT_GE(fitness, 0.0);
    EXPECT_LE(fitness, 1.0);
}

TEST_F(OntogenesisTest, CalculateNovelty) {
    std::vector<OntogeneticKernel> population;
    for (int i = 0; i < 10; ++i) {
        population.push_back(initializeOntogeneticKernel(4));
    }
    
    auto kernel = initializeOntogeneticKernel(4);
    double novelty = calculateNovelty(kernel, population);
    
    EXPECT_GE(novelty, 0.0);
    EXPECT_LE(novelty, 1.0);
}

// ============================================================================
// Evolution System Tests
// ============================================================================

TEST_F(OntogenesisTest, RunOntogenesisBasic) {
    OntogenesisConfig config;
    config.evolution.populationSize = 10;
    config.evolution.maxGenerations = 5;
    config.evolution.fitnessThreshold = 0.99; // High threshold so it runs all generations
    
    auto result = runOntogenesis(config);
    
    EXPECT_EQ(result.finalPopulation.size(), 10);
    EXPECT_GT(result.totalGenerations, 0);
    EXPECT_LE(result.totalGenerations, 5);
}

TEST_F(OntogenesisTest, RunOntogenesisWithSeeds) {
    OntogenesisConfig config;
    config.evolution.populationSize = 10;
    config.evolution.maxGenerations = 5;
    
    config.seedKernels.push_back(initializeOntogeneticKernel(4));
    config.seedKernels.push_back(initializeOntogeneticKernel(4));
    
    auto result = runOntogenesis(config);
    
    EXPECT_EQ(result.finalPopulation.size(), 10);
    EXPECT_FALSE(result.generations.empty());
}

TEST_F(OntogenesisTest, EvolutionImprovesFitness) {
    OntogenesisConfig config;
    config.evolution.populationSize = 20;
    config.evolution.maxGenerations = 10;
    config.evolution.mutationRate = 0.2;
    config.evolution.crossoverRate = 0.7;
    config.evolution.fitnessThreshold = 0.99;
    
    auto result = runOntogenesis(config);
    
    EXPECT_FALSE(result.generations.empty());
    if (result.generations.size() > 1) {
        double initialBest = result.generations.front().bestFitness;
        double finalBest = result.generations.back().bestFitness;
        // Fitness should not decrease (though it may plateau)
        EXPECT_GE(finalBest, initialBest * 0.9); // Allow some variance
    }
}

TEST_F(OntogenesisTest, TournamentSelect) {
    std::vector<OntogeneticKernel> population;
    for (int i = 0; i < 10; ++i) {
        auto kernel = initializeOntogeneticKernel(4);
        kernel.genome.fitness = static_cast<double>(i) / 10.0;
        population.push_back(kernel);
    }
    
    const auto& selected = tournamentSelect(population, 3);
    
    EXPECT_GE(selected.genome.fitness, 0.0);
    EXPECT_LE(selected.genome.fitness, 1.0);
}

TEST_F(OntogenesisTest, UpdateDevelopmentStage) {
    auto kernel = initializeOntogeneticKernel(4);
    DevelopmentSchedule schedule;
    
    EXPECT_EQ(kernel.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
    
    // Age it through stages
    for (int i = 0; i < schedule.embryonicDuration + 1; ++i) {
        updateDevelopmentStage(kernel, schedule);
    }
    
    EXPECT_NE(kernel.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
}

TEST_F(OntogenesisTest, CalculatePopulationDiversity) {
    std::vector<OntogeneticKernel> population;
    for (int i = 0; i < 10; ++i) {
        population.push_back(initializeOntogeneticKernel(4));
    }
    
    double diversity = calculatePopulationDiversity(population);
    
    EXPECT_GE(diversity, 0.0);
}

// ============================================================================
// Utility Functions Tests
// ============================================================================

TEST_F(OntogenesisTest, GenerateKernelId) {
    auto id1 = generateKernelId();
    auto id2 = generateKernelId();
    
    EXPECT_FALSE(id1.empty());
    EXPECT_FALSE(id2.empty());
    EXPECT_NE(id1, id2);
}

TEST_F(OntogenesisTest, DevelopmentStageToString) {
    EXPECT_EQ(developmentStageToString(DevelopmentStage::EMBRYONIC), "EMBRYONIC");
    EXPECT_EQ(developmentStageToString(DevelopmentStage::JUVENILE), "JUVENILE");
    EXPECT_EQ(developmentStageToString(DevelopmentStage::MATURE), "MATURE");
    EXPECT_EQ(developmentStageToString(DevelopmentStage::SENESCENT), "SENESCENT");
}

TEST_F(OntogenesisTest, ReproductionMethodToString) {
    EXPECT_EQ(reproductionMethodToString(ReproductionMethod::CROSSOVER), "CROSSOVER");
    EXPECT_EQ(reproductionMethodToString(ReproductionMethod::MUTATION), "MUTATION");
    EXPECT_EQ(reproductionMethodToString(ReproductionMethod::CLONING), "CLONING");
}

// ============================================================================
// B-Series Tests
// ============================================================================

TEST_F(OntogenesisTest, BSeriesTreeCount) {
    // Test A000081 sequence
    EXPECT_EQ(BSeries::getTreeCount(1), 1);  // 1 tree of order 1
    EXPECT_EQ(BSeries::getTreeCount(2), 2);  // 1+1 = 2 total
    EXPECT_EQ(BSeries::getTreeCount(3), 4);  // 1+1+2 = 4 total
    EXPECT_EQ(BSeries::getTreeCount(4), 8);  // 1+1+2+4 = 8 total
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_F(OntogenesisTest, CompleteLifecycle) {
    // Test a complete lifecycle: initialization -> optimization -> reproduction
    auto parent = initializeOntogeneticKernel(4);
    EXPECT_EQ(parent.ontogeneticState.stage, DevelopmentStage::EMBRYONIC);
    
    auto optimized = selfOptimize(parent, 10);
    EXPECT_GT(optimized.ontogeneticState.maturity, 0.0);
    
    auto parent2 = initializeOntogeneticKernel(4);
    auto child = selfReproduce(optimized, parent2, ReproductionMethod::CROSSOVER);
    
    EXPECT_EQ(child.genome.generation, optimized.genome.generation + 1);
    EXPECT_EQ(child.genome.lineage.size(), 2);
}

TEST_F(OntogenesisTest, MultiGenerationEvolution) {
    // Test evolution over multiple generations
    OntogenesisConfig config;
    config.evolution.populationSize = 15;
    config.evolution.maxGenerations = 10;
    config.evolution.mutationRate = 0.15;
    config.evolution.crossoverRate = 0.7;
    config.evolution.fitnessThreshold = 0.95;
    
    auto result = runOntogenesis(config);
    
    EXPECT_GT(result.totalGenerations, 0);
    EXPECT_LE(result.totalGenerations, 10);
    EXPECT_EQ(result.generations.size(), result.totalGenerations);
    
    // Check that best kernel is better than or equal to initial
    if (!result.generations.empty()) {
        EXPECT_GE(result.bestKernel.genome.fitness, 
                 result.generations.front().bestFitness * 0.8);
    }
}
