#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_EXPERIENCE_SRC_EVALUATORS_EXPERIENCEEVALUATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_EXPERIENCE_SRC_EVALUATORS_EXPERIENCEEVALUATOR_H
#include "core.hpp"
// External dependency removed
#include "../service.h"
#include "../types.h"

extern std::shared_ptr<Evaluator> experienceEvaluator;
string sanitizeContext(string text);

string detectDomain(string text);

#endif
