#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_CLI-PROMPTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_CLI-PROMPTS_H
#include "core.h"
#include "@clack/prompts.h"
#include "@elizaos/core.h"

extern string NAV_BACK;
extern string NAV_NEXT;
std::shared_ptr<Promise<string>> promptWithNav(string label, string initial = string_empty, std::function<any(string)> validate = nullptr);

std::shared_ptr<Promise<array<string>>> promptForMultipleItems(string fieldName, array<string> initial = array<string>());

std::shared_ptr<Promise<boolean>> confirmAction(string message);

#endif
