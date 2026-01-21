#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_FRONTEND_LIB_ACTION-MESSAGE-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_FRONTEND_LIB_ACTION-MESSAGE-UTILS_H
#include "core.h"
#include "@/components/action-tool.h"

std::shared_ptr<ToolPart> convertActionMessageToToolPart(std::any message);

boolean isActionMessage(std::any message);

#endif
