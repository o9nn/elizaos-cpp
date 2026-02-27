#include "message.hpp"

any IMessageService::override;

IMessageService::IMessageService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

