#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ANALYTICS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ANALYTICS_H
#include "core.hpp"
#include "./prisma.h"
#include "next/headers.h"

class Analytics;

class Analytics : public object, public std::enable_shared_from_this<Analytics> {
public:
    using std::enable_shared_from_this<Analytics>::shared_from_this;
    static void trackProjectView(string projectId, string userId = undefined);
    static void trackRelatedProjectClick(string sourceProjectId, string targetProjectId, string userId = undefined);
};

#endif
