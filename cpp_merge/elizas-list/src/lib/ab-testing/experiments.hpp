#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_AB_TESTING_EXPERIMENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_AB_TESTING_EXPERIMENTS_H
#include "core.hpp"
#include "ioredis.hpp"
#include "../monitoring/logger.h"
#include "../monitoring/metrics.h"

class Experiment;
class ABTestingService;

extern any redis;
class Experiment : public object, public std::enable_shared_from_this<Experiment> {
public:
    using std::enable_shared_from_this<Experiment>::shared_from_this;
    string id;

    string name;

    array<object> variants;

    std::shared_ptr<Date> startDate;

    std::shared_ptr<Date> endDate;
};

class ABTestingService : public object, public std::enable_shared_from_this<ABTestingService> {
public:
    using std::enable_shared_from_this<ABTestingService>::shared_from_this;
    static std::shared_ptr<Promise<string>> assignVariant(string experimentId, string userId);
    static void trackConversion(string experimentId, string userId, string conversionType, double value = std::nullopt);
    static any getResults(string experimentId);
    static any calculateStatistics(array<any> results);
};

#endif
