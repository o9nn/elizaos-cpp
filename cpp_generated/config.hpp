#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_CONFIG_H
#include "core.h"
#include "./types.h"
#include "./general.h"
#include "./project-overview.h"

extern array<std::shared_ptr<Rule>> CURSOR_RULES;
extern std::shared_ptr<RulesConfig> RULES_CONFIG;
string exportToCursorFormat(std::shared_ptr<Rule> rule);

Record<string, string> exportAllRulesToCursor();

template <typename P0>
any getLanguageConfig(P0 language);

boolean shouldApplyRules(string filePath, std::shared_ptr<Rule> rule);

template <typename P0>
any getLanguageConfig(P0 language)
{
    return (language == std::string("python")) ? GENERAL_CODING_GUIDELINES : TYPESCRIPT_CODING_GUIDELINES;
};


#endif
