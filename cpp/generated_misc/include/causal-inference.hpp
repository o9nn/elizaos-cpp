#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYSIS_CAUSAL-INFERENCE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYSIS_CAUSAL-INFERENCE_H
#include "core.h"
#include "causality-js.h"
#include "r-script.h"
#include "../monitoring/logger.h"

class CausalInference;

class CausalInference : public object, public std::enable_shared_from_this<CausalInference> {
public:
    using std::enable_shared_from_this<CausalInference>::shared_from_this;
    std::shared_ptr<DAG> dag;

    CausalInference();
    virtual std::shared_ptr<Promise<std::shared_ptr<CausalAnalysisResult>>> analyzeCausalRelationships(array<Record<string, any>> data, array<string> variables, array<string> treatments, array<string> outcomes);
    virtual std::shared_ptr<Promise<void>> buildDAG(array<Record<string, any>> data, array<string> variables);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<CausalEffect>>>> estimateCausalEffects(array<Record<string, any>> data, array<string> treatments, array<string> outcomes);
    virtual std::shared_ptr<Promise<object>> doubleMLEstimation(array<Record<string, any>> data, string treatment, string outcome);
    virtual std::shared_ptr<Promise<std::shared_ptr<SensitivityAnalysis>>> performSensitivityAnalysis(array<Record<string, any>> data, array<std::shared_ptr<CausalEffect>> effects);
};

#endif
