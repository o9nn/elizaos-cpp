#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ML_PREDICTOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ML_PREDICTOR_H
#include "core.h"

class MLPredictor;

class MLPredictor : public object, public std::enable_shared_from_this<MLPredictor> {
public:
    using std::enable_shared_from_this<MLPredictor>::shared_from_this;
    virtual any loadModel();
    virtual any predictEngagement(any features);
};

#endif
