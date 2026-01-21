#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_SEARCH-ANALYTICS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_SEARCH-ANALYTICS_H
#include "core.h"
#include "./prisma.h"
using prisma = _default;

void trackSearch(string query, string userId = undefined);

any getPopularSearches(double limit = 10);

#endif
