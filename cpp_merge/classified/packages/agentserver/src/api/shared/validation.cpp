#include "validation.hpp"
#include <string>

std::function<any(std::shared_ptr<Map<any, any>>, any)> getRuntime = [=](auto agents, auto agentId) mutable
{
    auto runtime = agents->get(agentId);
    if (!runtime) {
        throw any(std::make_shared<Error>(std::string("Agent not found: ") + agentId + string_empty));
    }
    return runtime;
};
std::function<any(string)> validateAgentId = [=](auto agentId) mutable
{
    return validateUuid(agentId);
};
std::function<any(string)> validateRoomId = [=](auto roomId) mutable
{
    return validateUuid(roomId);
};
std::function<any(string, string)> validateChannelId = [=](auto channelId, auto clientIp = std::nullopt) mutable
{
    auto validatedUuid = validateUuid(channelId);
    if (!validatedUuid) {
        if (clientIp) {
            logger->warn(std::string("[SECURITY] Invalid channel ID attempted from ") + clientIp + std::string(": ") + channelId + string_empty);
        }
        return nullptr;
    }
    auto suspiciousPatterns = array<string>{ std::string(".."), std::string("<"), std::string(">"), std::string("""), std::string("'"), std::string("\"), std::string("/") };
    auto hasSuspiciousPattern = suspiciousPatterns->some([=](auto pattern) mutable
    {
        return channelId->includes(pattern);
    }
    );
    if (hasSuspiciousPattern) {
        if (clientIp) {
            logger->warn(std::string("[SECURITY] Suspicious channel ID pattern from ") + clientIp + std::string(": ") + channelId + string_empty);
        }
        return nullptr;
    }
    return validatedUuid;
};
std::function<any(string)> validateMemoryId = [=](auto memoryId) mutable
{
    return validateUuid(memoryId);
};
std::function<any(string)> validateWorldId = [=](auto worldId) mutable
{
    return validateUuid(worldId);
};

void Main(void)
{
}

MAIN
