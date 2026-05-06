/**
 * @file ontogenesis.hpp
 * @brief Ontogenesis: Self-Generating Kernels
 * 
 * Implementation of self-generating, evolving kernels through recursive
 * application of differential operators. Enables kernels to generate
 * themselves, optimize themselves, reproduce with other kernels, and
 * evolve across generations.
 * 
 * Based on B-series expansions and the A000081 sequence of rooted trees.
 */

#ifndef ELIZAOS_ONTOGENESIS_HPP
#define ELIZAOS_ONTOGENESIS_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <optional>
#include <cstdint>
#include <random>

namespace elizaos {
namespace ontogenesis {

// ============================================================================
// Type Aliases
// ============================================================================

using UUID = std::string;
using CoefficientArray = std::vector<double>;
using OperatorArray = std::vector<std::string>;

// ============================================================================
// Enumerations
// ============================================================================

/**
 * @brief Development stages of an ontogenetic kernel
 * 
 * Kernels progress through life stages similar to biological organisms:
 * - Embryonic: Just generated, basic structure
 * - Juvenile: Developing, optimizing
 * - Mature: Fully developed, capable of reproduction
 * - Senescent: Declining, ready for replacement
 */
enum class DevelopmentStage {
    EMBRYONIC,   ///< Just generated, basic structure
    JUVENILE,    ///< Developing, optimizing
    MATURE,      ///< Fully developed, capable of reproduction
    SENESCENT    ///< Declining, ready for replacement
};

/**
 * @brief Types of genes in a kernel genome
 */
enum class GeneType {
    COEFFICIENT,   ///< B-series coefficients (mutable)
    OPERATOR,      ///< Differential operators (mutable)
    SYMMETRY,      ///< Domain symmetries (immutable)
    PRESERVATION   ///< Conserved quantities (immutable)
};

/**
 * @brief Reproduction methods for kernel combination
 */
enum class ReproductionMethod {
    CROSSOVER,   ///< Single-point genetic crossover
    MUTATION,    ///< Random coefficient mutation
    CLONING      ///< Direct copy
};

// ============================================================================
// Core Data Structures
// ============================================================================

/**
 * @brief A single gene in the kernel genome
 */
struct KernelGene {
    GeneType type;              ///< Type of gene
    std::string name;           ///< Gene identifier
    std::vector<double> values; ///< Gene values (coefficients, etc.)
    bool mutable_;              ///< Whether gene can be modified
    double mutationRate;        ///< Probability of mutation
    
    KernelGene() 
        : type(GeneType::COEFFICIENT)
        , name("")
        , mutable_(true)
        , mutationRate(0.1)
    {}
    
    KernelGene(GeneType t, const std::string& n, const std::vector<double>& v, bool mut = true)
        : type(t)
        , name(n)
        , values(v)
        , mutable_(mut)
        , mutationRate(0.1)
    {}
};

/**
 * @brief The "DNA" of a kernel
 * 
 * Contains all genetic information defining a kernel's structure,
 * behavior, and evolutionary history.
 */
struct KernelGenome {
    UUID id;                         ///< Unique identifier
    int generation;                  ///< Generation number (0 = original)
    std::vector<UUID> lineage;       ///< Parent IDs
    std::vector<KernelGene> genes;   ///< Genetic information
    double fitness;                  ///< Overall fitness score
    int age;                         ///< Age in generations
    
    // Fitness components
    double grip;                     ///< Quality of domain fit
    double stability;                ///< Numerical stability
    double efficiency;               ///< Computational efficiency
    double novelty;                  ///< Genetic diversity
    double symmetry;                 ///< Symmetry preservation
    
    KernelGenome()
        : id("")
        , generation(0)
        , fitness(0.0)
        , age(0)
        , grip(0.0)
        , stability(0.0)
        , efficiency(0.0)
        , novelty(0.0)
        , symmetry(0.0)
    {}
};

/**
 * @brief Development state of an ontogenetic kernel
 */
struct OntogeneticState {
    DevelopmentStage stage;               ///< Current development stage
    double maturity;                      ///< Maturity level (0.0 to 1.0)
    int generationsSinceBirth;            ///< Age in generations
    std::vector<std::string> events;      ///< Development history
    
    OntogeneticState()
        : stage(DevelopmentStage::EMBRYONIC)
        , maturity(0.0)
        , generationsSinceBirth(0)
    {}
};

/**
 * @brief B-series coefficients for numerical integration
 * 
 * The B-series expansion serves as the genetic code:
 * y_{n+1} = y_n + h * Σ b_i * Φ_i(f, y_n)
 * 
 * Where:
 * - b_i are the coefficient genes
 * - Φ_i are elementary differentials (rooted trees)
 * - Trees follow A000081 sequence: 1, 1, 2, 4, 9, 20, 48, 115, ...
 */
struct BSeries {
    int order;                      ///< Order of the method
    CoefficientArray coefficients;  ///< B-series coefficients
    
    BSeries() : order(0) {}
    explicit BSeries(int ord) : order(ord) {
        // Initialize with A000081 sequence counts
        // Number of rooted trees by order: 1, 1, 2, 4, 9, 20, 48, 115, ...
        coefficients.resize(getTreeCount(ord), 0.0);
    }
    
    static int getTreeCount(int order);
};

/**
 * @brief Grip evaluation components
 * 
 * Measures how well the kernel's differential structure matches the domain
 */
struct GripEvaluation {
    double contact;      ///< How well kernel touches domain
    double coverage;     ///< Completeness of span
    double efficiency;   ///< Computational cost
    double stability;    ///< Numerical properties
    double overall;      ///< Combined grip score
    
    GripEvaluation()
        : contact(0.0)
        , coverage(0.0)
        , efficiency(0.0)
        , stability(0.0)
        , overall(0.0)
    {}
};

/**
 * @brief An ontogenetic kernel with genetic capabilities
 * 
 * Enhanced kernel that can generate new kernels through recursive
 * self-composition, optimize itself, and reproduce with other kernels.
 */
struct OntogeneticKernel {
    UUID id;                           ///< Unique identifier
    KernelGenome genome;               ///< Genetic information
    OntogeneticState ontogeneticState; ///< Development state
    BSeries bseries;                   ///< B-series coefficients
    
    // Cache for performance
    std::optional<GripEvaluation> cachedGrip;
    
    OntogeneticKernel() : id("") {}
};

/**
 * @brief Development schedule configuration
 */
struct DevelopmentSchedule {
    int embryonicDuration;      ///< Generations in embryonic stage
    int juvenileDuration;       ///< Generations in juvenile stage
    int matureDuration;         ///< Generations in mature stage
    double maturityThreshold;   ///< Fitness threshold for maturity
    
    DevelopmentSchedule()
        : embryonicDuration(2)
        , juvenileDuration(5)
        , matureDuration(10)
        , maturityThreshold(0.8)
    {}
};

/**
 * @brief Evolution configuration
 */
struct EvolutionConfig {
    int populationSize;         ///< Number of individuals
    double mutationRate;        ///< Probability of mutation
    double crossoverRate;       ///< Probability of crossover
    double elitismRate;         ///< Fraction of elites to preserve
    int maxGenerations;         ///< Maximum generations to evolve
    double fitnessThreshold;    ///< Target fitness to reach
    double diversityPressure;   ///< Pressure to maintain diversity (0.0-1.0)
    
    EvolutionConfig()
        : populationSize(20)
        , mutationRate(0.1)
        , crossoverRate(0.7)
        , elitismRate(0.2)
        , maxGenerations(100)
        , fitnessThreshold(0.9)
        , diversityPressure(0.1)
    {}
};

/**
 * @brief Complete ontogenesis configuration
 */
struct OntogenesisConfig {
    EvolutionConfig evolution;
    DevelopmentSchedule developmentSchedule;
    std::vector<OntogeneticKernel> seedKernels;
    std::function<double(const OntogeneticKernel&)> customFitnessFunction;
    
    OntogenesisConfig() = default;
};

/**
 * @brief Statistics for a single generation
 */
struct GenerationStats {
    int generation;           ///< Generation number
    double bestFitness;       ///< Best fitness in generation
    double averageFitness;    ///< Average fitness
    double worstFitness;      ///< Worst fitness
    double diversity;         ///< Genetic diversity measure
    int eliteCount;           ///< Number of elite individuals
    
    GenerationStats()
        : generation(0)
        , bestFitness(0.0)
        , averageFitness(0.0)
        , worstFitness(0.0)
        , diversity(0.0)
        , eliteCount(0)
    {}
};

/**
 * @brief Result of evolution run
 */
struct EvolutionResult {
    std::vector<GenerationStats> generations;
    std::vector<OntogeneticKernel> finalPopulation;
    OntogeneticKernel bestKernel;
    bool converged;
    int totalGenerations;
    
    EvolutionResult()
        : converged(false)
        , totalGenerations(0)
    {}
};

// ============================================================================
// Core Operations
// ============================================================================

/**
 * @brief Initialize an ontogenetic kernel from B-series order
 * @param order Order of the B-series method
 * @return New ontogenetic kernel
 */
OntogeneticKernel initializeOntogeneticKernel(int order);

/**
 * @brief Generate offspring through recursive self-composition
 * 
 * Applies the chain rule: (f∘f)' = f'(f(x)) · f'(x)
 * 
 * @param parent Parent kernel
 * @return Offspring kernel
 */
OntogeneticKernel selfGenerate(const OntogeneticKernel& parent);

/**
 * @brief Optimize kernel through iterative grip improvement
 * @param kernel Kernel to optimize
 * @param iterations Number of optimization iterations
 * @return Optimized kernel
 */
OntogeneticKernel selfOptimize(const OntogeneticKernel& kernel, int iterations = 10);

/**
 * @brief Combine two kernels to create offspring
 * @param parent1 First parent
 * @param parent2 Second parent
 * @param method Reproduction method (crossover, mutation, cloning)
 * @return Offspring kernel
 */
OntogeneticKernel selfReproduce(
    const OntogeneticKernel& parent1,
    const OntogeneticKernel& parent2,
    ReproductionMethod method = ReproductionMethod::CROSSOVER
);

// ============================================================================
// Genetic Operations
// ============================================================================

/**
 * @brief Perform single-point crossover on two genomes
 * @param genome1 First parent genome
 * @param genome2 Second parent genome
 * @return Pair of offspring genomes
 */
std::pair<KernelGenome, KernelGenome> crossover(
    const KernelGenome& genome1,
    const KernelGenome& genome2
);

/**
 * @brief Mutate a genome
 * @param genome Genome to mutate
 * @param mutationRate Probability of mutation per gene
 * @return Mutated genome
 */
KernelGenome mutate(const KernelGenome& genome, double mutationRate);

/**
 * @brief Calculate genetic distance between two genomes
 * @param genome1 First genome
 * @param genome2 Second genome
 * @return Distance (0.0 = identical, 1.0 = maximum difference)
 */
double geneticDistance(const KernelGenome& genome1, const KernelGenome& genome2);

/**
 * @brief Clone a genome
 * @param genome Genome to clone
 * @return Cloned genome
 */
KernelGenome cloneGenome(const KernelGenome& genome);

// ============================================================================
// Fitness Evaluation
// ============================================================================

/**
 * @brief Evaluate grip of a kernel
 * @param kernel Kernel to evaluate
 * @return Grip evaluation
 */
GripEvaluation evaluateGrip(const OntogeneticKernel& kernel);

/**
 * @brief Evaluate overall fitness of a kernel
 * @param kernel Kernel to evaluate
 * @param population Optional population for novelty calculation
 * @return Fitness score (0.0 to 1.0)
 */
double evaluateFitness(
    const OntogeneticKernel& kernel,
    const std::vector<OntogeneticKernel>* population = nullptr
);

/**
 * @brief Calculate novelty score (genetic diversity)
 * @param kernel Kernel to evaluate
 * @param population Population for comparison
 * @return Novelty score (0.0 to 1.0)
 */
double calculateNovelty(
    const OntogeneticKernel& kernel,
    const std::vector<OntogeneticKernel>& population
);

// ============================================================================
// Evolution System
// ============================================================================

/**
 * @brief Run ontogenesis evolution
 * @param config Evolution configuration
 * @return Evolution result with statistics
 */
EvolutionResult runOntogenesis(const OntogenesisConfig& config);

/**
 * @brief Tournament selection
 * @param population Current population
 * @param tournamentSize Number of individuals in tournament
 * @return Selected individual
 */
const OntogeneticKernel& tournamentSelect(
    const std::vector<OntogeneticKernel>& population,
    int tournamentSize = 3
);

/**
 * @brief Update development stage based on age and fitness
 * @param kernel Kernel to update
 * @param schedule Development schedule
 */
void updateDevelopmentStage(
    OntogeneticKernel& kernel,
    const DevelopmentSchedule& schedule
);

/**
 * @brief Calculate population diversity
 * @param population Population to analyze
 * @return Diversity score (0.0 to 1.0)
 */
double calculatePopulationDiversity(const std::vector<OntogeneticKernel>& population);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Generate unique ID for kernel
 * @return UUID string
 */
UUID generateKernelId();

/**
 * @brief Convert development stage to string
 * @param stage Development stage
 * @return String representation
 */
std::string developmentStageToString(DevelopmentStage stage);

/**
 * @brief Convert reproduction method to string
 * @param method Reproduction method
 * @return String representation
 */
std::string reproductionMethodToString(ReproductionMethod method);

} // namespace ontogenesis
} // namespace elizaos

#endif // ELIZAOS_ONTOGENESIS_HPP
