#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_CLI_PROMPTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_CLI_PROMPTS_H
#include "core.hpp"
// External dependency removed
// External dependency removed

extern string NAV_BACK;
extern string NAV_NEXT;
std::shared_ptr<Promise<string>> promptWithNav(string label, string initial = string_empty, std::function<any(string)> validate = nullptr);

std::shared_ptr<Promise<array<string>>> promptForMultipleItems(string fieldName, array<string> initial = array<string>());

std::shared_ptr<Promise<boolean>> confirmAction(string message);

#endif
