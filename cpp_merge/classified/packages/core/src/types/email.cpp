#include "email.hpp"

any IEmailService::override;

IEmailService::IEmailService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

