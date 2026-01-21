#include "classified/packages/core/src/types/post.h"

std::any IPostService::override;

IPostService::IPostService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

