#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_COMMUNITYMANAGER_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_COMMUNITYMANAGER_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "dotenv.h"
using dotenv = _default;
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "../init.h"
#include "./plugins/communityManager.h"
using communityManagerPlugin = _default;

extern any imagePath;
extern string avatar;
extern std::shared_ptr<Character> character;
extern std::shared_ptr<OnboardingConfig> config;
extern std::shared_ptr<ProjectAgent> communityManager;
#endif
