#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_SEARCH-ANALYTICS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_SEARCH-ANALYTICS_H
#include "core.h"
#include "./prisma.h"
using prisma = _default;

void trackSearch(std::string query, std::string userId = undefined);

std::any getPopularSearches(double limit = 10);

#endif
