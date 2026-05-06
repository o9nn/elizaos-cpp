/**
 * @file ontogenesis_evolution_demo.cpp
 * @brief Multi-generation evolution demonstration
 */

#include "elizaos/ontogenesis.hpp"
#include <iostream>
#include <iomanip>

using namespace elizaos::ontogenesis;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Ontogenesis: Self-Generating Kernels" << std::endl;
    std::cout << "  Multi-Generation Evolution Demo" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Configure evolution
    OntogenesisConfig config;
    config.evolution.populationSize = 20;
    config.evolution.mutationRate = 0.15;
    config.evolution.crossoverRate = 0.7;
    config.evolution.elitismRate = 0.1;
    config.evolution.maxGenerations = 30;
    config.evolution.fitnessThreshold = 0.85;
    config.evolution.diversityPressure = 0.2;
    
    // Create seed kernels
    std::cout << "Creating seed kernels..." << std::endl;
    config.seedKernels.push_back(initializeOntogeneticKernel(2));
    config.seedKernels.push_back(initializeOntogeneticKernel(4));
    
    std::cout << "  Seed 1 fitness: " << config.seedKernels[0].genome.fitness << std::endl;
    std::cout << "  Seed 2 fitness: " << config.seedKernels[1].genome.fitness << std::endl;
    std::cout << std::endl;
    
    // Display evolution parameters
    std::cout << "Evolution parameters:" << std::endl;
    std::cout << "  Population size: " << config.evolution.populationSize << std::endl;
    std::cout << "  Mutation rate: " << config.evolution.mutationRate << std::endl;
    std::cout << "  Crossover rate: " << config.evolution.crossoverRate << std::endl;
    std::cout << "  Elitism rate: " << config.evolution.elitismRate << std::endl;
    std::cout << "  Max generations: " << config.evolution.maxGenerations << std::endl;
    std::cout << "  Fitness threshold: " << config.evolution.fitnessThreshold << std::endl;
    std::cout << std::endl;
    
    // Run evolution
    std::cout << "Running evolution..." << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << std::setw(6) << "Gen"
              << std::setw(12) << "Best"
              << std::setw(12) << "Average"
              << std::setw(12) << "Worst"
              << std::setw(12) << "Diversity"
              << std::setw(10) << "Elites"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    auto result = runOntogenesis(config);
    
    // Display generation statistics
    for (const auto& gen : result.generations) {
        std::cout << std::setw(6) << gen.generation
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << gen.bestFitness
                  << std::setw(12) << gen.averageFitness
                  << std::setw(12) << gen.worstFitness
                  << std::setw(12) << gen.diversity
                  << std::setw(10) << gen.eliteCount
                  << std::endl;
    }
    
    std::cout << std::string(70, '-') << std::endl;
    std::cout << std::endl;
    
    // Display results
    std::cout << "Evolution Results:" << std::endl;
    std::cout << "  Converged: " << (result.converged ? "YES" : "NO") << std::endl;
    std::cout << "  Total generations: " << result.totalGenerations << std::endl;
    std::cout << "  Final population: " << result.finalPopulation.size() << std::endl;
    std::cout << std::endl;
    
    // Analyze best kernel
    std::cout << "Best Kernel Analysis:" << std::endl;
    std::cout << "  ID: " << result.bestKernel.id << std::endl;
    std::cout << "  Generation: " << result.bestKernel.genome.generation << std::endl;
    std::cout << "  Fitness: " << result.bestKernel.genome.fitness << std::endl;
    std::cout << "  Stage: " << developmentStageToString(result.bestKernel.ontogeneticState.stage) << std::endl;
    std::cout << "  Maturity: " << result.bestKernel.ontogeneticState.maturity << std::endl;
    std::cout << "  Age: " << result.bestKernel.genome.age << " generations" << std::endl;
    std::cout << "  Lineage: " << result.bestKernel.genome.lineage.size() << " ancestors" << std::endl;
    std::cout << std::endl;
    
    // Fitness components
    std::cout << "Fitness Components:" << std::endl;
    std::cout << "  Grip: " << result.bestKernel.genome.grip << std::endl;
    std::cout << "  Stability: " << result.bestKernel.genome.stability << std::endl;
    std::cout << "  Efficiency: " << result.bestKernel.genome.efficiency << std::endl;
    std::cout << "  Novelty: " << result.bestKernel.genome.novelty << std::endl;
    std::cout << "  Symmetry: " << result.bestKernel.genome.symmetry << std::endl;
    std::cout << std::endl;
    
    // Top 5 individuals
    std::cout << "Top 5 Individuals:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), result.finalPopulation.size()); ++i) {
        const auto& kernel = result.finalPopulation[i];
        std::cout << "  " << (i + 1) << ". Fitness: " << kernel.genome.fitness
                  << ", Gen: " << kernel.genome.generation
                  << ", Stage: " << developmentStageToString(kernel.ontogeneticState.stage)
                  << std::endl;
    }
    std::cout << std::endl;
    
    // Genetic diversity analysis
    std::cout << "Population Diversity Analysis:" << std::endl;
    double diversity = calculatePopulationDiversity(result.finalPopulation);
    std::cout << "  Overall diversity: " << diversity << std::endl;
    
    // Count stages
    int embryonic = 0, juvenile = 0, mature = 0, senescent = 0;
    for (const auto& kernel : result.finalPopulation) {
        switch (kernel.ontogeneticState.stage) {
            case DevelopmentStage::EMBRYONIC:  embryonic++; break;
            case DevelopmentStage::JUVENILE:   juvenile++; break;
            case DevelopmentStage::MATURE:     mature++; break;
            case DevelopmentStage::SENESCENT:  senescent++; break;
        }
    }
    
    std::cout << "  Stage distribution:" << std::endl;
    std::cout << "    Embryonic: " << embryonic << std::endl;
    std::cout << "    Juvenile: " << juvenile << std::endl;
    std::cout << "    Mature: " << mature << std::endl;
    std::cout << "    Senescent: " << senescent << std::endl;
    std::cout << std::endl;
    
    // Evolution trajectory analysis
    if (!result.generations.empty()) {
        double improvement = result.generations.back().bestFitness - 
                           result.generations.front().bestFitness;
        double avgImprovement = improvement / result.totalGenerations;
        
        std::cout << "Evolution Trajectory:" << std::endl;
        std::cout << "  Initial best: " << result.generations.front().bestFitness << std::endl;
        std::cout << "  Final best: " << result.generations.back().bestFitness << std::endl;
        std::cout << "  Improvement: " << improvement << std::endl;
        std::cout << "  Avg improvement per gen: " << avgImprovement << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Evolution Demo Complete!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
