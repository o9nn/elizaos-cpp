#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_LIAISON_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_LIAISON_INDEX_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "@elizaos/core.h"
#include "dotenv.h"
using dotenv = _default;
#include "../init.h"

extern any imagePath;
extern string avatar;
extern std::shared_ptr<Character> character;
extern std::shared_ptr<OnboardingConfig> config;
extern std::shared_ptr<ProjectAgent> liaison;
#endif
