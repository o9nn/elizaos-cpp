#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_TRANSLATION_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_TRANSLATION_SERVICE_H
#include "core.h"
#include "@ai16z/eliza.h"
#include "./types.ts.h"

extern std::shared_ptr<TranslationServiceConfig> serviceConfig;
extern std::shared_ptr<TranslationService> translationService;
extern std::function<void(any)> initializeTranslationConfig;
std::shared_ptr<Promise<object>> simulateTranslationAPI(string text, string targetLang, string sourceLang = undefined, object config = undefined);

#endif
