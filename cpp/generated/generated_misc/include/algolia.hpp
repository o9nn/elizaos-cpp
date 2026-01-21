#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ALGOLIA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ALGOLIA_H
#include "core.h"
#include "algoliasearch.h"
using algoliasearch = _default;

extern std::any client;
extern std::any index;
void indexProject(std::any project);

void removeProject(std::string projectId);

#endif
