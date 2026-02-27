#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_WEB_SEARCH_SRC_ACTIONS_CRYPTONEWS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_WEB_SEARCH_SRC_ACTIONS_CRYPTONEWS_H
#include "core.hpp"
// External dependency removed
#include "../services/tavilyService.h"
#include "../services/coindeskService.h"
#include "../types.h"

extern double DEFAULT_MAX_CRYPTO_NEWS_CHARS;
extern object CRYPTO_NEWS_SOURCES;
string MaxTokens(string data, double maxTokens = DEFAULT_MAX_CRYPTO_NEWS_CHARS);

extern std::shared_ptr<Action> cryptoNews;
#endif
