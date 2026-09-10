#include "elizaos/evolutionary.hpp"
#include "thread_reaper.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace elizaos {
namespace {

template <typename Function>
class ScopeExit {
public:
    explicit ScopeExit(Function function) : function_(std::move(function)) {}
    ~ScopeExit() { function_(); }
    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;

private:
    Function function_;
};

template <typename Function>
ScopeExit<Function> makeScopeExit(Function function) {
    return ScopeExit<Function>(std::move(function));
}

std::vector<std::shared_ptr<AdaptationHook>> uniqueHooks(
    const std::vector<std::shared_ptr<AdaptationHook>>& first,
    const std::vector<std::shared_ptr<AdaptationHook>>& second) {
    std::vector<std::shared_ptr<AdaptationHook>> hooks;
    std::unordered_set<const AdaptationHook*> seen;
    const auto append = [&](const std::vector<std::shared_ptr<AdaptationHook>>& source) {
        for (const auto& hook : source) {
            if (hook && seen.insert(hook.get()).second) hooks.push_back(hook);
        }
    };
    append(first);
    append(second);
    return hooks;
}

class ProgramParser {
public:
    explicit ProgramParser(const std::string& input) : input_(input) {}

    std::shared_ptr<ProgramNode> parse() {
        auto result = parseNode();
        skipWhitespace();
        if (!result || position_ != input_.size()) {
            throw std::invalid_argument("invalid pattern structure");
        }
        return result;
    }

private:
    void skipWhitespace() {
        while (position_ < input_.size() &&
               std::isspace(static_cast<unsigned char>(input_[position_])) != 0) {
            ++position_;
        }
    }

    std::string parseToken() {
        skipWhitespace();
        const std::size_t start = position_;
        while (position_ < input_.size() && input_[position_] != '(' &&
               input_[position_] != ')' &&
               std::isspace(static_cast<unsigned char>(input_[position_])) == 0) {
            ++position_;
        }
        if (start == position_) throw std::invalid_argument("missing pattern token");
        return input_.substr(start, position_ - start);
    }

    std::shared_ptr<ProgramNode> parseNode() {
        skipWhitespace();
        if (position_ >= input_.size()) throw std::invalid_argument("missing pattern node");
        if (input_[position_] == '(') {
            ++position_;
            const std::string name = parseToken();
            const ProgramNode::Type type =
                name == "if" || name == "gt" || name == "lt"
                    ? ProgramNode::Type::CONDITIONAL
                    : ProgramNode::Type::FUNCTION;
            auto node = std::make_shared<ProgramNode>(type, name);
            for (;;) {
                skipWhitespace();
                if (position_ >= input_.size()) {
                    throw std::invalid_argument("unterminated pattern node");
                }
                if (input_[position_] == ')') {
                    ++position_;
                    return node;
                }
                node->children.push_back(parseNode());
            }
        }

        const std::string token = parseToken();
        std::size_t consumed = 0;
        try {
            const double value = std::stod(token, &consumed);
            if (consumed == token.size() && std::isfinite(value)) {
                auto node = std::make_shared<ProgramNode>(ProgramNode::Type::CONSTANT, "const");
                node->parameters.push_back(value);
                return node;
            }
        } catch (const std::exception&) {
            // A nonnumeric token is a variable.
        }
        return std::make_shared<ProgramNode>(ProgramNode::Type::VARIABLE, token);
    }

    const std::string& input_;
    std::size_t position_ = 0;
};

} // namespace

PatternExtractor::PatternExtractor() = default;

std::vector<PatternExtractor::Pattern> PatternExtractor::extractPatterns(
    const std::vector<Individual>& individuals, double minFitness) const {
    if (!std::isfinite(minFitness)) {
        throw std::invalid_argument("minimum fitness must be finite");
    }

    std::vector<Individual> successful;
    successful.reserve(individuals.size());
    for (const auto& individual : individuals) {
        const FitnessResult fitness = individual.getFitness();
        fitness.validate();
        if (individual.getProgram() && fitness.fitness >= minFitness) {
            successful.push_back(individual);
        }
    }
    if (successful.empty()) return {};

    std::vector<Pattern> patterns = extractSubtreePatterns(successful);
    auto behavior = extractBehaviorPatterns(successful);
    auto structural = extractStructuralPatterns(successful);
    patterns.insert(patterns.end(), behavior.begin(), behavior.end());
    patterns.insert(patterns.end(), structural.begin(), structural.end());
    std::stable_sort(patterns.begin(), patterns.end(), [](const Pattern& lhs,
                                                          const Pattern& rhs) {
        if (lhs.effectiveness != rhs.effectiveness) {
            return lhs.effectiveness > rhs.effectiveness;
        }
        if (lhs.frequency != rhs.frequency) return lhs.frequency > rhs.frequency;
        return lhs.name < rhs.name;
    });
    return patterns;
}

std::vector<PatternExtractor::Pattern> PatternExtractor::extractPatternsFromHistory(
    const std::vector<EvolutionaryOptimizer::Statistics>& history) const {
    for (const auto& statistics : history) {
        statistics.bestFitness.validate();
        statistics.averageFitness.validate();
        if (!std::isfinite(statistics.diversity) ||
            !std::isfinite(statistics.convergenceRate)) {
            throw std::invalid_argument("history statistics must be finite");
        }
    }

    std::vector<Pattern> patterns;
    if (history.size() < 3) return patterns;

    bool hasPlateauEvidence = true;
    constexpr double convergenceThreshold = 0.01;
    for (std::size_t index = history.size() - 3; index + 1 < history.size(); ++index) {
        const double improvement = history[index + 1].bestFitness.getOverallScore() -
                                   history[index].bestFitness.getOverallScore();
        if (std::abs(improvement) > convergenceThreshold) {
            hasPlateauEvidence = false;
            break;
        }
    }
    if (hasPlateauEvidence) {
        Pattern pattern("convergence",
                        std::make_shared<ProgramNode>(ProgramNode::Type::FUNCTION,
                                                      "convergence"));
        pattern.effectiveness = history.back().bestFitness.fitness;
        pattern.frequency = 1.0;
        pattern.contexts.push_back("fitness_plateau");
        patterns.push_back(pattern);
    }

    double averageDiversity = 0.0;
    for (const auto& statistics : history) averageDiversity += statistics.diversity;
    averageDiversity /= static_cast<double>(history.size());
    if (averageDiversity > 0.5) {
        Pattern pattern("high_diversity",
                        std::make_shared<ProgramNode>(ProgramNode::Type::FUNCTION,
                                                      "high_diversity"));
        pattern.effectiveness = averageDiversity;
        pattern.frequency = 1.0;
        pattern.contexts.push_back("exploration");
        patterns.push_back(pattern);
    }
    return patterns;
}

std::vector<PatternExtractor::Pattern> PatternExtractor::getCommonPatterns(
    const std::vector<Pattern>& patterns, double minFrequency) const {
    if (!std::isfinite(minFrequency) || minFrequency < 0.0 || minFrequency > 1.0) {
        throw std::invalid_argument("minimum frequency must be finite and in [0, 1]");
    }
    std::vector<Pattern> common;
    for (const auto& pattern : patterns) {
        if (!std::isfinite(pattern.frequency) || pattern.frequency < 0.0 ||
            pattern.frequency > 1.0 || !std::isfinite(pattern.effectiveness)) {
            throw std::invalid_argument("pattern statistics are invalid");
        }
        if (pattern.frequency >= minFrequency) common.push_back(pattern);
    }
    std::stable_sort(common.begin(), common.end(), [](const Pattern& lhs,
                                                       const Pattern& rhs) {
        if (lhs.frequency != rhs.frequency) return lhs.frequency > rhs.frequency;
        return lhs.name < rhs.name;
    });
    return common;
}

double PatternExtractor::patternSimilarity(const Pattern& first,
                                           const Pattern& second) const {
    if (!first.structure || !second.structure) return 0.0;
    const std::string firstString = first.structure->toString();
    const std::string secondString = second.structure->toString();
    if (firstString == secondString) return 1.0;

    std::set<std::string> firstTokens;
    std::set<std::string> secondTokens;
    std::istringstream firstStream(firstString);
    std::istringstream secondStream(secondString);
    std::string token;
    while (firstStream >> token) firstTokens.insert(token);
    while (secondStream >> token) secondTokens.insert(token);
    std::vector<std::string> intersection;
    std::vector<std::string> unionSet;
    std::set_intersection(firstTokens.begin(), firstTokens.end(),
                          secondTokens.begin(), secondTokens.end(),
                          std::back_inserter(intersection));
    std::set_union(firstTokens.begin(), firstTokens.end(),
                   secondTokens.begin(), secondTokens.end(),
                   std::back_inserter(unionSet));
    return unionSet.empty()
               ? 0.0
               : static_cast<double>(intersection.size()) /
                     static_cast<double>(unionSet.size());
}

void PatternExtractor::savePatterns(const std::vector<Pattern>& patterns,
                                    const std::string& filename) const {
    if (filename.empty()) throw std::invalid_argument("pattern filename must not be empty");
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("unable to open pattern file for writing");
    file << "# Extracted Patterns\n";
    file << "# Format: name|structure|frequency|effectiveness|contexts\n";
    for (const auto& pattern : patterns) {
        if (!pattern.structure || !std::isfinite(pattern.frequency) ||
            pattern.frequency < 0.0 || pattern.frequency > 1.0 ||
            !std::isfinite(pattern.effectiveness)) {
            throw std::invalid_argument("cannot save invalid pattern");
        }
        file << pattern.name << '|' << pattern.structure->toString() << '|'
             << pattern.frequency << '|' << pattern.effectiveness << '|';
        for (std::size_t index = 0; index < pattern.contexts.size(); ++index) {
            if (index != 0) file << ';';
            file << pattern.contexts[index];
        }
        file << '\n';
    }
    if (!file) throw std::runtime_error("failed while writing pattern file");
}

std::vector<PatternExtractor::Pattern> PatternExtractor::loadPatterns(
    const std::string& filename) const {
    if (filename.empty()) throw std::invalid_argument("pattern filename must not be empty");
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("unable to open pattern file for reading");

    std::vector<Pattern> patterns;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.front() == '#') continue;
        std::istringstream stream(line);
        std::string name;
        std::string structure;
        std::string frequency;
        std::string effectiveness;
        std::string contexts;
        if (!std::getline(stream, name, '|') || !std::getline(stream, structure, '|') ||
            !std::getline(stream, frequency, '|') ||
            !std::getline(stream, effectiveness, '|') ||
            !std::getline(stream, contexts)) {
            continue;
        }
        try {
            Pattern pattern(name, ProgramParser(structure).parse());
            std::size_t consumedFrequency = 0;
            std::size_t consumedEffectiveness = 0;
            pattern.frequency = std::stod(frequency, &consumedFrequency);
            pattern.effectiveness = std::stod(effectiveness, &consumedEffectiveness);
            if (consumedFrequency != frequency.size() ||
                consumedEffectiveness != effectiveness.size() ||
                !std::isfinite(pattern.frequency) || pattern.frequency < 0.0 ||
                pattern.frequency > 1.0 || !std::isfinite(pattern.effectiveness)) {
                continue;
            }
            std::istringstream contextStream(contexts);
            std::string context;
            while (std::getline(contextStream, context, ';')) {
                if (!context.empty()) pattern.contexts.push_back(context);
            }
            patterns.push_back(std::move(pattern));
        } catch (const std::exception&) {
            // Malformed records are ignored without discarding valid records.
        }
    }
    return patterns;
}

std::vector<PatternExtractor::Pattern> PatternExtractor::extractSubtreePatterns(
    const std::vector<Individual>& individuals) const {
    struct Evidence {
        int individualCount = 0;
        double totalFitness = 0.0;
        std::shared_ptr<ProgramNode> structure;
    };
    std::map<std::string, Evidence> evidence;
    for (const auto& individual : individuals) {
        std::map<std::string, std::shared_ptr<ProgramNode>> uniqueSubtrees;
        for (const auto& subtree : getAllSubtrees(individual.getProgram())) {
            uniqueSubtrees.emplace(subtree->toString(), subtree);
        }
        for (const auto& entry : uniqueSubtrees) {
            Evidence& item = evidence[entry.first];
            ++item.individualCount;
            item.totalFitness += individual.getFitness().fitness;
            if (!item.structure) item.structure = entry.second->clone();
        }
    }

    std::vector<Pattern> patterns;
    for (const auto& entry : evidence) {
        if (entry.second.individualCount < 2) continue;
        Pattern pattern("subtree_" + std::to_string(patterns.size()),
                        entry.second.structure);
        pattern.frequency = static_cast<double>(entry.second.individualCount) /
                            static_cast<double>(individuals.size());
        pattern.effectiveness = entry.second.totalFitness /
                                static_cast<double>(entry.second.individualCount);
        pattern.contexts.push_back("subtree");
        patterns.push_back(std::move(pattern));
    }
    return patterns;
}

std::vector<PatternExtractor::Pattern> PatternExtractor::extractBehaviorPatterns(
    const std::vector<Individual>& individuals) const {
    std::map<std::string, std::vector<Individual>> groups;
    for (const auto& individual : individuals) {
        const FitnessResult fitness = individual.getFitness();
        const int fitnessRange = static_cast<int>(std::floor(fitness.fitness * 10.0));
        const int complexityRange = static_cast<int>(std::floor(fitness.complexity / 10.0));
        const int noveltyRange = static_cast<int>(std::floor(fitness.novelty * 10.0));
        const std::string signature = std::to_string(fitnessRange) + '_' +
                                      std::to_string(complexityRange) + '_' +
                                      std::to_string(noveltyRange);
        groups[signature].push_back(individual);
    }

    std::vector<Pattern> patterns;
    for (const auto& entry : groups) {
        if (entry.second.size() < 2) continue;
        Pattern pattern("behavior_" + entry.first,
                        std::make_shared<ProgramNode>(ProgramNode::Type::FUNCTION,
                                                      "behavior_" + entry.first));
        pattern.frequency = static_cast<double>(entry.second.size()) /
                            static_cast<double>(individuals.size());
        for (const auto& individual : entry.second) {
            pattern.effectiveness += individual.getFitness().fitness;
        }
        pattern.effectiveness /= static_cast<double>(entry.second.size());
        pattern.contexts.push_back("behavior");
        patterns.push_back(std::move(pattern));
    }
    return patterns;
}

std::vector<PatternExtractor::Pattern> PatternExtractor::extractStructuralPatterns(
    const std::vector<Individual>& individuals) const {
    struct Evidence {
        int count = 0;
        double totalFitness = 0.0;
    };
    std::map<std::string, Evidence> evidence;
    for (const auto& individual : individuals) {
        std::map<std::string, int> nodeTypes;
        std::function<void(const std::shared_ptr<ProgramNode>&)> countNodes;
        countNodes = [&](const std::shared_ptr<ProgramNode>& node) {
            if (!node) return;
            ++nodeTypes[node->name];
            for (const auto& child : node->children) countNodes(child);
        };
        countNodes(individual.getProgram());
        std::string signature;
        for (const auto& nodeType : nodeTypes) {
            signature += nodeType.first + ':' + std::to_string(nodeType.second) + ';';
        }
        Evidence& item = evidence[signature];
        ++item.count;
        item.totalFitness += individual.getFitness().fitness;
    }

    std::vector<Pattern> patterns;
    for (const auto& entry : evidence) {
        if (entry.second.count < 2) continue;
        Pattern pattern("structure_" + std::to_string(patterns.size()),
                        std::make_shared<ProgramNode>(ProgramNode::Type::FUNCTION,
                                                      "structure"));
        pattern.frequency = static_cast<double>(entry.second.count) /
                            static_cast<double>(individuals.size());
        pattern.effectiveness = entry.second.totalFitness /
                                static_cast<double>(entry.second.count);
        pattern.contexts.push_back("structure");
        patterns.push_back(std::move(pattern));
    }
    return patterns;
}

std::vector<std::shared_ptr<ProgramNode>> PatternExtractor::getAllSubtrees(
    const std::shared_ptr<ProgramNode>& program) const {
    std::vector<std::shared_ptr<ProgramNode>> subtrees;
    if (!program) return subtrees;
    subtrees.push_back(program);
    for (const auto& child : program->children) {
        auto descendants = getAllSubtrees(child);
        subtrees.insert(subtrees.end(), descendants.begin(), descendants.end());
    }
    return subtrees;
}

OptimizationPipeline::Stage::Stage(const std::string& n, const FitnessFunction& f)
    : name(n), fitnessFunc(f) {}

struct OptimizationPipeline::SharedState {
    mutable std::mutex mutex;
    std::condition_variable controlCv;
    std::vector<Stage> stages;
    std::vector<std::string> stageOrder;
    std::vector<std::shared_ptr<AdaptationHook>> globalHooks;
    PipelineResult lastResult;
    bool running = false;
    bool paused = false;
    bool stopped = false;
    bool shuttingDown = false;
    std::shared_ptr<EvolutionaryOptimizer> activeOptimizer;
};

OptimizationPipeline::OptimizationPipeline() : shared_(std::make_shared<SharedState>()) {}

OptimizationPipeline::~OptimizationPipeline() {
    std::shared_ptr<EvolutionaryOptimizer> optimizer;
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        shared_->shuttingDown = true;
        shared_->stopped = true;
        shared_->paused = false;
        optimizer = shared_->activeOptimizer;
    }
    shared_->controlCv.notify_all();
    if (optimizer) optimizer->stop();
    joinAsyncTasks();
}

void OptimizationPipeline::addStage(const Stage& stage) {
    if (stage.name.empty()) throw std::invalid_argument("stage name must not be empty");
    if (!stage.fitnessFunc) throw std::invalid_argument("stage fitness function must be callable");
    EvolutionaryOptimizer::validateConfig(stage.config);
    for (const auto& hook : stage.hooks) {
        if (!hook) throw std::invalid_argument("stage hooks must not be null");
    }
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (shared_->running) throw std::logic_error("cannot add a stage while the pipeline is running");
    const auto duplicate = std::find_if(shared_->stages.begin(), shared_->stages.end(),
                                        [&](const Stage& existing) {
                                            return existing.name == stage.name;
                                        });
    if (duplicate != shared_->stages.end()) {
        throw std::invalid_argument("duplicate pipeline stage: " + stage.name);
    }
    shared_->stages.push_back(stage);
    shared_->stageOrder.push_back(stage.name);
}

void OptimizationPipeline::removeStage(const std::string& name) {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (shared_->running) throw std::logic_error("cannot remove a stage while the pipeline is running");
    const auto found = std::find_if(shared_->stages.begin(), shared_->stages.end(),
                                    [&](const Stage& stage) { return stage.name == name; });
    if (found == shared_->stages.end()) throw std::invalid_argument("unknown pipeline stage: " + name);
    shared_->stages.erase(found);
    shared_->stageOrder.erase(
        std::remove(shared_->stageOrder.begin(), shared_->stageOrder.end(), name),
        shared_->stageOrder.end());
}

void OptimizationPipeline::setStageOrder(const std::vector<std::string>& order) {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (shared_->running) throw std::logic_error("cannot reorder stages while the pipeline is running");
    if (order.size() != shared_->stages.size()) {
        throw std::invalid_argument("stage order must contain every stage exactly once");
    }
    std::unordered_set<std::string> seen;
    for (const std::string& name : order) {
        if (!seen.insert(name).second) {
            throw std::invalid_argument("duplicate stage in order: " + name);
        }
        const bool exists = std::any_of(shared_->stages.begin(), shared_->stages.end(),
                                        [&](const Stage& stage) { return stage.name == name; });
        if (!exists) throw std::invalid_argument("unknown stage in order: " + name);
    }
    shared_->stageOrder = order;
}

Individual OptimizationPipeline::runPipeline(const State& state) {
    return runPipelineImpl(shared_, state);
}

Individual OptimizationPipeline::runPipelineImpl(
    const std::shared_ptr<SharedState>& shared, const State& state) {
    std::vector<Stage> stages;
    std::vector<std::string> order;
    std::vector<std::shared_ptr<AdaptationHook>> globalHooks;
    {
        std::lock_guard<std::mutex> lock(shared->mutex);
        if (shared->shuttingDown) {
            throw std::logic_error("pipeline is shutting down");
        }
        if (shared->running) throw std::logic_error("pipeline is already running");
        if (shared->stages.empty()) throw std::logic_error("cannot run an empty pipeline");
        shared->running = true;
        shared->paused = false;
        shared->stopped = false;
        stages = shared->stages;
        order = shared->stageOrder;
        globalHooks = shared->globalHooks;
    }
    const auto finishRun = makeScopeExit([&] {
        std::lock_guard<std::mutex> lock(shared->mutex);
        shared->activeOptimizer.reset();
        shared->running = false;
        shared->paused = false;
        shared->controlCv.notify_all();
    });

    PipelineResult pipelineResult;
    Individual currentBest(nullptr);
    const auto start = std::chrono::steady_clock::now();

    for (const std::string& stageName : order) {
        {
            std::unique_lock<std::mutex> lock(shared->mutex);
            shared->controlCv.wait(lock, [&] { return !shared->paused || shared->stopped; });
            if (shared->stopped) break;
        }

        const auto stageIterator = std::find_if(stages.begin(), stages.end(),
                                                [&](const Stage& stage) {
                                                    return stage.name == stageName;
                                                });
        if (stageIterator == stages.end()) {
            throw std::logic_error("pipeline stage order became inconsistent");
        }
        Stage stage = *stageIterator;
        EvolutionaryOptimizer::validateConfig(stage.config);
        auto optimizer = std::make_shared<EvolutionaryOptimizer>(stage.config);
        if (currentBest.getProgram()) {
            std::vector<Individual> initialPopulation;
            initialPopulation.reserve(stage.config.populationSize);
            initialPopulation.push_back(currentBest);
            while (initialPopulation.size() < stage.config.populationSize) {
                initialPopulation.push_back(currentBest.mutate(stage.config.mutationRate));
            }
            Population population(stage.config.populationSize);
            population.replaceIndividuals(initialPopulation);
            optimizer->setPopulation(population);
        }
        {
            std::lock_guard<std::mutex> lock(shared->mutex);
            if (shared->stopped) break;
            shared->activeOptimizer = optimizer;
        }

        const bool hadOldResult = currentBest.getProgram() != nullptr;
        const FitnessResult oldFitness = currentBest.getFitness();
        Individual stageResult = optimizer->optimize(stage.fitnessFunc, state);
        const EvolutionaryOptimizer::Statistics statistics = optimizer->getStatistics();
        const auto population = optimizer->getPopulation();
        {
            std::lock_guard<std::mutex> lock(shared->mutex);
            shared->activeOptimizer.reset();
        }
        if (!stageResult.getProgram()) break;

        pipelineResult.stageResults.push_back(stageResult);
        pipelineResult.stageStatistics.push_back(statistics);
        const auto hooks = uniqueHooks(stage.hooks, globalHooks);

        EvolutionaryOptimizer::Config adaptedConfig = stage.config;
        for (const auto& hook : hooks) {
            const auto beforeHook = adaptedConfig;
            try {
                hook->onAdaptationUpdate(statistics, adaptedConfig);
                EvolutionaryOptimizer::validateConfig(adaptedConfig);
            } catch (...) {
                adaptedConfig = beforeHook;
            }
        }
        {
            std::lock_guard<std::mutex> lock(shared->mutex);
            const auto original = std::find_if(shared->stages.begin(), shared->stages.end(),
                                               [&](const Stage& candidate) {
                                                   return candidate.name == stage.name;
                                               });
            if (original != shared->stages.end()) original->config = adaptedConfig;
        }

        const FitnessResult newFitness = stageResult.getFitness();
        if (hadOldResult &&
            newFitness.getOverallScore() > oldFitness.getOverallScore()) {
            for (const auto& hook : hooks) {
                try {
                    hook->onFitnessImprovement(stageResult, oldFitness, newFitness, state);
                } catch (...) {
                    // One failing observer cannot suppress other evidence notifications.
                }
            }
        }

        const bool convergenceEvidence =
            population && !population->empty() &&
            (statistics.diversity <= stage.config.diversityThreshold ||
             statistics.stagnationCount >= stage.config.maxStagnationGenerations);
        if (convergenceEvidence) {
            for (const auto& hook : hooks) {
                try {
                    hook->onConvergence(*population, state);
                } catch (...) {
                    // Isolate observer failures.
                }
            }
        }
        currentBest = stageResult;
    }

    if (!pipelineResult.stageResults.empty()) {
        pipelineResult.extractedPatterns =
            PatternExtractor().extractPatterns(pipelineResult.stageResults, 0.5);
    }

    std::vector<std::shared_ptr<AdaptationHook>> patternHooks;
    std::unordered_set<const AdaptationHook*> seen;
    for (const std::string& stageName : order) {
        const auto stageIterator = std::find_if(stages.begin(), stages.end(),
                                                [&](const Stage& stage) {
                                                    return stage.name == stageName;
                                                });
        if (stageIterator == stages.end()) continue;
        for (const auto& hook : stageIterator->hooks) {
            if (hook && seen.insert(hook.get()).second) patternHooks.push_back(hook);
        }
    }
    for (const auto& hook : globalHooks) {
        if (hook && seen.insert(hook.get()).second) patternHooks.push_back(hook);
    }
    for (const auto& pattern : pipelineResult.extractedPatterns) {
        for (const auto& hook : patternHooks) {
            try {
                hook->onPatternDiscovered(pattern, state);
            } catch (...) {
                // Isolate observer failures.
            }
        }
    }

    pipelineResult.totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);
    pipelineResult.bestIndividual = currentBest;
    {
        std::lock_guard<std::mutex> lock(shared->mutex);
        shared->lastResult = pipelineResult;
    }
    return currentBest;
}

std::future<Individual> OptimizationPipeline::runPipelineAsync(const State& state) {
    auto promise = std::make_shared<std::promise<Individual>>();
    std::future<Individual> future = promise->get_future();
    const auto shared = shared_;
    const State stateCopy = state;
    std::thread worker([shared, stateCopy, promise] {
        try {
            promise->set_value(runPipelineImpl(shared, stateCopy));
        } catch (...) {
            promise->set_exception(std::current_exception());
        }
    });
    {
        std::lock_guard<std::mutex> lock(asyncMutex_);
        asyncThreads_.push_back(std::move(worker));
    }
    return future;
}

void OptimizationPipeline::pause() {
    std::shared_ptr<EvolutionaryOptimizer> optimizer;
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        if (shared_->running) shared_->paused = true;
        optimizer = shared_->activeOptimizer;
    }
    if (optimizer) optimizer->pause();
}

void OptimizationPipeline::resume() {
    std::shared_ptr<EvolutionaryOptimizer> optimizer;
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        shared_->paused = false;
        optimizer = shared_->activeOptimizer;
    }
    shared_->controlCv.notify_all();
    if (optimizer) optimizer->resume();
}

void OptimizationPipeline::stop() {
    std::shared_ptr<EvolutionaryOptimizer> optimizer;
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        shared_->stopped = true;
        shared_->paused = false;
        optimizer = shared_->activeOptimizer;
    }
    shared_->controlCv.notify_all();
    if (optimizer) optimizer->stop();
}

bool OptimizationPipeline::isRunning() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return shared_->running;
}

OptimizationPipeline::PipelineResult OptimizationPipeline::getLastResult() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return shared_->lastResult;
}

void OptimizationPipeline::addGlobalHook(std::shared_ptr<AdaptationHook> hook) {
    if (!hook) throw std::invalid_argument("global hook must not be null");
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (std::find(shared_->globalHooks.begin(), shared_->globalHooks.end(), hook) ==
        shared_->globalHooks.end()) {
        shared_->globalHooks.push_back(std::move(hook));
    }
}

void OptimizationPipeline::removeGlobalHook(
    const std::shared_ptr<AdaptationHook>& hook) {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    shared_->globalHooks.erase(
        std::remove(shared_->globalHooks.begin(), shared_->globalHooks.end(), hook),
        shared_->globalHooks.end());
}

void OptimizationPipeline::joinAsyncTasks() noexcept {
    std::vector<std::thread> threads;
    {
        std::lock_guard<std::mutex> lock(asyncMutex_);
        threads.swap(asyncThreads_);
    }
    for (auto& thread : threads) {
        if (!thread.joinable()) continue;
        if (thread.get_id() == std::this_thread::get_id()) {
            evolutionary_detail::joinWithoutDetach(std::move(thread));
        } else {
            thread.join();
        }
    }
}

} // namespace elizaos
