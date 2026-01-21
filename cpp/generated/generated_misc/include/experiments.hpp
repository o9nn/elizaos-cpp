#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_AB-TESTING_EXPERIMENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_AB-TESTING_EXPERIMENTS_H
#include "core.h"
#include "ioredis.h"
#include "../monitoring/logger.h"
#include "../monitoring/metrics.h"

class Experiment;
class ABTestingService;

extern std::any redis;
class Experiment : public object, public std::enable_shared_from_this<Experiment> {
public:
    using std::enable_shared_from_this<Experiment>::shared_from_this;
    std::string id;

    std::string name;

    array<object> variants;

    std::shared_ptr<Date> startDate;

    std::shared_ptr<Date> endDate;
};

class ABTestingService : public object, public std::enable_shared_from_this<ABTestingService> {
public:
    using std::enable_shared_from_this<ABTestingService>::shared_from_this;
    static std::shared_ptr<Promise<string>> assignVariant(std::string experimentId, std::string userId);
    static void trackConversion(std::string experimentId, std::string userId, std::string conversionType, double value = undefined);
    static std::any getResults(std::string experimentId);
    static std::any calculateStatistics(array<any> results);
};

#endif
