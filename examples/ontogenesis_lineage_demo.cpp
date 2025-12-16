/**
 * @file ontogenesis_lineage_demo.cpp
 * @brief Lineage tracking demonstration
 */

#include "elizaos/ontogenesis.hpp"
#include <iostream>
#include <iomanip>
#include <map>

using namespace elizaos::ontogenesis;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Ontogenesis: Self-Generating Kernels" << std::endl;
    std::cout << "  Lineage Tracking Demo" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Create ancestor kernel
    std::cout << "Creating ancestor kernel..." << std::endl;
    auto ancestor = initializeOntogeneticKernel(4);
    
    std::cout << "  Ancestor ID: " << ancestor.id << std::endl;
    std::cout << "  Initial fitness: " << ancestor.genome.fitness << std::endl;
    std::cout << std::endl;
    
    // Generate lineage through self-generation
    std::cout << "Generating lineage (10 generations)..." << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << std::setw(5) << "Gen"
              << std::setw(25) << "Kernel ID"
              << std::setw(12) << "Fitness"
              << std::setw(15) << "Stage"
              << std::setw(12) << "Maturity"
              << std::setw(11) << "Ancestors"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    std::vector<OntogeneticKernel> lineage;
    lineage.push_back(ancestor);
    
    // Display ancestor
    std::cout << std::setw(5) << 0
              << std::setw(25) << ancestor.id.substr(0, 22) << "..."
              << std::fixed << std::setprecision(4)
              << std::setw(12) << ancestor.genome.fitness
              << std::setw(15) << developmentStageToString(ancestor.ontogeneticState.stage)
              << std::setprecision(2)
              << std::setw(12) << ancestor.ontogeneticState.maturity
              << std::setw(11) << ancestor.genome.lineage.size()
              << std::endl;
    
    // Generate descendants
    for (int gen = 1; gen <= 10; ++gen) {
        auto parent = lineage.back();
        auto offspring = selfGenerate(parent);
        
        // Optimize offspring
        offspring = selfOptimize(offspring, 5);
        
        lineage.push_back(offspring);
        
        std::cout << std::setw(5) << gen
                  << std::setw(25) << offspring.id.substr(0, 22) << "..."
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << offspring.genome.fitness
                  << std::setw(15) << developmentStageToString(offspring.ontogeneticState.stage)
                  << std::setprecision(2)
                  << std::setw(12) << offspring.ontogeneticState.maturity
                  << std::setw(11) << offspring.genome.lineage.size()
                  << std::endl;
    }
    
    std::cout << std::string(80, '-') << std::endl;
    std::cout << std::endl;
    
    // Analyze lineage
    std::cout << "Lineage Analysis:" << std::endl;
    std::cout << "  Total generations: " << lineage.size() << std::endl;
    
    // Fitness progression
    std::cout << "  Fitness progression:" << std::endl;
    for (size_t i = 0; i < lineage.size(); ++i) {
        std::cout << "    Gen " << i << ": " << lineage[i].genome.fitness;
        if (i > 0) {
            double change = lineage[i].genome.fitness - lineage[i-1].genome.fitness;
            std::cout << " (" << (change >= 0 ? "+" : "") << change << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // Stage progression
    std::cout << "  Stage progression:" << std::endl;
    std::map<DevelopmentStage, int> stageChanges;
    for (size_t i = 0; i < lineage.size(); ++i) {
        stageChanges[lineage[i].ontogeneticState.stage]++;
        if (i > 0 && lineage[i].ontogeneticState.stage != lineage[i-1].ontogeneticState.stage) {
            std::cout << "    Gen " << i << ": "
                      << developmentStageToString(lineage[i-1].ontogeneticState.stage)
                      << " -> "
                      << developmentStageToString(lineage[i].ontogeneticState.stage)
                      << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Genetic distance analysis
    std::cout << "  Genetic distances from ancestor:" << std::endl;
    for (size_t i = 1; i < lineage.size(); ++i) {
        double distance = geneticDistance(lineage[0].genome, lineage[i].genome);
        std::cout << "    Gen " << i << ": " << distance << std::endl;
    }
    std::cout << std::endl;
    
    // Grip evolution
    std::cout << "  Grip evolution:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), lineage.size()); ++i) {
        auto grip = evaluateGrip(lineage[i]);
        std::cout << "    Gen " << i << ": overall=" << grip.overall
                  << " (contact=" << grip.contact
                  << ", coverage=" << grip.coverage
                  << ", efficiency=" << grip.efficiency
                  << ", stability=" << grip.stability << ")"
                  << std::endl;
    }
    std::cout << std::endl;
    
    // Create family tree with reproduction
    std::cout << "Creating family tree with reproduction..." << std::endl;
    
    // Create two branches
    auto branch1_parent = initializeOntogeneticKernel(4);
    auto branch2_parent = initializeOntogeneticKernel(4);
    
    std::cout << "  Branch 1 parent fitness: " << branch1_parent.genome.fitness << std::endl;
    std::cout << "  Branch 2 parent fitness: " << branch2_parent.genome.fitness << std::endl;
    
    // Generate offspring from both branches
    auto branch1_child1 = selfGenerate(branch1_parent);
    auto branch1_child2 = selfGenerate(branch1_parent);
    auto branch2_child1 = selfGenerate(branch2_parent);
    auto branch2_child2 = selfGenerate(branch2_parent);
    
    std::cout << "  Branch 1 generated 2 children" << std::endl;
    std::cout << "  Branch 2 generated 2 children" << std::endl;
    
    // Cross-breed
    auto hybrid1 = selfReproduce(branch1_child1, branch2_child1, ReproductionMethod::CROSSOVER);
    auto hybrid2 = selfReproduce(branch1_child2, branch2_child2, ReproductionMethod::CROSSOVER);
    
    std::cout << "  Created 2 hybrid offspring" << std::endl;
    std::cout << "    Hybrid 1 fitness: " << hybrid1.genome.fitness << std::endl;
    std::cout << "    Hybrid 2 fitness: " << hybrid2.genome.fitness << std::endl;
    std::cout << "    Hybrid 1 has " << hybrid1.genome.lineage.size() << " ancestors" << std::endl;
    std::cout << "    Hybrid 2 has " << hybrid2.genome.lineage.size() << " ancestors" << std::endl;
    std::cout << std::endl;
    
    // Final statistics
    std::cout << "Final Statistics:" << std::endl;
    std::cout << "  Simple lineage kernels: " << lineage.size() << std::endl;
    std::cout << "  Family tree kernels: 6 (2 parents + 4 children + 2 hybrids)" << std::endl;
    std::cout << "  Best fitness in lineage: " 
              << std::max_element(lineage.begin(), lineage.end(),
                    [](const auto& a, const auto& b) {
                        return a.genome.fitness < b.genome.fitness;
                    })->genome.fitness
              << std::endl;
    std::cout << "  Genetic diversity in lineage: "
              << calculatePopulationDiversity(lineage)
              << std::endl;
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Lineage Demo Complete!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
