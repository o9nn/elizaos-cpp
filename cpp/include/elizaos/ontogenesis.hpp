#pragma once

/**
 * ElizaOS C++ - Ontogenesis Module
 *
 * Self-generating kernels using B-Series differential calculus grammar,
 * genetic programming, and developmental biology metaphors.
 */

#include "elizaos.hpp"
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace elizaos {
namespace ontogenesis {

// ============================================================================
// Enumerations
// ============================================================================

enum class DevelopmentStage {
    EMBRYONIC,
    JUVENILE,
    ADOLESCENT,
    MATURE,
    SENESCENT
};

enum class GeneType {
    COEFFICIENT,
    OPERATOR,
    SYMMETRY,
    TOPOLOGY,
    ACTIVATION
};

// ============================================================================
// B-Series (Butcher series) – order determines complexity
// ============================================================================

struct BSeries {
    int                  order        = 1;
    std::vector<double>  coefficients;

    BSeries() = default;
    explicit BSeries(int ord);

    static int getTreeCount(int order);
};

// ============================================================================
// Kernel gene
// ============================================================================

struct KernelGene {
    std::string          id;
    GeneType             type       = GeneType::COEFFICIENT;
    std::string          name;
    std::vector<double>  values;
    bool                 mutable_   = true;
    double               expression = 1.0;

    KernelGene() = default;
    KernelGene(GeneType t, const std::string& n, const std::vector<double>& v,
               bool mutableFlag = true)
        : type(t), name(n), values(v), mutable_(mutableFlag) {}
};

// ============================================================================
// Kernel genome
// ============================================================================

struct KernelGenome {
    std::string              id;
    int                      generation = 0;
    int                      age        = 0;
    double                   fitness    = 0.0;
    std::vector<std::string> lineage;
    std::vector<KernelGene>  genes;
};

// ============================================================================
// Ontogenetic state
// ============================================================================

struct DevelopmentEvent {
    std::string event;
    int         step = 0;
};

struct OntogeneticState {
    DevelopmentStage         stage    = DevelopmentStage::EMBRYONIC;
    float                    maturity = 0.0f;
    std::vector<DevelopmentEvent> events;
};

// ============================================================================
// OntogeneticKernel – the main entity
// ============================================================================

struct OntogeneticKernel {
    std::string      id;
    BSeries          bseries;
    KernelGenome     genome;
    OntogeneticState ontogeneticState;
};

// ============================================================================
// Grip evaluation
// ============================================================================

struct GripEvaluation {
    double optimalGrip   = 0.0;
    double adaptability  = 0.0;
    double coherence     = 0.0;
    double relevance     = 0.0;
};

// ============================================================================
// Evolution result and config
// ============================================================================

struct EvolutionStats {
    double bestFitness     = 0.0;
    double averageFitness  = 0.0;
    double diversity       = 0.0;
    int    generation      = 0;
    int    eliteCount      = 0;
};

struct EvolutionResult {
    OntogeneticKernel        bestKernel;
    std::vector<EvolutionStats> history;
    int                      totalGenerations = 0;
    bool                     converged        = false;
};

struct EvolutionParameters {
    int    populationSize   = 20;
    int    maxGenerations   = 50;
    double mutationRate     = 0.1;
    double crossoverRate    = 0.7;
    double elitismRate      = 0.1;
    double fitnessThreshold = 0.95;
};

struct DevelopmentSchedule {
    std::vector<int> stageTransitions; // generation indices for each stage
};

struct OntogenesisConfig {
    std::vector<OntogeneticKernel> seedKernels;
    EvolutionParameters            evolution;
    DevelopmentSchedule            developmentSchedule;
};

// ============================================================================
// Free functions
// ============================================================================

OntogeneticKernel initializeOntogeneticKernel(int order);
OntogeneticKernel selfGenerate(const OntogeneticKernel& parent);
OntogeneticKernel selfOptimize(const OntogeneticKernel& kernel, int iterations);
OntogeneticKernel selfReproduce(const OntogeneticKernel& parent1,
                                 const OntogeneticKernel& parent2,
                                 double crossoverRate = 0.5);

std::pair<KernelGenome, KernelGenome> crossover(const KernelGenome& g1, const KernelGenome& g2);
KernelGenome   mutate(const KernelGenome& genome, double mutationRate);
double         geneticDistance(const KernelGenome& g1, const KernelGenome& g2);
KernelGenome   cloneGenome(const KernelGenome& genome);

GripEvaluation evaluateGrip(const OntogeneticKernel& kernel);
double         evaluateFitness(const OntogeneticKernel& kernel);
double         calculateNovelty(const OntogeneticKernel& kernel,
                                 const std::vector<OntogeneticKernel>& population);

EvolutionResult runOntogenesis(const OntogenesisConfig& config);

} // namespace ontogenesis
} // namespace elizaos
