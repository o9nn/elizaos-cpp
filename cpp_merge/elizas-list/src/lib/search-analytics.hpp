#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_SEARCH_ANALYTICS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_SEARCH_ANALYTICS_H
#include "core.hpp"
#include "./prisma.h"
// Using alias removed (invalid transpilation)

void trackSearch(string query, string userId = undefined);

any getPopularSearches(double limit = 10);

#endif
