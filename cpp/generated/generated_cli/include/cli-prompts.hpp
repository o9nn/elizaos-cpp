#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_CLI-PROMPTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_CLI-PROMPTS_H
#include "core.h"
#include "@clack/prompts.h"
#include "@elizaos/core.h"

extern std::string NAV_BACK;
extern std::string NAV_NEXT;
std::shared_ptr<Promise<string>> promptWithNav(std::string label, std::string initial = string_empty, std::function<std::any(std::string)> validate = nullptr);

std::shared_ptr<Promise<array<string>>> promptForMultipleItems(std::string fieldName, array<string> initial = array<string>());

std::shared_ptr<Promise<boolean>> confirmAction(std::string message);

#endif
