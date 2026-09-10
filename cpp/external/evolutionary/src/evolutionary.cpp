#include "elizaos/evolutionary.hpp"
#include "thread_reaper.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace elizaos {
namespace {

constexpr std::uint32_t kDeterministicSeed = 0xE11A05u;

std::uint32_t stableSeed(const std::string& text) noexcept {
    std::uint32_t value = 2166136261u;
    for (const unsigned char byte : text) {
        value ^= byte;
        value *= 16777619u;
    }
    return value;
}

void validateProbability(double value, const char* name) {
    if (!std::isfinite(value) || value < 0.0 || value > 1.0) {
        throw std::invalid_argument(std::string(name) + " must be finite and in [0, 1]");
    }
}

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

bool scoreLess(const Individual& lhs, const Individual& rhs) {
    return lhs.getFitness().getOverallScore() < rhs.getFitness().getOverallScore();
}

bool scoreGreater(const Individual& lhs, const Individual& rhs) {
    return lhs.getFitness().getOverallScore() > rhs.getFitness().getOverallScore();
}

} // namespace

std::shared_ptr<ProgramNode> ProgramNode::clone() const {
    auto cloned = std::make_shared<ProgramNode>(type, name);
    cloned->parameters = parameters;
    cloned->children.reserve(children.size());
    for (const auto& child : children) {
        cloned->children.push_back(child ? child->clone() : nullptr);
    }
    return cloned;
}

double ProgramNode::evaluate(
    const std::unordered_map<std::string, double>& context) const {
    const auto childValue = [&](std::size_t index) {
        return children[index] ? children[index]->evaluate(context) : 0.0;
    };

    switch (type) {
        case Type::CONSTANT:
            return parameters.empty() ? 0.0 : parameters.front();
        case Type::VARIABLE: {
            const auto it = context.find(name);
            return it == context.end() ? 0.0 : it->second;
        }
        case Type::FUNCTION:
            if (name == "add" && children.size() >= 2) return childValue(0) + childValue(1);
            if (name == "sub" && children.size() >= 2) return childValue(0) - childValue(1);
            if (name == "mul" && children.size() >= 2) return childValue(0) * childValue(1);
            if (name == "div" && children.size() >= 2) {
                const double divisor = childValue(1);
                return divisor == 0.0 ? 0.0 : childValue(0) / divisor;
            }
            if (name == "sin" && !children.empty()) return std::sin(childValue(0));
            if (name == "cos" && !children.empty()) return std::cos(childValue(0));
            if (name == "exp" && !children.empty()) return std::exp(childValue(0));
            if (name == "log" && !children.empty()) {
                const double value = childValue(0);
                return value > 0.0 ? std::log(value) : 0.0;
            }
            if (name == "max" && children.size() >= 2) return std::max(childValue(0), childValue(1));
            if (name == "min" && children.size() >= 2) return std::min(childValue(0), childValue(1));
            return 0.0;
        case Type::CONDITIONAL:
            if (name == "if" && children.size() >= 3) {
                return childValue(0) > 0.0 ? childValue(1) : childValue(2);
            }
            if (name == "gt" && children.size() >= 2) {
                return childValue(0) > childValue(1) ? 1.0 : 0.0;
            }
            if (name == "lt" && children.size() >= 2) {
                return childValue(0) < childValue(1) ? 1.0 : 0.0;
            }
            return 0.0;
    }
    return 0.0;
}

std::string ProgramNode::toString() const {
    std::ostringstream output;
    if (type == Type::CONSTANT) {
        output << (parameters.empty() ? 0.0 : parameters.front());
    } else if (type == Type::VARIABLE) {
        output << name;
    } else {
        output << '(' << name;
        for (const auto& child : children) {
            output << ' ' << (child ? child->toString() : "null");
        }
        output << ')';
    }
    return output.str();
}

FitnessResult::FitnessResult() : fitness(0.0), complexity(0.0), novelty(0.0) {}

FitnessResult::FitnessResult(double f, double c, double n)
    : fitness(f), complexity(c), novelty(n) {
    validate();
}

bool FitnessResult::isFinite() const noexcept {
    if (!std::isfinite(fitness) || !std::isfinite(complexity) ||
        !std::isfinite(novelty)) {
        return false;
    }
    for (const double value : behaviorSignature) {
        if (!std::isfinite(value)) return false;
    }
    const double score = fitness - 0.1 * complexity + 0.05 * novelty;
    return std::isfinite(score);
}

void FitnessResult::validate() const {
    if (!isFinite()) {
        throw std::invalid_argument("fitness result must contain only finite values");
    }
}

double FitnessResult::getOverallScore() const {
    validate();
    return fitness - 0.1 * complexity + 0.05 * novelty;
}

Individual::Individual(std::shared_ptr<ProgramNode> program)
    : program_(std::move(program)), id_(elizaos::generateUUID()) {}

Individual::Individual(const Individual& other)
    : program_(other.program_ ? other.program_->clone() : nullptr),
      fitness_(other.fitness_),
      age_(other.age_),
      id_(elizaos::generateUUID()) {}

Individual& Individual::operator=(const Individual& other) {
    if (this != &other) {
        program_ = other.program_ ? other.program_->clone() : nullptr;
        fitness_ = other.fitness_;
        age_ = other.age_;
        id_ = elizaos::generateUUID();
    }
    return *this;
}

void Individual::setFitness(const FitnessResult& fitness) {
    fitness.validate();
    fitness_ = fitness;
}

Individual Individual::crossover(const Individual& parent1,
                                 const Individual& parent2) {
    if (!parent1.program_ || !parent2.program_) return Individual(nullptr);

    auto offspring = parent1.program_->clone();
    std::vector<std::shared_ptr<ProgramNode>> targets;
    std::vector<std::shared_ptr<ProgramNode>> donors;
    const auto collect = [](const std::shared_ptr<ProgramNode>& root,
                            std::vector<std::shared_ptr<ProgramNode>>& nodes) {
        std::function<void(const std::shared_ptr<ProgramNode>&)> visit;
        visit = [&](const std::shared_ptr<ProgramNode>& node) {
            if (!node) return;
            nodes.push_back(node);
            for (const auto& child : node->children) visit(child);
        };
        visit(root);
    };
    collect(offspring, targets);
    collect(parent2.program_, donors);

    if (!targets.empty() && !donors.empty()) {
        const std::uint32_t seed = stableSeed(parent1.program_->toString() + "|" +
                                              parent2.program_->toString());
        std::mt19937 generator(seed);
        const std::size_t firstTarget = targets.size() > 1 ? 1 : 0;
        std::uniform_int_distribution<std::size_t> targetDistribution(
            firstTarget, targets.size() - 1);
        std::uniform_int_distribution<std::size_t> donorDistribution(
            0, donors.size() - 1);
        const auto target = targets[targetDistribution(generator)];
        const auto donor = donors[donorDistribution(generator)];
        *target = *donor->clone();
    }
    return Individual(offspring);
}

Individual Individual::mutate(double mutationRate) const {
    validateProbability(mutationRate, "mutationRate");
    if (!program_) return Individual(nullptr);

    auto mutated = program_->clone();
    std::mt19937 generator(stableSeed(program_->toString()) ^
                           static_cast<std::uint32_t>(mutationRate * 1000000.0));
    std::uniform_real_distribution<double> probability(0.0, 1.0);
    std::function<void(const std::shared_ptr<ProgramNode>&)> mutateNode;
    mutateNode = [&](const std::shared_ptr<ProgramNode>& node) {
        if (!node) return;
        if (probability(generator) < mutationRate) {
            if (node->type == ProgramNode::Type::CONSTANT &&
                !node->parameters.empty()) {
                std::normal_distribution<double> value(node->parameters.front(), 0.1);
                node->parameters.front() = value(generator);
            } else if (node->type == ProgramNode::Type::FUNCTION) {
                static const std::vector<std::string> functions{
                    "add", "sub", "mul", "div", "sin", "cos", "max", "min"};
                std::uniform_int_distribution<std::size_t> functionDistribution(
                    0, functions.size() - 1);
                node->name = functions[functionDistribution(generator)];
            }
        }
        for (const auto& child : node->children) mutateNode(child);
    };
    mutateNode(mutated);
    return Individual(mutated);
}

double Individual::similarity(const Individual& other) const {
    if (!program_ || !other.program_) return 0.0;
    const std::string first = program_->toString();
    const std::string second = other.program_->toString();
    if (first == second) return 1.0;

    std::set<std::string> firstTokens;
    std::set<std::string> secondTokens;
    std::istringstream firstStream(first);
    std::istringstream secondStream(second);
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

std::string Individual::serialize() const {
    std::ostringstream output;
    output << "Individual{id:" << id_ << ",age:" << age_
           << ",fitness:" << fitness_.fitness
           << ",program:" << (program_ ? program_->toString() : "null") << '}';
    return output.str();
}

Individual Individual::deserialize(const std::string& data) {
    const std::size_t idPosition = data.find("id:");
    const std::size_t agePosition = data.find(",age:");
    const std::size_t fitnessPosition = data.find(",fitness:");
    const std::size_t programPosition = data.find(",program:");
    if (idPosition == std::string::npos || agePosition == std::string::npos ||
        fitnessPosition == std::string::npos || programPosition == std::string::npos ||
        !(idPosition < agePosition && agePosition < fitnessPosition &&
          fitnessPosition < programPosition)) {
        throw std::invalid_argument("malformed serialized individual");
    }

    Individual individual(nullptr);
    try {
        individual.id_ = data.substr(idPosition + 3,
                                     agePosition - (idPosition + 3));
        individual.age_ = std::stoi(data.substr(agePosition + 5,
                                                fitnessPosition - (agePosition + 5)));
        individual.setFitness(FitnessResult(std::stod(data.substr(
            fitnessPosition + 9, programPosition - (fitnessPosition + 9)))));
    } catch (const std::exception&) {
        throw std::invalid_argument("malformed serialized individual");
    }
    return individual;
}

Population::Population(std::size_t maxSize) : maxSize_(maxSize) {
    if (maxSize_ == 0) throw std::invalid_argument("population maximum size must be nonzero");
}

Population::Population(const Population& other) : maxSize_(other.maxSize()) {
    individuals_ = other.getIndividuals();
}

Population& Population::operator=(const Population& other) {
    if (this == &other) return *this;
    const auto snapshot = other.getIndividuals();
    const std::size_t newMaximum = other.maxSize();
    std::lock_guard<std::mutex> lock(populationMutex_);
    maxSize_ = newMaximum;
    individuals_ = snapshot;
    return *this;
}

void Population::addIndividual(const Individual& individual) {
    individual.getFitness().validate();
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (individuals_.size() < maxSize_) {
        individuals_.push_back(individual);
        return;
    }
    const auto worst = std::min_element(individuals_.begin(), individuals_.end(), scoreLess);
    if (worst != individuals_.end() && scoreGreater(individual, *worst)) {
        *worst = individual;
    }
}

void Population::removeIndividual(std::size_t index) {
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (index >= individuals_.size()) throw std::out_of_range("population index");
    individuals_.erase(individuals_.begin() + static_cast<std::ptrdiff_t>(index));
}

void Population::setIndividual(std::size_t index, const Individual& individual) {
    individual.getFitness().validate();
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (index >= individuals_.size()) throw std::out_of_range("population index");
    individuals_[index] = individual;
}

void Population::replaceIndividuals(const std::vector<Individual>& individuals) {
    if (individuals.size() > maxSize_) {
        throw std::invalid_argument("replacement population exceeds maximum size");
    }
    for (const auto& individual : individuals) individual.getFitness().validate();
    std::lock_guard<std::mutex> lock(populationMutex_);
    individuals_ = individuals;
}

std::size_t Population::size() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    return individuals_.size();
}

std::size_t Population::maxSize() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    return maxSize_;
}

bool Population::empty() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    return individuals_.empty();
}

Individual Population::getIndividual(std::size_t index) const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    return individuals_.at(index);
}

std::vector<Individual> Population::getIndividuals() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    return individuals_;
}

FitnessResult Population::getBestFitness() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (individuals_.empty()) return FitnessResult{};
    return std::max_element(individuals_.begin(), individuals_.end(), scoreLess)->getFitness();
}

FitnessResult Population::getAverageFitness() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (individuals_.empty()) return FitnessResult{};
    FitnessResult average;
    for (const auto& individual : individuals_) {
        const FitnessResult fitness = individual.getFitness();
        average.fitness += fitness.fitness;
        average.complexity += fitness.complexity;
        average.novelty += fitness.novelty;
    }
    const double count = static_cast<double>(individuals_.size());
    average.fitness /= count;
    average.complexity /= count;
    average.novelty /= count;
    average.validate();
    return average;
}

double Population::getDiversity() const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (individuals_.size() < 2) return 0.0;
    double similarity = 0.0;
    std::size_t comparisons = 0;
    for (std::size_t i = 0; i < individuals_.size(); ++i) {
        for (std::size_t j = i + 1; j < individuals_.size(); ++j) {
            similarity += individuals_[i].similarity(individuals_[j]);
            ++comparisons;
        }
    }
    return 1.0 - similarity / static_cast<double>(comparisons);
}

std::vector<Individual> Population::tournamentSelection(
    std::size_t tournamentSize, std::size_t numSelected) const {
    if (tournamentSize == 0) throw std::invalid_argument("tournament size must be nonzero");
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (numSelected == 0) return {};
    if (individuals_.empty()) throw std::logic_error("cannot select from an empty population");

    std::mt19937 generator(kDeterministicSeed);
    std::uniform_int_distribution<std::size_t> distribution(0, individuals_.size() - 1);
    std::vector<Individual> selected;
    selected.reserve(numSelected);
    for (std::size_t i = 0; i < numSelected; ++i) {
        Individual best = individuals_[distribution(generator)];
        for (std::size_t candidateIndex = 1; candidateIndex < tournamentSize;
             ++candidateIndex) {
            const Individual& candidate = individuals_[distribution(generator)];
            if (scoreGreater(candidate, best)) best = candidate;
        }
        selected.push_back(best);
    }
    return selected;
}

std::vector<Individual> Population::rouletteWheelSelection(
    std::size_t numSelected) const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    if (numSelected == 0) return {};
    if (individuals_.empty()) throw std::logic_error("cannot select from an empty population");

    double minimum = std::numeric_limits<double>::infinity();
    for (const auto& individual : individuals_) {
        minimum = std::min(minimum, individual.getFitness().getOverallScore());
    }
    const double shift = minimum <= 0.0 ? -minimum + 1.0 : 0.0;
    std::vector<double> cumulative;
    cumulative.reserve(individuals_.size());
    double total = 0.0;
    for (const auto& individual : individuals_) {
        total += individual.getFitness().getOverallScore() + shift;
        cumulative.push_back(total);
    }
    if (!std::isfinite(total) || total <= 0.0) {
        throw std::invalid_argument("population scores cannot form selection weights");
    }

    std::mt19937 generator(kDeterministicSeed);
    std::uniform_real_distribution<double> distribution(0.0, total);
    std::vector<Individual> selected;
    selected.reserve(numSelected);
    for (std::size_t i = 0; i < numSelected; ++i) {
        const double draw = distribution(generator);
        const auto it = std::lower_bound(cumulative.begin(), cumulative.end(), draw);
        const std::size_t index = it == cumulative.end()
                                      ? individuals_.size() - 1
                                      : static_cast<std::size_t>(it - cumulative.begin());
        selected.push_back(individuals_[index]);
    }
    return selected;
}

std::vector<Individual> Population::eliteSelection(std::size_t numElite) const {
    std::lock_guard<std::mutex> lock(populationMutex_);
    std::vector<Individual> elite = individuals_;
    std::stable_sort(elite.begin(), elite.end(), scoreGreater);
    if (elite.size() > numElite) {
        elite.erase(elite.begin() + static_cast<std::ptrdiff_t>(numElite),
                    elite.end());
    }
    return elite;
}

void Population::sort() {
    std::lock_guard<std::mutex> lock(populationMutex_);
    std::stable_sort(individuals_.begin(), individuals_.end(), scoreGreater);
}

void Population::ageIndividuals() {
    std::lock_guard<std::mutex> lock(populationMutex_);
    for (auto& individual : individuals_) individual.incrementAge();
}

void Population::clear() {
    std::lock_guard<std::mutex> lock(populationMutex_);
    individuals_.clear();
}

struct EvolutionaryOptimizer::SharedState {
    explicit SharedState(const Config& initialConfig)
        : config(initialConfig), population(initialConfig.populationSize), rng(kDeterministicSeed) {}

    mutable std::mutex mutex;
    std::condition_variable controlCv;
    Config config;
    Population population;
    std::vector<Statistics> history;
    bool running = false;
    bool paused = false;
    bool stopped = false;
    bool shuttingDown = false;
    std::mt19937 rng;
};

EvolutionaryOptimizer::EvolutionaryOptimizer(const Config& config) {
    validateConfig(config);
    shared_ = std::make_shared<SharedState>(config);
}

EvolutionaryOptimizer::~EvolutionaryOptimizer() {
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        shared_->shuttingDown = true;
        shared_->stopped = true;
        shared_->paused = false;
    }
    shared_->controlCv.notify_all();
    joinAsyncTasks();
}

void EvolutionaryOptimizer::validateConfig(const Config& config) {
    if (config.populationSize == 0) {
        throw std::invalid_argument("populationSize must be nonzero");
    }
    if (config.maxGenerations == 0) {
        throw std::invalid_argument("maxGenerations must be nonzero");
    }
    validateProbability(config.mutationRate, "mutationRate");
    validateProbability(config.crossoverRate, "crossoverRate");
    validateProbability(config.eliteRatio, "eliteRatio");
    validateProbability(config.diversityThreshold, "diversityThreshold");
    if (config.tournamentSize == 0) {
        throw std::invalid_argument("tournamentSize must be nonzero");
    }
    if (config.maxComplexity <= 0) {
        throw std::invalid_argument("maxComplexity must be positive");
    }
    if (!std::isfinite(config.stagnationThreshold) ||
        config.stagnationThreshold < 0.0) {
        throw std::invalid_argument("stagnationThreshold must be finite and nonnegative");
    }
    if (config.maxStagnationGenerations <= 0) {
        throw std::invalid_argument("maxStagnationGenerations must be positive");
    }
}

std::shared_ptr<ProgramNode> EvolutionaryOptimizer::generateRandomProgram(
    SharedState& shared, int maxDepth) {
    std::uniform_int_distribution<int> typeDistribution(0, 3);
    std::uniform_real_distribution<double> valueDistribution(-10.0, 10.0);
    ProgramNode::Type type = static_cast<ProgramNode::Type>(typeDistribution(shared.rng));
    if (maxDepth <= 0) {
        type = typeDistribution(shared.rng) % 2 == 0
                   ? ProgramNode::Type::CONSTANT
                   : ProgramNode::Type::VARIABLE;
    }

    if (type == ProgramNode::Type::CONSTANT) {
        auto node = std::make_shared<ProgramNode>(type, "const");
        node->parameters.push_back(valueDistribution(shared.rng));
        return node;
    }
    if (type == ProgramNode::Type::VARIABLE) {
        static const std::vector<std::string> variables{
            "x", "y", "z", "t", "fitness", "age"};
        std::uniform_int_distribution<std::size_t> distribution(0, variables.size() - 1);
        return std::make_shared<ProgramNode>(type, variables[distribution(shared.rng)]);
    }
    if (type == ProgramNode::Type::FUNCTION) {
        static const std::vector<std::string> functions{
            "add", "sub", "mul", "div", "sin", "cos", "exp", "log", "max", "min"};
        std::uniform_int_distribution<std::size_t> distribution(0, functions.size() - 1);
        auto node = std::make_shared<ProgramNode>(type, functions[distribution(shared.rng)]);
        const int arity = node->name == "sin" || node->name == "cos" ||
                                  node->name == "exp" || node->name == "log"
                              ? 1
                              : 2;
        for (int index = 0; index < arity; ++index) {
            node->children.push_back(generateRandomProgram(shared, maxDepth - 1));
        }
        return node;
    }

    static const std::vector<std::string> conditionals{"if", "gt", "lt"};
    std::uniform_int_distribution<std::size_t> distribution(0, conditionals.size() - 1);
    auto node = std::make_shared<ProgramNode>(type, conditionals[distribution(shared.rng)]);
    const int arity = node->name == "if" ? 3 : 2;
    for (int index = 0; index < arity; ++index) {
        node->children.push_back(generateRandomProgram(shared, maxDepth - 1));
    }
    return node;
}

Individual EvolutionaryOptimizer::optimize(const FitnessFunction& fitnessFunc,
                                            const State& state) {
    return optimizeImpl(shared_, fitnessFunc, state, nullptr);
}

Individual EvolutionaryOptimizer::optimize(
    const FitnessFunction& fitnessFunc, const State& state,
    const std::vector<Individual>& initialPopulation) {
    return optimizeImpl(shared_, fitnessFunc, state, &initialPopulation);
}

Individual EvolutionaryOptimizer::optimizeImpl(
    const std::shared_ptr<SharedState>& shared, const FitnessFunction& fitnessFunc,
    const State& state, const std::vector<Individual>* initialPopulation) {
    if (!fitnessFunc) throw std::invalid_argument("fitness function must be callable");

    Config config;
    {
        std::lock_guard<std::mutex> lock(shared->mutex);
        if (shared->shuttingDown) {
            throw std::logic_error("optimizer is shutting down");
        }
        if (shared->running) throw std::logic_error("optimizer is already running");
        validateConfig(shared->config);
        shared->running = true;
        shared->paused = false;
        shared->stopped = false;
        shared->history.clear();
        shared->rng.seed(kDeterministicSeed);
        config = shared->config;
    }
    const auto finishRun = makeScopeExit([&] {
        std::lock_guard<std::mutex> lock(shared->mutex);
        shared->running = false;
        shared->paused = false;
        shared->controlCv.notify_all();
    });

    Population population(config.populationSize);
    if (initialPopulation) {
        if (initialPopulation->size() > config.populationSize) {
            throw std::invalid_argument("initial population exceeds configured size");
        }
        population.replaceIndividuals(*initialPopulation);
    } else {
        const auto existing = shared->population.getIndividuals();
        if (existing.size() <= config.populationSize) population.replaceIndividuals(existing);
    }
    while (population.size() < config.populationSize) {
        population.addIndividual(Individual(generateRandomProgram(*shared)));
    }

    bool evaluatedAnyGeneration = false;
    for (std::size_t generation = 0; generation < config.maxGenerations; ++generation) {
        {
            std::unique_lock<std::mutex> lock(shared->mutex);
            shared->controlCv.wait(lock, [&] { return !shared->paused || shared->stopped; });
            if (shared->stopped) break;
        }

        const auto generationStart = std::chrono::steady_clock::now();
        auto individuals = population.getIndividuals();
        for (auto& individual : individuals) {
            FitnessResult fitness = fitnessFunc(individual, state);
            fitness.validate();
            if (individual.getProgram()) {
                fitness.complexity = static_cast<double>(
                    individual.getProgram()->toString().size());
                if (fitness.complexity > static_cast<double>(config.maxComplexity)) {
                    fitness.fitness *= 0.5;
                }
            }
            fitness.validate();
            individual.setFitness(fitness);
        }

        if (config.useNoveltySearch && individuals.size() > 1) {
            for (std::size_t i = 0; i < individuals.size(); ++i) {
                double novelty = 0.0;
                for (std::size_t j = 0; j < individuals.size(); ++j) {
                    if (i != j) novelty += 1.0 - individuals[i].similarity(individuals[j]);
                }
                FitnessResult fitness = individuals[i].getFitness();
                fitness.novelty = novelty / static_cast<double>(individuals.size() - 1);
                individuals[i].setFitness(fitness);
            }
        }
        population.replaceIndividuals(individuals);
        evaluatedAnyGeneration = true;

        Statistics statistics;
        statistics.generation = generation;
        statistics.bestFitness = population.getBestFitness();
        statistics.averageFitness = population.getAverageFitness();
        statistics.diversity = population.getDiversity();
        statistics.generationTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - generationStart);

        {
            std::lock_guard<std::mutex> lock(shared->mutex);
            if (!shared->history.empty()) {
                const double previous = shared->history.back().bestFitness.getOverallScore();
                const double current = statistics.bestFitness.getOverallScore();
                statistics.convergenceRate = current - previous;
                statistics.stagnationCount =
                    statistics.convergenceRate < config.stagnationThreshold
                        ? shared->history.back().stagnationCount + 1
                        : 0;
            }
            shared->history.push_back(statistics);
            shared->population = population;
        }

        const bool convergenceEvidence =
            !population.empty() &&
            (statistics.diversity <= config.diversityThreshold ||
             statistics.stagnationCount >= config.maxStagnationGenerations);
        if (convergenceEvidence || generation + 1 == config.maxGenerations) break;

        std::stable_sort(individuals.begin(), individuals.end(), scoreGreater);
        const std::size_t requestedElite = static_cast<std::size_t>(
            static_cast<double>(config.populationSize) * config.eliteRatio);
        const std::size_t eliteCount = std::min(requestedElite, individuals.size());
        std::vector<Individual> nextGeneration;
        nextGeneration.reserve(config.populationSize);
        nextGeneration.insert(nextGeneration.end(), individuals.begin(),
                              individuals.begin() + static_cast<std::ptrdiff_t>(eliteCount));

        std::uniform_int_distribution<std::size_t> parentDistribution(
            0, individuals.size() - 1);
        std::uniform_real_distribution<double> probability(0.0, 1.0);
        const auto tournamentParent = [&]() -> Individual {
            Individual best = individuals[parentDistribution(shared->rng)];
            for (std::size_t index = 1; index < config.tournamentSize; ++index) {
                const Individual& candidate = individuals[parentDistribution(shared->rng)];
                if (scoreGreater(candidate, best)) best = candidate;
            }
            return best;
        };

        while (nextGeneration.size() < config.populationSize) {
            const Individual first = tournamentParent();
            const Individual second = tournamentParent();
            Individual child = probability(shared->rng) < config.crossoverRate
                                   ? Individual::crossover(first, second)
                                   : first;
            if (probability(shared->rng) < config.mutationRate) {
                child = child.mutate(config.mutationRate);
            }
            nextGeneration.push_back(child);
        }

        if (config.useDemeSplitting && statistics.diversity < config.diversityThreshold &&
            nextGeneration.size() > 3) {
            const std::size_t replacements = nextGeneration.size() / 4;
            for (std::size_t index = 0; index < replacements; ++index) {
                nextGeneration[nextGeneration.size() - 1 - index] =
                    Individual(generateRandomProgram(*shared));
            }
        }
        population.replaceIndividuals(nextGeneration);
        population.ageIndividuals();
    }

    if (!evaluatedAnyGeneration) return Individual(nullptr);
    population.sort();
    {
        std::lock_guard<std::mutex> lock(shared->mutex);
        shared->population = population;
    }
    return population.getIndividual(0);
}

std::future<Individual> EvolutionaryOptimizer::optimizeAsync(
    const FitnessFunction& fitnessFunc, const State& state) {
    auto promise = std::make_shared<std::promise<Individual>>();
    std::future<Individual> future = promise->get_future();
    const auto shared = shared_;
    const State stateCopy = state;
    std::thread worker([shared, fitnessFunc, stateCopy, promise] {
        try {
            promise->set_value(optimizeImpl(shared, fitnessFunc, stateCopy, nullptr));
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

void EvolutionaryOptimizer::setPopulation(const Population& population) {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (shared_->running) throw std::logic_error("cannot replace population while running");
    if (population.size() > shared_->config.populationSize) {
        throw std::invalid_argument("population exceeds configured size");
    }
    Population replacement(shared_->config.populationSize);
    replacement.replaceIndividuals(population.getIndividuals());
    shared_->population = replacement;
}

std::shared_ptr<Population> EvolutionaryOptimizer::getPopulation() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return std::make_shared<Population>(shared_->population);
}

void EvolutionaryOptimizer::setConfig(const Config& config) {
    validateConfig(config);
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (shared_->running) throw std::logic_error("cannot change config while running");
    const auto current = shared_->population.getIndividuals();
    Population replacement(config.populationSize);
    std::vector<Individual> retained = current;
    if (retained.size() > config.populationSize) {
        retained.erase(retained.begin() +
                           static_cast<std::ptrdiff_t>(config.populationSize),
                       retained.end());
    }
    replacement.replaceIndividuals(retained);
    shared_->population = replacement;
    shared_->config = config;
}

EvolutionaryOptimizer::Config EvolutionaryOptimizer::getConfig() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return shared_->config;
}

void EvolutionaryOptimizer::pause() {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    if (shared_->running) shared_->paused = true;
}

void EvolutionaryOptimizer::resume() {
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        shared_->paused = false;
    }
    shared_->controlCv.notify_all();
}

void EvolutionaryOptimizer::stop() {
    {
        std::lock_guard<std::mutex> lock(shared_->mutex);
        shared_->stopped = true;
        shared_->paused = false;
    }
    shared_->controlCv.notify_all();
}

bool EvolutionaryOptimizer::isRunning() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return shared_->running;
}

EvolutionaryOptimizer::Statistics EvolutionaryOptimizer::getStatistics() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return shared_->history.empty() ? Statistics{} : shared_->history.back();
}

std::vector<EvolutionaryOptimizer::Statistics> EvolutionaryOptimizer::getHistory() const {
    std::lock_guard<std::mutex> lock(shared_->mutex);
    return shared_->history;
}

void EvolutionaryOptimizer::joinAsyncTasks() noexcept {
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
