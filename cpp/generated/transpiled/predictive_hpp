#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_PREDICTIVE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_PREDICTIVE_H
#include "core.h"
#include "../ml/predictor.h"
#include "./segmentation.h"
#include "../monitoring/logger.h"

class PredictiveAnalytics;

class PredictiveAnalytics : public object, public std::enable_shared_from_this<PredictiveAnalytics> {
public:
    using std::enable_shared_from_this<PredictiveAnalytics>::shared_from_this;
    std::shared_ptr<MLPredictor> mlPredictor;

    PredictiveAnalytics();
    virtual void initialize();
    virtual std::shared_ptr<Promise<object>> predictProjectSuccess(string projectId);
    virtual std::shared_ptr<Promise<object>> predictUserEngagement(string userId);
};

#endif
