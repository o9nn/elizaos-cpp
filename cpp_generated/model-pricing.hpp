#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_AGENT_UTILS_MODEL-PRICING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_AGENT_UTILS_MODEL-PRICING_H
#include "core.h"

class ModelPricing;

class ModelPricing : public object, public std::enable_shared_from_this<ModelPricing> {
public:
    using std::enable_shared_from_this<ModelPricing>::shared_from_this;
    double inputCostPer1k;

    double outputCostPer1k;

    double maxInputTokens;

    double maxOutputTokens;

    string provider;
};

extern Record<string, std::shared_ptr<ModelPricing>> MODEL_PRICING;
double calculateCost(string modelName, double inputTokens, double outputTokens);

object getModelLimits(string modelName);

boolean isWithinTokenLimit(string modelName, double inputTokens, double outputTokens);

#endif
