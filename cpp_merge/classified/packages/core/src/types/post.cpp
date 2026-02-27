#include "post.hpp"

any IPostService::override;

IPostService::IPostService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

