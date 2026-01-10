#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_GENERAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_GENERAL_H
#include "core.h"
#include "./types.h"

extern array<std::shared_ptr<CodingRule>> PYTHON_CODING_RULES;
extern array<std::shared_ptr<CodingRule>> TYPESCRIPT_CODING_RULES;
extern std::shared_ptr<CodingGuidelines> GENERAL_CODING_GUIDELINES;
extern std::shared_ptr<CodingGuidelines> TYPESCRIPT_CODING_GUIDELINES;
object validateAgainstRules(string code, std::shared_ptr<CodingGuidelines> guidelines);

template <typename P1>
array<std::shared_ptr<CodingRule>> getApplicableRules(string filePath, P1 language = undefined);

template <typename P1>
array<std::shared_ptr<CodingRule>> getApplicableRules(string filePath, P1 language)
{
    auto lang = OR((language), (((filePath->endsWith(std::string(".py"))) ? std::string("python") : std::string("typescript"))));
    return (lang == std::string("python")) ? PYTHON_CODING_RULES : TYPESCRIPT_CODING_RULES;
};


#endif
