#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_EVALUATORS_EXPERIENCEEVALUATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_EVALUATORS_EXPERIENCEEVALUATOR_H
#include "core.h"
#include "@elizaos/core.h"
#include "../service.h"
#include "../types.h"

extern std::shared_ptr<Evaluator> experienceEvaluator;
std::string extractContext(array<std::shared_ptr<Memory>> messages);

std::string extractAction(std::string text);

std::string extractError(std::string text);

std::string extractDiscovery(std::string text);

std::string extractLearning(std::string text, std::string type);

std::string extractHypothesis(std::string text);

std::string detectDomain(std::string text);

#endif
