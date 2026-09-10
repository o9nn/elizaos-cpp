#include <gtest/gtest.h>

#include "elizaos/core.hpp"
#include "elizaos/evolutionary.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {
State state() {
    return State(AgentConfig{"evolution-agent", "Evolution Agent", "bio", "lore", "adaptive"});
}

std::shared_ptr<ProgramNode> constant(double value) {
    auto node = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "const");
    node->parameters.push_back(value);
    return node;
}

OptimizationPipeline::Stage stage(const std::string& name, double score,
                                  std::size_t population = 4,
                                  std::size_t generations = 1) {
    OptimizationPipeline::Stage value(name, [score](const Individual&, const State&) {
        return FitnessResult(score);
    });
    value.config.populationSize = population;
    value.config.maxGenerations = generations;
    value.config.mutationRate = 0.0;
    value.config.crossoverRate = 0.0;
    value.config.eliteRatio = 0.0;
    value.config.useDemeSplitting = false;
    value.config.useNoveltySearch = false;
    value.config.maxComplexity = 10000;
    value.config.diversityThreshold = 1.0;
    value.config.maxStagnationGenerations = 10;
    return value;
}

class Recorder final : public AdaptationHook {
public:
    void onPatternDiscovered(const PatternExtractor::Pattern& pattern, const State&) override {
        ++patternCount;
        patternNames.push_back(pattern.name);
    }
    void onFitnessImprovement(const Individual&, const FitnessResult& oldFitness,
                              const FitnessResult& newFitness, const State&) override {
        ++improvementCount;
        improvements.emplace_back(oldFitness.fitness, newFitness.fitness);
    }
    void onConvergence(const Population& population, const State&) override {
        ++convergenceCount;
        convergenceSizes.push_back(population.size());
    }
    void onAdaptationUpdate(const EvolutionaryOptimizer::Statistics& statistics,
                            EvolutionaryOptimizer::Config& config) override {
        ++adaptationCount;
        adaptationStats.push_back(statistics);
        config.mutationRate = 0.25;
    }

    int patternCount = 0;
    int improvementCount = 0;
    int convergenceCount = 0;
    int adaptationCount = 0;
    std::vector<std::string> patternNames;
    std::vector<std::pair<double, double>> improvements;
    std::vector<std::size_t> convergenceSizes;
    std::vector<EvolutionaryOptimizer::Statistics> adaptationStats;
};

class Thrower final : public AdaptationHook {
public:
    void onPatternDiscovered(const PatternExtractor::Pattern&, const State&) override {
        throw std::runtime_error("pattern");
    }
    void onFitnessImprovement(const Individual&, const FitnessResult&,
                              const FitnessResult&, const State&) override {
        throw std::runtime_error("improvement");
    }
    void onConvergence(const Population&, const State&) override {
        throw std::runtime_error("convergence");
    }
    void onAdaptationUpdate(const EvolutionaryOptimizer::Statistics&,
                            EvolutionaryOptimizer::Config& config) override {
        config.populationSize = 0;
        throw std::runtime_error("adaptation");
    }
};

template <typename Predicate>
bool waitUntil(Predicate predicate, std::chrono::milliseconds timeout = 1s) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) return true;
        std::this_thread::sleep_for(1ms);
    }
    return predicate();
}
} // namespace

TEST(EvolutionaryAudit, PublicValueAndPopulationMethodsAreSafe) {
    auto node = constant(3.5);
    auto clone = node->clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_NE(clone.get(), node.get());
    EXPECT_DOUBLE_EQ(clone->evaluate({}), 3.5);
    EXPECT_EQ(clone->toString(), node->toString());

    Individual individual(node);
    individual.setFitness(FitnessResult(0.8, 1.0, 0.2));
    individual.incrementAge();
    Individual copy = individual;
    EXPECT_NE(copy.getProgram().get(), individual.getProgram().get());
    EXPECT_EQ(copy.getAge(), 1);
    EXPECT_DOUBLE_EQ(copy.getFitness().fitness, 0.8);
    EXPECT_NEAR(individual.similarity(copy), 1.0, 1e-12);
    EXPECT_DOUBLE_EQ(Individual::deserialize(individual.serialize()).getFitness().fitness, 0.8);

    Population population(3);
    population.addIndividual(individual);
    population.addIndividual(Individual(constant(7.0)));
    auto snapshot = population.getIndividuals();
    snapshot.clear();
    EXPECT_EQ(population.size(), 2u);
    EXPECT_THROW(population.getIndividual(9), std::out_of_range);
    EXPECT_EQ(population.tournamentSelection(2, 3).size(), 3u);
    EXPECT_EQ(population.rouletteWheelSelection(3).size(), 3u);
    EXPECT_EQ(population.eliteSelection(1).size(), 1u);
}

TEST(EvolutionaryAudit, RejectsMalformedConfigFitnessAndRates) {
    EvolutionaryOptimizer::Config config;
    config.populationSize = 0;
    EXPECT_THROW(static_cast<void>(EvolutionaryOptimizer{config}),
                 std::invalid_argument);
    config.populationSize = 4;
    config.maxGenerations = 0;
    EXPECT_THROW(static_cast<void>(EvolutionaryOptimizer{config}),
                 std::invalid_argument);
    config.maxGenerations = 1;
    config.mutationRate = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(static_cast<void>(EvolutionaryOptimizer{config}),
                 std::invalid_argument);
    config.mutationRate = 0.1;
    config.crossoverRate = 2.0;
    EXPECT_THROW(static_cast<void>(EvolutionaryOptimizer{config}),
                 std::invalid_argument);
    EXPECT_THROW(FitnessResult(std::numeric_limits<double>::infinity()), std::invalid_argument);
    EXPECT_THROW(Individual(constant(1.0)).mutate(-0.1), std::invalid_argument);
    EXPECT_THROW(Population(0), std::invalid_argument);

    config.crossoverRate = 0.0;
    EvolutionaryOptimizer optimizer(config);
    EXPECT_THROW(optimizer.optimize([](const Individual&, const State&) {
                     FitnessResult value;
                     value.fitness = std::numeric_limits<double>::quiet_NaN();
                     return value;
                 }, state()), std::invalid_argument);
}

TEST(EvolutionaryAudit, OddPopulationAndRunOrderAreDeterministic) {
    EvolutionaryOptimizer::Config config;
    config.populationSize = 5;
    config.maxGenerations = 3;
    config.mutationRate = 0.0;
    config.crossoverRate = 0.0;
    config.eliteRatio = 0.0;
    config.useDemeSplitting = false;
    config.useNoveltySearch = false;
    config.maxComplexity = 10000;
    config.diversityThreshold = 0.0;
    const auto fitness = [](const Individual& individual, const State&) {
        return FitnessResult(individual.getProgram()->evaluate({}));
    };
    EvolutionaryOptimizer first(config);
    EvolutionaryOptimizer second(config);
    const auto firstBest = first.optimize(fitness, state());
    const auto secondBest = second.optimize(fitness, state());
    EXPECT_EQ(firstBest.getProgram()->toString(), secondBest.getProgram()->toString());
    EXPECT_EQ(first.getPopulation()->size(), 5u);
}

TEST(EvolutionaryAudit, OptimizerRejectsConcurrentRunAndStopWakesPause) {
    EvolutionaryOptimizer::Config config;
    config.populationSize = 3;
    config.maxGenerations = 100;
    config.useDemeSplitting = false;
    config.useNoveltySearch = false;
    config.maxComplexity = 10000;
    config.diversityThreshold = 0.0;
    EvolutionaryOptimizer optimizer(config);
    std::mutex mutex;
    std::condition_variable gate;
    bool release = false;
    std::atomic<int> calls{0};
    auto future = optimizer.optimizeAsync([&](const Individual&, const State&) {
        ++calls;
        std::unique_lock<std::mutex> lock(mutex);
        gate.wait(lock, [&] { return release; });
        return FitnessResult(0.5);
    }, state());
    ASSERT_TRUE(waitUntil([&] { return calls.load() > 0; }));
    EXPECT_THROW(optimizer.optimize([](const Individual&, const State&) {
                     return FitnessResult(0.5);
                 }, state()), std::logic_error);
    optimizer.pause();
    {
        std::lock_guard<std::mutex> lock(mutex);
        release = true;
    }
    gate.notify_all();
    optimizer.stop();
    EXPECT_EQ(future.wait_for(1s), std::future_status::ready);
    EXPECT_NO_THROW(future.get());
}

TEST(EvolutionaryAudit, AsyncOptimizerAndPipelineAreJoinedOnDestruction) {
    EvolutionaryOptimizer::Config config;
    config.populationSize = 3;
    config.maxGenerations = 100;
    config.useDemeSplitting = false;
    config.useNoveltySearch = false;
    config.maxComplexity = 10000;
    config.diversityThreshold = 0.0;
    std::future<Individual> optimizerFuture;
    {
        EvolutionaryOptimizer optimizer(config);
        optimizerFuture = optimizer.optimizeAsync([](const Individual&, const State&) {
            std::this_thread::sleep_for(1ms);
            return FitnessResult(0.2);
        }, state());
    }
    EXPECT_EQ(optimizerFuture.wait_for(1s), std::future_status::ready);
    try { static_cast<void>(optimizerFuture.get()); } catch (const std::logic_error&) {}

    std::future<Individual> pipelineFuture;
    {
        OptimizationPipeline pipeline;
        auto slow = stage("slow", 0.3, 3, 100);
        slow.config.diversityThreshold = 0.0;
        slow.fitnessFunc = [](const Individual&, const State&) {
            std::this_thread::sleep_for(1ms);
            return FitnessResult(0.3);
        };
        pipeline.addStage(slow);
        pipelineFuture = pipeline.runPipelineAsync(state());
    }
    EXPECT_EQ(pipelineFuture.wait_for(1s), std::future_status::ready);
    try { static_cast<void>(pipelineFuture.get()); } catch (const std::logic_error&) {}
}

TEST(EvolutionaryAudit, PipelineRejectsEmptyDuplicateAndUnknownOrder) {
    OptimizationPipeline pipeline;
    EXPECT_THROW(pipeline.runPipeline(state()), std::logic_error);
    auto first = stage("first", 0.5);
    pipeline.addStage(first);
    EXPECT_THROW(pipeline.addStage(first), std::invalid_argument);
    pipeline.addStage(stage("second", 0.6));
    EXPECT_THROW(pipeline.setStageOrder({"first", "first"}), std::invalid_argument);
    EXPECT_THROW(pipeline.setStageOrder({"first", "missing"}), std::invalid_argument);
    EXPECT_THROW(pipeline.setStageOrder({"first"}), std::invalid_argument);
    EXPECT_THROW(pipeline.removeStage("missing"), std::invalid_argument);
    EXPECT_THROW(pipeline.addGlobalHook(nullptr), std::invalid_argument);
}

TEST(EvolutionaryAudit, HooksReceiveOnlyExactEvidenceAndAreDeduplicated) {
    OptimizationPipeline pipeline;
    auto recorder = std::make_shared<Recorder>();
    auto first = stage("first", 0.6);
    auto equal = stage("equal", 0.6);
    auto better = stage("better", 0.9);
    first.hooks.push_back(recorder);
    equal.hooks.push_back(recorder);
    better.hooks.push_back(recorder);
    pipeline.addStage(first);
    pipeline.addStage(equal);
    pipeline.addStage(better);
    pipeline.addGlobalHook(recorder);
    pipeline.addGlobalHook(recorder);
    pipeline.runPipeline(state());

    const auto result = pipeline.getLastResult();
    ASSERT_EQ(result.stageStatistics.size(), 3u);
    EXPECT_EQ(recorder->adaptationCount, 3);
    EXPECT_EQ(recorder->improvementCount, 1);
    ASSERT_EQ(recorder->improvements.size(), 1u);
    EXPECT_DOUBLE_EQ(recorder->improvements.front().first, 0.6);
    EXPECT_DOUBLE_EQ(recorder->improvements.front().second, 0.9);
    EXPECT_FALSE(result.extractedPatterns.empty());
    EXPECT_EQ(recorder->patternCount, static_cast<int>(result.extractedPatterns.size()));
    EXPECT_EQ(recorder->convergenceCount, 3);
    for (const auto size : recorder->convergenceSizes) EXPECT_EQ(size, 4u);
}

TEST(EvolutionaryAudit, HookExceptionsAndReentrancyAreIsolated) {
    OptimizationPipeline pipeline;
    auto recorder = std::make_shared<Recorder>();
    auto thrower = std::make_shared<Thrower>();
    class Reentrant final : public AdaptationHook {
    public:
        explicit Reentrant(OptimizationPipeline& owner) : owner_(owner) {}
        void onPatternDiscovered(const PatternExtractor::Pattern&, const State&) override {
            static_cast<void>(owner_.getLastResult());
        }
        void onFitnessImprovement(const Individual&, const FitnessResult&,
                                  const FitnessResult&, const State&) override {
            static_cast<void>(owner_.isRunning());
        }
        void onConvergence(const Population&, const State&) override {
            static_cast<void>(owner_.getLastResult());
        }
        void onAdaptationUpdate(const EvolutionaryOptimizer::Statistics&,
                                EvolutionaryOptimizer::Config&) override {
            static_cast<void>(owner_.isRunning());
        }
    private:
        OptimizationPipeline& owner_;
    };
    auto reentrant = std::make_shared<Reentrant>(pipeline);
    auto first = stage("first", 0.6);
    auto second = stage("second", 0.9);
    first.hooks = {thrower, reentrant, recorder};
    second.hooks = {thrower, reentrant, recorder};
    pipeline.addStage(first);
    pipeline.addStage(second);
    EXPECT_NO_THROW(pipeline.runPipeline(state()));
    EXPECT_EQ(recorder->adaptationCount, 2);
    EXPECT_EQ(recorder->improvementCount, 1);
    pipeline.removeStage("first");
    EXPECT_NO_THROW(pipeline.runPipeline(state()));
}

TEST(EvolutionaryAudit, PipelineOrderIsDeterministicAndConcurrentRunRejected) {
    OptimizationPipeline pipeline;
    std::mutex mutex;
    std::condition_variable gate;
    bool release = false;
    std::vector<std::string> order;
    const auto blocking = [&](const std::string& name) {
        auto value = stage(name, 0.5);
        value.fitnessFunc = [&, name](const Individual&, const State&) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                order.push_back(name);
            }
            if (name == "third") {
                std::unique_lock<std::mutex> lock(mutex);
                gate.wait(lock, [&] { return release; });
            }
            return FitnessResult(0.5);
        };
        return value;
    };
    pipeline.addStage(blocking("first"));
    pipeline.addStage(blocking("second"));
    pipeline.addStage(blocking("third"));
    pipeline.setStageOrder({"third", "first", "second"});
    auto future = pipeline.runPipelineAsync(state());
    ASSERT_TRUE(waitUntil([&] {
        std::lock_guard<std::mutex> lock(mutex);
        return !order.empty();
    }));
    EXPECT_THROW(pipeline.runPipeline(state()), std::logic_error);
    {
        std::lock_guard<std::mutex> lock(mutex);
        release = true;
    }
    gate.notify_all();
    ASSERT_EQ(future.wait_for(1s), std::future_status::ready);
    EXPECT_NO_THROW(future.get());
    EXPECT_EQ(order.front(), "third");
    EXPECT_LT(std::find(order.begin(), order.end(), "first"),
              std::find(order.begin(), order.end(), "second"));
}
