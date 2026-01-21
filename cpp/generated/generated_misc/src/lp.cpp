#include "eliza/packages/core/src/types/lp.h"

std::any ILpService::override;

ILpService::ILpService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

