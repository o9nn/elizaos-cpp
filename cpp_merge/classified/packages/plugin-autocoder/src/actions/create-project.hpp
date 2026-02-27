#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC_ACTIONS_CREATE_PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC_ACTIONS_CREATE_PROJECT_H
#include "core.hpp"
#include <string>
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "../types/index.h"
// External dependency removed

any extractProjectName(string text);

template <typename RET>
RET extractProjectType(string text);

any getFormTemplate(string projectType);

extern std::shared_ptr<Action> createProjectAction;
template <typename RET>
RET extractProjectType(string text)
{
    auto lower = text->toLowerCase();
    if (lower->includes(std::string("plugin"))) {
        return std::string("plugin");
    }
    if (OR((lower->includes(std::string("agent"))), (lower->includes(std::string("bot"))))) {
        return std::string("agent");
    }
    if (OR((lower->includes(std::string("workflow"))), (lower->includes(std::string("flow"))))) {
        return std::string("workflow");
    }
    if (OR((lower->includes(std::string("mcp"))), (lower->includes(std::string("model context"))))) {
        return std::string("mcp");
    }
    if (OR((lower->includes(std::string("app"))), (lower->includes(std::string("application"))))) {
        return std::string("full-stack");
    }
    return std::string("plugin");
};


#endif
