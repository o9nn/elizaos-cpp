#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_CREATE-PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_CREATE-PROJECT_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "../types/index.h"
#include "@elizaos/plugin-forms.h"

any extractProjectName(string text);

template <typename RET>
RET extractProjectType(string text);

any getFormTemplate(string projectType);

extern std::shared_ptr<Action> createProjectAction;
template <typename RET>
RET extractProjectType(string text)
{
    auto lower = text->toLowerCase();
    if (lower->includes(std:("plugin"))) {
        return std:("plugin");
    }
    if (OR((lower->includes(std:("agent"))), (lower->includes(std:("bot"))))) {
        return std:("agent");
    }
    if (OR((lower->includes(std:("workflow"))), (lower->includes(std:("flow"))))) {
        return std:("workflow");
    }
    if (OR((lower->includes(std:("mcp"))), (lower->includes(std:("model context"))))) {
        return std:("mcp");
    }
    if (OR((lower->includes(std:("app"))), (lower->includes(std:("application"))))) {
        return std:("full-stack");
    }
    return std:("plugin");
};


#endif
