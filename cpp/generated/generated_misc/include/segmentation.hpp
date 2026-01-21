#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_SEGMENTATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_SEGMENTATION_H
#include "core.h"
#include "@prisma/client.h"
#include "ml-kmeans.h"
#include "../monitoring/logger.h"

class UserSegment;
class UserSegmentation;

extern any prisma;
class UserSegment : public object, public std::enable_shared_from_this<UserSegment> {
public:
    using std::enable_shared_from_this<UserSegment>::shared_from_this;
    string id;

    string name;

    Record<string, any> characteristics;

    double size;
};

class UserSegmentation : public object, public std::enable_shared_from_this<UserSegmentation> {
public:
    using std::enable_shared_from_this<UserSegmentation>::shared_from_this;
    static std::shared_ptr<Promise<array<std::shared_ptr<UserSegment>>>> generateSegments();
};

#endif
