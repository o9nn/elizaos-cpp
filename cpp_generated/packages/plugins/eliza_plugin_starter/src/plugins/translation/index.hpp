#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_TRANSLATION_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_TRANSLATION_INDEX_H
#include "core.h"
#include "@ai16z/eliza.h"
#include "./action.ts.h"
#include "./evaluator.ts.h"
#include "./provider/index.ts.h"
#include "./service.ts.h"
#include "./types.ts.h"

extern std::shared_ptr<Plugin> translationPlugin;
extern std::function<void(any)> initializeTranslationPlugin;
#endif
