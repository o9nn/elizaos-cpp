#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_EXPERIENCE_EVALUATORS_EXPERIENCEEVALUATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_EXPERIENCE_EVALUATORS_EXPERIENCEEVALUATOR_H
#include "core.hpp"
// External dependency removed
#include "../service.h"
#include "../types.h"

extern std::shared_ptr<Evaluator> experienceEvaluator;
string extractContext(array<std::shared_ptr<Memory>> messages);

string extractAction(string text);

string extractError(string text);

string extractDiscovery(string text);

string extractLearning(string text, string type);

string extractHypothesis(string text);

string detectDomain(string text);

#endif
