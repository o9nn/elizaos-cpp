/**
 * @file ontogenesis_simple_demo.cpp
 * @brief Simple demonstration of self-generating kernels
 */

#include "elizaos/ontogenesis.hpp"
#include <iostream>
#include <iomanip>

using namespace elizaos::ontogenesis;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Ontogenesis: Self-Generating Kernels" << std::endl;
    std::cout << "  Simple Self-Generation Demo" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Create a parent kernel of order 4
    std::cout << "Creating parent kernel (order 4)..." << std::endl;
    auto parent = initializeOntogeneticKernel(4);
    
    std::cout << "  Parent ID: " << parent.id << std::endl;
    std::cout << "  Generation: " << parent.genome.generation << std::endl;
    std::cout << "  Fitness: " << std::fixed << std::setprecision(4) 
              << parent.genome.fitness << std::endl;
    std::cout << "  Stage: " << developmentStageToString(parent.ontogeneticState.stage) << std::endl;
    std::cout << "  B-series coefficients: " << parent.bseries.coefficients.size() << std::endl;
    std::cout << std::endl;
    
    // Generate offspring through self-composition
    std::cout << "Generating offspring through self-composition..." << std::endl;
    auto offspring = selfGenerate(parent);
    
    std::cout << "  Offspring ID: " << offspring.id << std::endl;
    std::cout << "  Generation: " << offspring.genome.generation << std::endl;
    std::cout << "  Fitness: " << std::fixed << std::setprecision(4) 
              << offspring.genome.fitness << std::endl;
    std::cout << "  Stage: " << developmentStageToString(offspring.ontogeneticState.stage) << std::endl;
    std::cout << "  Parent lineage: " << offspring.genome.lineage.size() << " ancestors" << std::endl;
    std::cout << std::endl;
    
    // Optimize the offspring
    std::cout << "Optimizing offspring (10 iterations)..." << std::endl;
    auto optimized = selfOptimize(offspring, 10);
    
    std::cout << "  Optimized fitness: " << std::fixed << std::setprecision(4) 
              << optimized.genome.fitness << std::endl;
    std::cout << "  Maturity: " << optimized.ontogeneticState.maturity << std::endl;
    std::cout << "  Development events: " << optimized.ontogeneticState.events.size() << std::endl;
    std::cout << std::endl;
    
    // Evaluate grip
    std::cout << "Grip evaluation:" << std::endl;
    auto grip = evaluateGrip(optimized);
    std::cout << "  Contact: " << grip.contact << std::endl;
    std::cout << "  Coverage: " << grip.coverage << std::endl;
    std::cout << "  Efficiency: " << grip.efficiency << std::endl;
    std::cout << "  Stability: " << grip.stability << std::endl;
    std::cout << "  Overall: " << grip.overall << std::endl;
    std::cout << std::endl;
    
    // Show development events
    std::cout << "Development history:" << std::endl;
    for (size_t i = 0; i < optimized.ontogeneticState.events.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " 
                  << optimized.ontogeneticState.events[i] << std::endl;
    }
    std::cout << std::endl;
    
    // Demonstrate reproduction
    std::cout << "Creating second parent and reproducing..." << std::endl;
    auto parent2 = initializeOntogeneticKernel(4);
    
    std::cout << "  Parent 2 fitness: " << parent2.genome.fitness << std::endl;
    
    auto child = selfReproduce(parent, parent2, ReproductionMethod::CROSSOVER);
    std::cout << "  Child ID: " << child.id << std::endl;
    std::cout << "  Child generation: " << child.genome.generation << std::endl;
    std::cout << "  Child fitness: " << child.genome.fitness << std::endl;
    std::cout << "  Child has " << child.genome.lineage.size() << " ancestors" << std::endl;
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo Complete!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
