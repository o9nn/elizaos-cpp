#pragma once

#include "elizaos/core.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

class EvolutionaryProgram;
class EvolutionaryOptimizer;
class PatternExtractor;
class AdaptationHook;

/**
 * Evolutionary search framework inspired by MOSES (Meta-Optimizing Semantic
 * Evolutionary Search).
 */
struct ProgramNode {
    enum class Type {
        CONSTANT,
        VARIABLE,
        FUNCTION,
        CONDITIONAL
    };

    Type type;
    std::string name;
    std::vector<double> parameters;
    std::vector<std::shared_ptr<ProgramNode>> children;

    ProgramNode(Type t, const std::string& n) : type(t), name(n) {}

    std::shared_ptr<ProgramNode> clone() const;
    double evaluate(const std::unordered_map<std::string, double>& context) const;
    std::string toString() const;
};

struct FitnessResult {
    double fitness;
    double complexity;
    double novelty;
    std::vector<double> behaviorSignature;
    std::string description;

    FitnessResult();
    FitnessResult(double f, double c = 0.0, double n = 0.0);

    bool isFinite() const noexcept;
    void validate() const;
    double getOverallScore() const;
};

class Individual {
public:
    explicit Individual(std::shared_ptr<ProgramNode> program);
    Individual(const Individual& other);
    Individual& operator=(const Individual& other);

    std::shared_ptr<ProgramNode> getProgram() const { return program_; }

    FitnessResult getFitness() const { return fitness_; }
    void setFitness(const FitnessResult& fitness);

    int getAge() const { return age_; }
    void incrementAge() { ++age_; }

    static Individual crossover(const Individual& parent1, const Individual& parent2);
    Individual mutate(double mutationRate = 0.1) const;
    double similarity(const Individual& other) const;

    std::string serialize() const;
    static Individual deserialize(const std::string& data);

private:
    std::shared_ptr<ProgramNode> program_;
    FitnessResult fitness_;
    int age_ = 0;
    UUID id_;
};

/**
 * A bounded, synchronized population. Accessors return snapshots rather than
 * references whose lifetime would extend beyond the population lock.
 */
class Population {
public:
    explicit Population(std::size_t maxSize = 100);
    Population(const Population& other);
    Population& operator=(const Population& other);

    void addIndividual(const Individual& individual);
    void removeIndividual(std::size_t index);
    void setIndividual(std::size_t index, const Individual& individual);
    void replaceIndividuals(const std::vector<Individual>& individuals);
    std::size_t size() const;
    std::size_t maxSize() const;
    bool empty() const;

    Individual getIndividual(std::size_t index) const;
    std::vector<Individual> getIndividuals() const;

    FitnessResult getBestFitness() const;
    FitnessResult getAverageFitness() const;
    double getDiversity() const;

    std::vector<Individual> tournamentSelection(std::size_t tournamentSize,
                                                std::size_t numSelected) const;
    std::vector<Individual> rouletteWheelSelection(std::size_t numSelected) const;
    std::vector<Individual> eliteSelection(std::size_t numElite) const;

    void sort();
    void ageIndividuals();
    void clear();

private:
    std::vector<Individual> individuals_;
    std::size_t maxSize_;
    mutable std::mutex populationMutex_;
};

using FitnessFunction = std::function<FitnessResult(const Individual&, const State&)>;

class EvolutionaryOptimizer {
public:
    struct Config {
        std::size_t populationSize = 100;
        std::size_t maxGenerations = 1000;
        double mutationRate = 0.1;
        double crossoverRate = 0.8;
        double eliteRatio = 0.1;
        std::size_t tournamentSize = 3;
        double diversityThreshold = 0.1;
        bool useDemeSplitting = true;
        bool useNoveltySearch = true;
        int maxComplexity = 50;
        double stagnationThreshold = 0.001;
        int maxStagnationGenerations = 50;
    };

    struct Statistics {
        std::size_t generation = 0;
        FitnessResult bestFitness;
        FitnessResult averageFitness;
        double diversity = 0.0;
        double convergenceRate = 0.0;
        int stagnationCount = 0;
        std::chrono::milliseconds generationTime{0};
    };

    explicit EvolutionaryOptimizer(const Config& config);
    ~EvolutionaryOptimizer();

    EvolutionaryOptimizer(const EvolutionaryOptimizer&) = delete;
    EvolutionaryOptimizer& operator=(const EvolutionaryOptimizer&) = delete;

    Individual optimize(const FitnessFunction& fitnessFunc, const State& state);
    Individual optimize(const FitnessFunction& fitnessFunc, const State& state,
                        const std::vector<Individual>& initialPopulation);
    std::future<Individual> optimizeAsync(const FitnessFunction& fitnessFunc,
                                          const State& state);

    void setPopulation(const Population& population);
    std::shared_ptr<Population> getPopulation() const;

    void setConfig(const Config& config);
    Config getConfig() const;
    static void validateConfig(const Config& config);

    void pause();
    void resume();
    void stop();
    bool isRunning() const;

    Statistics getStatistics() const;
    std::vector<Statistics> getHistory() const;

private:
    struct SharedState;
    std::shared_ptr<SharedState> shared_;
    mutable std::mutex asyncMutex_;
    std::vector<std::thread> asyncThreads_;

    static Individual optimizeImpl(const std::shared_ptr<SharedState>& shared,
                                   const FitnessFunction& fitnessFunc,
                                   const State& state,
                                   const std::vector<Individual>* initialPopulation);
    static std::shared_ptr<ProgramNode> generateRandomProgram(SharedState& shared,
                                                              int maxDepth = 5);
    void joinAsyncTasks() noexcept;
};

class PatternExtractor {
public:
    struct Pattern {
        std::string name;
        std::shared_ptr<ProgramNode> structure;
        double frequency;
        double effectiveness;
        std::vector<std::string> contexts;

        Pattern(const std::string& n, std::shared_ptr<ProgramNode> s)
            : name(n), structure(std::move(s)), frequency(0.0), effectiveness(0.0) {}
    };

    PatternExtractor();

    std::vector<Pattern> extractPatterns(const std::vector<Individual>& individuals,
                                         double minFitness = 0.8) const;
    std::vector<Pattern> extractPatternsFromHistory(
        const std::vector<EvolutionaryOptimizer::Statistics>& history) const;
    std::vector<Pattern> getCommonPatterns(const std::vector<Pattern>& patterns,
                                           double minFrequency = 0.1) const;
    double patternSimilarity(const Pattern& p1, const Pattern& p2) const;
    void savePatterns(const std::vector<Pattern>& patterns,
                      const std::string& filename) const;
    std::vector<Pattern> loadPatterns(const std::string& filename) const;

private:
    std::vector<Pattern> extractSubtreePatterns(
        const std::vector<Individual>& individuals) const;
    std::vector<Pattern> extractBehaviorPatterns(
        const std::vector<Individual>& individuals) const;
    std::vector<Pattern> extractStructuralPatterns(
        const std::vector<Individual>& individuals) const;
    std::vector<std::shared_ptr<ProgramNode>> getAllSubtrees(
        const std::shared_ptr<ProgramNode>& program) const;
};

class AdaptationHook {
public:
    virtual ~AdaptationHook() = default;

    virtual void onPatternDiscovered(const PatternExtractor::Pattern& pattern,
                                     const State& state) = 0;
    virtual void onFitnessImprovement(const Individual& individual,
                                      const FitnessResult& oldFitness,
                                      const FitnessResult& newFitness,
                                      const State& state) = 0;
    virtual void onConvergence(const Population& population,
                               const State& state) = 0;
    virtual void onAdaptationUpdate(const EvolutionaryOptimizer::Statistics& stats,
                                    EvolutionaryOptimizer::Config& config) = 0;
};

class OptimizationPipeline {
public:
    struct Stage {
        std::string name;
        FitnessFunction fitnessFunc;
        EvolutionaryOptimizer::Config config;
        std::vector<std::shared_ptr<AdaptationHook>> hooks;

        Stage(const std::string& n, const FitnessFunction& f);
    };

    struct PipelineResult {
        Individual bestIndividual{nullptr};
        std::vector<Individual> stageResults;
        std::vector<EvolutionaryOptimizer::Statistics> stageStatistics;
        std::vector<PatternExtractor::Pattern> extractedPatterns;
        std::chrono::milliseconds totalTime{0};
    };

    OptimizationPipeline();
    ~OptimizationPipeline();

    OptimizationPipeline(const OptimizationPipeline&) = delete;
    OptimizationPipeline& operator=(const OptimizationPipeline&) = delete;

    void addStage(const Stage& stage);
    void removeStage(const std::string& name);
    void setStageOrder(const std::vector<std::string>& order);

    Individual runPipeline(const State& state);
    std::future<Individual> runPipelineAsync(const State& state);

    void pause();
    void resume();
    void stop();
    bool isRunning() const;

    PipelineResult getLastResult() const;

    void addGlobalHook(std::shared_ptr<AdaptationHook> hook);
    void removeGlobalHook(const std::shared_ptr<AdaptationHook>& hook);

private:
    struct SharedState;
    std::shared_ptr<SharedState> shared_;
    mutable std::mutex asyncMutex_;
    std::vector<std::thread> asyncThreads_;

    static Individual runPipelineImpl(const std::shared_ptr<SharedState>& shared,
                                      const State& state);
    void joinAsyncTasks() noexcept;
};

} // namespace elizaos
