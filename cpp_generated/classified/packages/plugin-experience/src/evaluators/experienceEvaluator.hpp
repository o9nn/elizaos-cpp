#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-EXPERIENCE_SRC_EVALUATORS_EXPERIENCEEVALUATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-EXPERIENCE_SRC_EVALUATORS_EXPERIENCEEVALUATOR_H
#include "core.h"
#include "@elizaos/core.h"
#include "../service.h"
#include "../types.h"

extern std::shared_ptr<Evaluator> experienceEvaluator;
string sanitizeContext(string text);

string detectDomain(string text);

#endif
