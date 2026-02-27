#include "lp.hpp"

any ILpService::override;

ILpService::ILpService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

