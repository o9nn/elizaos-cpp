/**
 * @file ontogenesis.cpp
 * @brief Implementation of Ontogenesis: Self-Generating Kernels
 */

#include "elizaos/ontogenesis.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace elizaos {
namespace ontogenesis {

// ============================================================================
// Static Helper Functions
// ============================================================================

namespace {

// Random number generator (thread-local for thread safety)
thread_local std::mt19937 rng(std::random_device{}());

/**
 * @brief Generate random double in range [0, 1]
 */
double randomDouble() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

/**
 * @brief Generate random double in range [min, max]
 */
double randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

/**
 * @brief Generate random integer in range [min, max]
 */
int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

/**
 * @brief Calculate A000081 sequence value (number of rooted trees)
 * 
 * Sequence: 1, 1, 2, 4, 9, 20, 48, 115, 286, 719, ...
 */
int a000081(int n) {
    static const int lookup[] = {0, 1, 1, 2, 4, 9, 20, 48, 115, 286, 719};
    if (n >= 0 && n < 11) {
        return lookup[n];
    }
    // For higher orders, use approximate formula
    // A(n) ≈ C * α^n / n^(3/2) where α ≈ 2.9558 and C ≈ 0.4399
    const double alpha = 2.9558;
    const double C = 0.4399;
    return static_cast<int>(C * std::pow(alpha, n) / std::pow(n, 1.5));
}

} // anonymous namespace

// ============================================================================
// BSeries Implementation
// ============================================================================

int BSeries::getTreeCount(int order) {
    int total = 0;
    for (int i = 1; i <= order; ++i) {
        total += a000081(i);
    }
    return total;
}

// ============================================================================
// Core Operations
// ============================================================================

UUID generateKernelId() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    std::stringstream ss;
    ss << "kernel-" << std::hex << std::setfill('0') << std::setw(12) << millis
       << "-" << std::setw(4) << randomInt(0, 0xFFFF);
    return ss.str();
}

OntogeneticKernel initializeOntogeneticKernel(int order) {
    OntogeneticKernel kernel;
    kernel.id = generateKernelId();
    
    // Initialize B-series
    kernel.bseries = BSeries(order);
    
    // Initialize with classical Runge-Kutta coefficients based on order
    if (order == 1) {
        // Euler method: b_1 = 1
        kernel.bseries.coefficients = {1.0};
    } else if (order == 2) {
        // Midpoint/Heun: b_1 = 0.5, b_2 = 0.5
        kernel.bseries.coefficients = {0.5, 0.5};
    } else if (order == 4) {
        // Classical RK4
        int count = BSeries::getTreeCount(4);
        kernel.bseries.coefficients.resize(count);
        // Simplified initialization
        for (int i = 0; i < count; ++i) {
            kernel.bseries.coefficients[i] = 1.0 / static_cast<double>(count);
        }
    } else {
        // General case: uniform initialization
        int count = BSeries::getTreeCount(order);
        kernel.bseries.coefficients.resize(count);
        for (int i = 0; i < count; ++i) {
            kernel.bseries.coefficients[i] = 1.0 / static_cast<double>(count);
        }
    }
    
    // Initialize genome
    kernel.genome.id = kernel.id;
    kernel.genome.generation = 0;
    kernel.genome.age = 0;
    
    // Create coefficient genes
    KernelGene coeffGene(
        GeneType::COEFFICIENT,
        "bseries_coefficients",
        kernel.bseries.coefficients,
        true  // mutable
    );
    kernel.genome.genes.push_back(coeffGene);
    
    // Create operator gene
    std::vector<double> operatorValues = {
        1.0,  // chain rule weight
        0.5,  // product rule weight
        0.1   // quotient rule weight
    };
    KernelGene operatorGene(
        GeneType::OPERATOR,
        "differential_operators",
        operatorValues,
        true  // mutable
    );
    kernel.genome.genes.push_back(operatorGene);
    
    // Create symmetry gene (immutable)
    std::vector<double> symmetryValues = {1.0}; // preserve symmetry
    KernelGene symmetryGene(
        GeneType::SYMMETRY,
        "symmetry_preservation",
        symmetryValues,
        false  // immutable
    );
    kernel.genome.genes.push_back(symmetryGene);
    
    // Initialize ontogenetic state
    kernel.ontogeneticState.stage = DevelopmentStage::EMBRYONIC;
    kernel.ontogeneticState.maturity = 0.0;
    kernel.ontogeneticState.generationsSinceBirth = 0;
    kernel.ontogeneticState.events.push_back("Initialized from order " + std::to_string(order));
    
    // Evaluate initial fitness
    kernel.genome.fitness = evaluateFitness(kernel);
    
    return kernel;
}

OntogeneticKernel selfGenerate(const OntogeneticKernel& parent) {
    OntogeneticKernel offspring;
    offspring.id = generateKernelId();
    offspring.bseries = BSeries(parent.bseries.order);
    
    // Apply chain rule: (f∘f)' = f'(f(x)) · f'(x)
    // In terms of B-series: compose coefficients
    auto& parentCoeffs = parent.bseries.coefficients;
    auto& childCoeffs = offspring.bseries.coefficients;
    
    childCoeffs.resize(parentCoeffs.size());
    for (size_t i = 0; i < parentCoeffs.size(); ++i) {
        // Self-composition: square the coefficients and normalize
        childCoeffs[i] = parentCoeffs[i] * parentCoeffs[i];
    }
    
    // Normalize to maintain stability
    double sum = std::accumulate(childCoeffs.begin(), childCoeffs.end(), 0.0);
    if (sum > 0.0) {
        for (auto& c : childCoeffs) {
            c /= sum;
        }
    }
    
    // Initialize genome
    offspring.genome.id = offspring.id;
    offspring.genome.generation = parent.genome.generation + 1;
    offspring.genome.age = 0;
    offspring.genome.lineage = parent.genome.lineage;
    offspring.genome.lineage.push_back(parent.genome.id);
    
    // Copy and mutate genes
    for (const auto& parentGene : parent.genome.genes) {
        KernelGene childGene = parentGene;
        
        if (childGene.mutable_ && randomDouble() < 0.1) {
            // Small mutation
            for (auto& val : childGene.values) {
                val += randomDouble(-0.05, 0.05);
                val = std::max(0.0, std::min(1.0, val)); // clamp to [0, 1]
            }
        }
        
        offspring.genome.genes.push_back(childGene);
    }
    
    // Update coefficient gene with new coefficients
    for (auto& gene : offspring.genome.genes) {
        if (gene.type == GeneType::COEFFICIENT) {
            gene.values = childCoeffs;
            break;
        }
    }
    
    // Initialize ontogenetic state
    offspring.ontogeneticState.stage = DevelopmentStage::EMBRYONIC;
    offspring.ontogeneticState.maturity = 0.0;
    offspring.ontogeneticState.generationsSinceBirth = 0;
    offspring.ontogeneticState.events.push_back(
        "Self-generated from parent " + parent.genome.id
    );
    
    // Evaluate fitness
    offspring.genome.fitness = evaluateFitness(offspring);
    
    return offspring;
}

OntogeneticKernel selfOptimize(const OntogeneticKernel& kernel, int iterations) {
    OntogeneticKernel optimized = kernel;
    
    for (int iter = 0; iter < iterations; ++iter) {
        // Gradient-based optimization of coefficients
        auto currentFitness = evaluateFitness(optimized);
        
        // Try small perturbations to improve fitness
        OntogeneticKernel candidate = optimized;
        
        // Find coefficient gene
        for (auto& gene : candidate.genome.genes) {
            if (gene.type == GeneType::COEFFICIENT && gene.mutable_) {
                // Perturb coefficients
                for (auto& coeff : gene.values) {
                    double perturbation = randomDouble(-0.01, 0.01);
                    coeff += perturbation;
                    coeff = std::max(0.0, std::min(1.0, coeff)); // clamp
                }
                
                // Update B-series
                candidate.bseries.coefficients = gene.values;
                break;
            }
        }
        
        // Evaluate candidate
        double candidateFitness = evaluateFitness(candidate);
        
        // Keep if better
        if (candidateFitness > currentFitness) {
            optimized = candidate;
            optimized.ontogeneticState.maturity += 0.01;
            optimized.ontogeneticState.events.push_back(
                "Optimized iteration " + std::to_string(iter) +
                ": fitness " + std::to_string(candidateFitness)
            );
        }
    }
    
    // Update fitness
    optimized.genome.fitness = evaluateFitness(optimized);
    
    return optimized;
}

OntogeneticKernel selfReproduce(
    const OntogeneticKernel& parent1,
    const OntogeneticKernel& parent2,
    ReproductionMethod method
) {
    OntogeneticKernel offspring;
    offspring.id = generateKernelId();
    
    switch (method) {
        case ReproductionMethod::CROSSOVER: {
            auto [genome1, genome2] = crossover(parent1.genome, parent2.genome);
            offspring.genome = genome1; // Use first offspring
            break;
        }
        
        case ReproductionMethod::MUTATION: {
            offspring.genome = mutate(parent1.genome, 0.2);
            break;
        }
        
        case ReproductionMethod::CLONING: {
            offspring.genome = cloneGenome(parent1.genome);
            break;
        }
    }
    
    // Update genome metadata
    offspring.genome.id = offspring.id;
    offspring.genome.generation = std::max(
        parent1.genome.generation,
        parent2.genome.generation
    ) + 1;
    offspring.genome.age = 0;
    offspring.genome.lineage.push_back(parent1.genome.id);
    if (method == ReproductionMethod::CROSSOVER) {
        offspring.genome.lineage.push_back(parent2.genome.id);
    }
    
    // Extract B-series coefficients from genome
    for (const auto& gene : offspring.genome.genes) {
        if (gene.type == GeneType::COEFFICIENT) {
            offspring.bseries.coefficients = gene.values;
            offspring.bseries.order = parent1.bseries.order;
            break;
        }
    }
    
    // Initialize ontogenetic state
    offspring.ontogeneticState.stage = DevelopmentStage::EMBRYONIC;
    offspring.ontogeneticState.maturity = 0.0;
    offspring.ontogeneticState.generationsSinceBirth = 0;
    offspring.ontogeneticState.events.push_back(
        "Reproduced via " + reproductionMethodToString(method)
    );
    
    // Evaluate fitness
    offspring.genome.fitness = evaluateFitness(offspring);
    
    return offspring;
}

// ============================================================================
// Genetic Operations
// ============================================================================

std::pair<KernelGenome, KernelGenome> crossover(
    const KernelGenome& genome1,
    const KernelGenome& genome2
) {
    KernelGenome offspring1 = genome1;
    KernelGenome offspring2 = genome2;
    
    // Perform single-point crossover on each gene type
    for (size_t i = 0; i < std::min(genome1.genes.size(), genome2.genes.size()); ++i) {
        const auto& gene1 = genome1.genes[i];
        const auto& gene2 = genome2.genes[i];
        
        // Only crossover mutable genes of same type
        if (gene1.type != gene2.type || !gene1.mutable_ || !gene2.mutable_) {
            continue;
        }
        
        auto& offGene1 = offspring1.genes[i];
        auto& offGene2 = offspring2.genes[i];
        
        // Single-point crossover on values
        if (!gene1.values.empty() && !gene2.values.empty()) {
            size_t minSize = std::min(gene1.values.size(), gene2.values.size());
            int crossoverPoint = randomInt(0, minSize - 1);
            
            for (size_t j = crossoverPoint; j < minSize; ++j) {
                std::swap(offGene1.values[j], offGene2.values[j]);
            }
        }
    }
    
    return {offspring1, offspring2};
}

KernelGenome mutate(const KernelGenome& genome, double mutationRate) {
    KernelGenome mutated = genome;
    
    for (auto& gene : mutated.genes) {
        if (!gene.mutable_) continue;
        
        // Mutate each value with probability mutationRate
        for (auto& val : gene.values) {
            if (randomDouble() < mutationRate) {
                // Add random perturbation ±10%
                val += randomDouble(-0.1, 0.1);
                val = std::max(0.0, std::min(1.0, val)); // clamp to [0, 1]
            }
        }
    }
    
    return mutated;
}

double geneticDistance(const KernelGenome& genome1, const KernelGenome& genome2) {
    double totalDistance = 0.0;
    int comparisonCount = 0;
    
    // Compare genes
    for (size_t i = 0; i < std::min(genome1.genes.size(), genome2.genes.size()); ++i) {
        const auto& gene1 = genome1.genes[i];
        const auto& gene2 = genome2.genes[i];
        
        if (gene1.type != gene2.type) continue;
        
        // Calculate Euclidean distance between gene values
        size_t minSize = std::min(gene1.values.size(), gene2.values.size());
        double geneDistance = 0.0;
        
        for (size_t j = 0; j < minSize; ++j) {
            double diff = gene1.values[j] - gene2.values[j];
            geneDistance += diff * diff;
        }
        
        if (minSize > 0) {
            geneDistance = std::sqrt(geneDistance / minSize);
            totalDistance += geneDistance;
            comparisonCount++;
        }
    }
    
    return (comparisonCount > 0) ? (totalDistance / comparisonCount) : 1.0;
}

KernelGenome cloneGenome(const KernelGenome& genome) {
    KernelGenome clone = genome;
    clone.id = generateKernelId();
    clone.generation = genome.generation + 1;
    clone.age = 0;
    return clone;
}

// ============================================================================
// Fitness Evaluation
// ============================================================================

GripEvaluation evaluateGrip(const OntogeneticKernel& kernel) {
    GripEvaluation grip;
    
    // Calculate contact: how well coefficients sum to expected value
    double coeffSum = std::accumulate(
        kernel.bseries.coefficients.begin(),
        kernel.bseries.coefficients.end(),
        0.0
    );
    grip.contact = 1.0 - std::abs(coeffSum - 1.0); // Should sum to ~1.0
    grip.contact = std::max(0.0, std::min(1.0, grip.contact));
    
    // Calculate coverage: how many non-zero coefficients
    int nonZeroCount = std::count_if(
        kernel.bseries.coefficients.begin(),
        kernel.bseries.coefficients.end(),
        [](double c) { return std::abs(c) > 1e-6; }
    );
    grip.coverage = static_cast<double>(nonZeroCount) / 
                    std::max(1.0, static_cast<double>(kernel.bseries.coefficients.size()));
    
    // Calculate efficiency: prefer fewer but larger coefficients
    double maxCoeff = 0.0;
    if (!kernel.bseries.coefficients.empty()) {
        maxCoeff = *std::max_element(
            kernel.bseries.coefficients.begin(),
            kernel.bseries.coefficients.end()
        );
    }
    grip.efficiency = maxCoeff;
    
    // Calculate stability: variance should be moderate
    if (!kernel.bseries.coefficients.empty()) {
        double mean = coeffSum / kernel.bseries.coefficients.size();
        double variance = 0.0;
        for (double c : kernel.bseries.coefficients) {
            double diff = c - mean;
            variance += diff * diff;
        }
        variance /= kernel.bseries.coefficients.size();
        
        // Prefer moderate variance (not too uniform, not too spiky)
        double targetVariance = 0.1;
        grip.stability = 1.0 - std::abs(variance - targetVariance);
        grip.stability = std::max(0.0, std::min(1.0, grip.stability));
    }
    
    // Overall grip
    grip.overall = (grip.contact * 0.4 +
                   grip.coverage * 0.2 +
                   grip.efficiency * 0.2 +
                   grip.stability * 0.2);
    
    return grip;
}

double evaluateFitness(
    const OntogeneticKernel& kernel,
    const std::vector<OntogeneticKernel>* population
) {
    // Evaluate grip
    GripEvaluation grip = evaluateGrip(kernel);
    
    // Calculate stability (reuse from grip)
    double stability = grip.stability;
    
    // Calculate efficiency (reuse from grip)
    double efficiency = grip.efficiency;
    
    // Calculate novelty if population provided
    double novelty = 0.5; // default
    if (population != nullptr && !population->empty()) {
        novelty = calculateNovelty(kernel, *population);
    }
    
    // Calculate symmetry preservation
    double symmetry = 1.0; // default: assume symmetry preserved
    for (const auto& gene : kernel.genome.genes) {
        if (gene.type == GeneType::SYMMETRY && !gene.values.empty()) {
            symmetry = gene.values[0];
            break;
        }
    }
    
    // Weighted fitness
    double fitness = (
        grip.overall * 0.4 +
        stability * 0.2 +
        efficiency * 0.2 +
        novelty * 0.1 +
        symmetry * 0.1
    );
    
    return std::max(0.0, std::min(1.0, fitness));
}

double calculateNovelty(
    const OntogeneticKernel& kernel,
    const std::vector<OntogeneticKernel>& population
) {
    if (population.empty()) return 0.5;
    
    double totalDistance = 0.0;
    for (const auto& other : population) {
        if (other.id != kernel.id) {
            totalDistance += geneticDistance(kernel.genome, other.genome);
        }
    }
    
    double avgDistance = totalDistance / population.size();
    return std::min(1.0, avgDistance); // Normalize to [0, 1]
}

// ============================================================================
// Evolution System
// ============================================================================

EvolutionResult runOntogenesis(const OntogenesisConfig& config) {
    EvolutionResult result;
    
    // Initialize population
    std::vector<OntogeneticKernel> population;
    
    if (!config.seedKernels.empty()) {
        population = config.seedKernels;
    } else {
        // Create random population
        for (int i = 0; i < config.evolution.populationSize; ++i) {
            int order = randomInt(1, 4);
            population.push_back(initializeOntogeneticKernel(order));
        }
    }
    
    // Expand to full population size if needed
    while (static_cast<int>(population.size()) < config.evolution.populationSize) {
        int order = randomInt(1, 4);
        population.push_back(initializeOntogeneticKernel(order));
    }
    
    // Evolution loop
    for (int gen = 0; gen < config.evolution.maxGenerations; ++gen) {
        // Evaluate all fitnesses
        for (auto& kernel : population) {
            kernel.genome.fitness = evaluateFitness(kernel, &population);
        }
        
        // Sort by fitness
        std::sort(population.begin(), population.end(),
            [](const OntogeneticKernel& a, const OntogeneticKernel& b) {
                return a.genome.fitness > b.genome.fitness;
            });
        
        // Calculate statistics
        GenerationStats stats;
        stats.generation = gen;
        stats.bestFitness = population[0].genome.fitness;
        stats.worstFitness = population.back().genome.fitness;
        
        double sumFitness = 0.0;
        for (const auto& k : population) {
            sumFitness += k.genome.fitness;
        }
        stats.averageFitness = sumFitness / population.size();
        stats.diversity = calculatePopulationDiversity(population);
        stats.eliteCount = static_cast<int>(population.size() * config.evolution.elitismRate);
        
        result.generations.push_back(stats);
        
        // Check convergence
        if (stats.bestFitness >= config.evolution.fitnessThreshold) {
            result.converged = true;
            result.totalGenerations = gen + 1;
            break;
        }
        
        // Create next generation
        std::vector<OntogeneticKernel> nextGen;
        
        // Elitism: preserve best individuals
        int eliteCount = stats.eliteCount;
        for (int i = 0; i < eliteCount; ++i) {
            nextGen.push_back(population[i]);
            updateDevelopmentStage(nextGen.back(), config.developmentSchedule);
        }
        
        // Generate offspring
        while (static_cast<int>(nextGen.size()) < config.evolution.populationSize) {
            // Select parents via tournament
            const auto& parent1 = tournamentSelect(population);
            const auto& parent2 = tournamentSelect(population);
            
            // Reproduce
            ReproductionMethod method;
            double r = randomDouble();
            if (r < config.evolution.crossoverRate) {
                method = ReproductionMethod::CROSSOVER;
            } else if (r < config.evolution.crossoverRate + config.evolution.mutationRate) {
                method = ReproductionMethod::MUTATION;
            } else {
                method = ReproductionMethod::CLONING;
            }
            
            auto offspring = selfReproduce(parent1, parent2, method);
            nextGen.push_back(offspring);
        }
        
        population = std::move(nextGen);
    }
    
    // Final evaluation
    for (auto& kernel : population) {
        kernel.genome.fitness = evaluateFitness(kernel, &population);
    }
    
    std::sort(population.begin(), population.end(),
        [](const OntogeneticKernel& a, const OntogeneticKernel& b) {
            return a.genome.fitness > b.genome.fitness;
        });
    
    result.finalPopulation = population;
    result.bestKernel = population[0];
    if (!result.converged) {
        result.totalGenerations = config.evolution.maxGenerations;
    }
    
    return result;
}

const OntogeneticKernel& tournamentSelect(
    const std::vector<OntogeneticKernel>& population,
    int tournamentSize
) {
    int bestIdx = randomInt(0, population.size() - 1);
    double bestFitness = population[bestIdx].genome.fitness;
    
    for (int i = 1; i < tournamentSize; ++i) {
        int idx = randomInt(0, population.size() - 1);
        if (population[idx].genome.fitness > bestFitness) {
            bestIdx = idx;
            bestFitness = population[idx].genome.fitness;
        }
    }
    
    return population[bestIdx];
}

void updateDevelopmentStage(
    OntogeneticKernel& kernel,
    const DevelopmentSchedule& schedule
) {
    kernel.ontogeneticState.generationsSinceBirth++;
    kernel.genome.age++;
    
    int age = kernel.ontogeneticState.generationsSinceBirth;
    double fitness = kernel.genome.fitness;
    
    // Update stage based on age and fitness
    if (age < schedule.embryonicDuration) {
        kernel.ontogeneticState.stage = DevelopmentStage::EMBRYONIC;
        kernel.ontogeneticState.maturity = static_cast<double>(age) / schedule.embryonicDuration * 0.25;
    } else if (age < schedule.embryonicDuration + schedule.juvenileDuration) {
        kernel.ontogeneticState.stage = DevelopmentStage::JUVENILE;
        int juvenileAge = age - schedule.embryonicDuration;
        kernel.ontogeneticState.maturity = 0.25 + static_cast<double>(juvenileAge) / schedule.juvenileDuration * 0.50;
    } else if (age < schedule.embryonicDuration + schedule.juvenileDuration + schedule.matureDuration) {
        if (fitness >= schedule.maturityThreshold) {
            kernel.ontogeneticState.stage = DevelopmentStage::MATURE;
            kernel.ontogeneticState.maturity = 0.75 + (fitness - schedule.maturityThreshold) / (1.0 - schedule.maturityThreshold) * 0.25;
        } else {
            kernel.ontogeneticState.stage = DevelopmentStage::JUVENILE;
        }
    } else {
        kernel.ontogeneticState.stage = DevelopmentStage::SENESCENT;
        kernel.ontogeneticState.maturity = std::max(0.0, kernel.ontogeneticState.maturity - 0.01);
    }
}

double calculatePopulationDiversity(const std::vector<OntogeneticKernel>& population) {
    if (population.size() < 2) return 0.0;
    
    double totalDistance = 0.0;
    int comparisons = 0;
    
    for (size_t i = 0; i < population.size(); ++i) {
        for (size_t j = i + 1; j < population.size(); ++j) {
            totalDistance += geneticDistance(population[i].genome, population[j].genome);
            comparisons++;
        }
    }
    
    return (comparisons > 0) ? (totalDistance / comparisons) : 0.0;
}

// ============================================================================
// Utility Functions
// ============================================================================

std::string developmentStageToString(DevelopmentStage stage) {
    switch (stage) {
        case DevelopmentStage::EMBRYONIC:  return "EMBRYONIC";
        case DevelopmentStage::JUVENILE:   return "JUVENILE";
        case DevelopmentStage::MATURE:     return "MATURE";
        case DevelopmentStage::SENESCENT:  return "SENESCENT";
        default:                           return "UNKNOWN";
    }
}

std::string reproductionMethodToString(ReproductionMethod method) {
    switch (method) {
        case ReproductionMethod::CROSSOVER: return "CROSSOVER";
        case ReproductionMethod::MUTATION:  return "MUTATION";
        case ReproductionMethod::CLONING:   return "CLONING";
        default:                            return "UNKNOWN";
    }
}

} // namespace ontogenesis
} // namespace elizaos
