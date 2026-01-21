#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_CREATE-PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_CREATE-PROJECT_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "../types/index.h"
#include "@elizaos/plugin-forms.h"

std::any extractProjectName(std::string text);

template <typename RET>
RET extractProjectType(std::string text);

std::any getFormTemplate(std::string projectType);

extern std::shared_ptr<Action> createProjectAction;
template <typename RET>
RET extractProjectType(std::string text)
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
