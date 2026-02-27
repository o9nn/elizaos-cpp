#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ALGOLIA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ALGOLIA_H
#include "core.h"
#include "algoliasearch.h"
using algoliasearch = _default;

extern any client;
extern any index;
void indexProject(any project);

void removeProject(string projectId);

#endif
