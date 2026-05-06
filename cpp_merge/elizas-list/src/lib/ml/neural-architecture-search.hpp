#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ML_NEURAL_ARCHITECTURE_SEARCH_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ML_NEURAL_ARCHITECTURE_SEARCH_H
#include "core.hpp"
// External dependency removed
#include "./evolutionary-optimizer.h"
#include "../monitoring/logger.h"

class NeuralArchitectureSearch;

class NeuralArchitectureSearch : public object, public std::enable_shared_from_this<NeuralArchitectureSearch> {
public:
    using std::enable_shared_from_this<NeuralArchitectureSearch>::shared_from_this;
    std::shared_ptr<EvolutionaryOptimizer> optimizer;

    std::shared_ptr<SearchSpace> searchSpace;

    double maxTrials;

    NeuralArchitectureSearch(std::shared_ptr<NASConfig> config);
    virtual std::shared_ptr<Promise<std::shared_ptr<NASResult>>> search(object data, object validationData);
    virtual std::shared_ptr<SearchSpace> defineSearchSpace(std::shared_ptr<NASConfig> config);
    virtual std::shared_ptr<Promise<std::shared_ptr<ArchitectureEvaluation>>> evaluateArchitecture(std::shared_ptr<NeuralArchitecture> architecture, object data, object validationData);
    virtual array<std::shared_ptr<tf::Callback>> createTrainingCallbacks();
};

#endif
