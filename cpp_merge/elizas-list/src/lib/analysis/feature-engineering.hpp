#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ANALYSIS_FEATURE_ENGINEERING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ANALYSIS_FEATURE_ENGINEERING_H
#include "core.hpp"
// External dependency removed
#include "ml-pca.h"
#include "../monitoring/logger.h"

class FeatureEngineering;

class FeatureEngineering : public object, public std::enable_shared_from_this<FeatureEngineering> {
public:
    using std::enable_shared_from_this<FeatureEngineering>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<EngineeredFeatures>>> engineerFeatures(array<Record<string, any>> data, std::shared_ptr<FeatureConfig> config);
    virtual any processNumericalFeatures(array<Record<string, any>> data, std::shared_ptr<NumericalFeatureConfig> config);
    virtual any processCategoricalFeatures(array<Record<string, any>> data, std::shared_ptr<CategoricalFeatureConfig> config);
};

#endif
