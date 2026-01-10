#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-WEB-SEARCH_SRC_ACTIONS_CRYPTONEWS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-WEB-SEARCH_SRC_ACTIONS_CRYPTONEWS_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/tavilyService.h"
#include "../services/coindeskService.h"
#include "../types.h"

extern double DEFAULT_MAX_CRYPTO_NEWS_CHARS;
extern object CRYPTO_NEWS_SOURCES;
string MaxTokens(string data, double maxTokens = DEFAULT_MAX_CRYPTO_NEWS_CHARS);

extern std::shared_ptr<Action> cryptoNews;
#endif
