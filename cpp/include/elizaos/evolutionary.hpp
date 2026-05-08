#pragma once

/**
 * ElizaOS C++ - Evolutionary Module
 *
 * Genetic programming: ProgramNode, Individual, Population,
 * and EvolutionaryOptimizer.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// ProgramNode – tree-structured genetic program
// ============================================================================

class ProgramNode {
public:
    enum class Type { CONSTANT, VARIABLE, FUNCTION };

    Type        type = Type::CONSTANT;
    std::string name;
    std::vector<double>                        parameters;
    std::vector<std::shared_ptr<ProgramNode>>  children;

    ProgramNode() = default;
    ProgramNode(Type t, const std::string& n) : type(t), name(n) {}

    double evaluate(const std::unordered_map<std::string, double>& context) const;
    std::shared_ptr<ProgramNode> clone() const;
    std::string toString() const;
};

// ============================================================================
// FitnessResult
// ============================================================================

struct FitnessResult {
    double      fitness   = 0.0;
    double      diversity = 0.0;
    std::string details;

    bool operator<(const FitnessResult& o) const { return fitness < o.fitness; }
    bool operator>(const FitnessResult& o) const { return fitness > o.fitness; }
};

using FitnessFunction = std::function<FitnessResult(const ProgramNode&, const State&)>;

// ============================================================================
// Individual – single member of the population
// ============================================================================

class Individual {
public:
    std::shared_ptr<ProgramNode> program_;
    FitnessResult fitness_;
    int           age_        = 0;
    int           generation_ = 0;

    Individual() = default;
    explicit Individual(std::shared_ptr<ProgramNode> program);
    Individual(const Individual& other);
    Individual& operator=(const Individual& other);

    static Individual crossover(const Individual& parent1, const Individual& parent2);
    Individual        mutate(double mutationRate) const;

    double     similarity(const Individual& other) const;
    std::string serialize() const;
    static Individual deserialize(const std::string& data);
};

// ============================================================================
// Population
// ============================================================================

class Population {
public:
    explicit Population(size_t maxSize = 100);

    void addIndividual(const Individual& individual);
    void removeIndividual(size_t index);
    const Individual& getIndividual(size_t index) const;
    Individual&       getIndividual(size_t index);

    size_t      size()  const { return individuals_.size(); }
    size_t      maxSize() const { return maxSize_; }
    bool        empty() const { return individuals_.empty(); }

    FitnessResult              getBestFitness() const;
    FitnessResult              getAverageFitness() const;
    double                     getDiversity() const;

    std::vector<Individual>    tournamentSelection(size_t tournamentSize, size_t numSelected) const;
    std::vector<Individual>    eliteSelection(size_t numElite) const;
    std::vector<Individual>    rouletteWheelSelection(size_t numSelected) const;

    void sort();
    void ageIndividuals();

private:
    std::vector<Individual> individuals_;
    size_t                  maxSize_;
};

// ============================================================================
// EvolutionaryOptimizer
// ============================================================================

class EvolutionaryOptimizer {
public:
    struct Config {
        size_t populationSize  = 50;
        double mutationRate    = 0.1;
        double crossoverRate   = 0.7;
        int    maxGenerations  = 100;
        size_t eliteCount      = 5;
        int    tournamentSize  = 3;
        std::string selectionStrategy = "tournament";
    };

    explicit EvolutionaryOptimizer(const Config& config = Config{});

    Individual optimize(const FitnessFunction& fitnessFunc, const State& state);
    Individual evolve(const FitnessFunction& fitnessFunc, const State& state);

    void                  setConfig(const Config& config) { config_ = config; }
    const Config&         getConfig() const { return config_; }
    const Population&     getPopulation() const { return population_; }
    int                   getCurrentGeneration() const { return currentGeneration_; }

    std::vector<FitnessResult> getFitnessHistory() const { return fitnessHistory_; }

private:
    void   evaluateFitness(const FitnessFunction& fitnessFunc, const State& state);
    void   evolveGeneration(const FitnessFunction& fitnessFunc, const State& state);
    void   selectParents(std::vector<Individual>& parents);

    Config      config_;
    Population  population_;
    int         currentGeneration_ = 0;
    std::vector<FitnessResult> fitnessHistory_;
    mutable std::mutex optimizerMutex_;
};

} // namespace elizaos
