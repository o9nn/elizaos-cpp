#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./services/CodeGenerationService.h"
#include "./services/SecretsManagerService.h"
#include "./services/ProjectPlanningService.h"
#include "./services/ProjectStatusManager.h"
#include "./actions/generate-code.h"
#include "./actions/create-project.h"
#include "./providers/projects-provider.h"
#include "./providers/current-project-provider.h"
#include "./__tests__/e2e/index.h"
using testSuites = _default;

extern std::shared_ptr<Plugin> autocoderPlugin;
#endif
