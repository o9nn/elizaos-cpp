#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_TRANSLATION_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_TRANSLATION_SERVICE_H
#include "core.hpp"
// External dependency removed
#include "./types.ts.h"

extern std::shared_ptr<TranslationServiceConfig> serviceConfig;
extern std::shared_ptr<TranslationService> translationService;
extern std::function<void(any)> initializeTranslationConfig;
std::shared_ptr<Promise<object>> simulateTranslationAPI(string text, string targetLang, string sourceLang = undefined, object config = undefined);

#endif
