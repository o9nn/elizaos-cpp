#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-WEB-SEARCH_SRC_ACTIONS_WEBSEARCH_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-WEB-SEARCH_SRC_ACTIONS_WEBSEARCH_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/tavilyService.h"
#include "../types.h"

extern double DEFAULT_MAX_WEB_SEARCH_CHARS;
string MaxTokens(string data, double maxTokens = DEFAULT_MAX_WEB_SEARCH_CHARS);

extern std::shared_ptr<Action> webSearch;
#endif
