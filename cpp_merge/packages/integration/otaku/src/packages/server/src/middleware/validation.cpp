#include "validation.hpp"
#include <string>

std::function<std::function<void(any, any, any)>(any)> agentExistsMiddleware = [=](auto elizaOS) mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        auto agentId = validateUuid(req->params->agentId);
        if (!agentId) {
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        auto runtime = elizaOS->getAgent(agentId);
        if (!runtime) {
            return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found"));
        }
        (as<Record<string, any>>(as<any>(req)))->runtime = runtime;
        (as<Record<string, any>>(as<any>(req)))->agentId = agentId;
        next();
    };
};
std::function<std::function<void(any, any, any)>(string)> validateUuidMiddleware = [=](auto paramName) mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        auto paramValue = const_(req->params)[paramName];
        any validatedUuid;
        if (paramName == std::string("channelId")) {
            auto clientIp = OR((req->ip), (std::string("unknown")));
            validatedUuid = validateChannelId(paramValue, clientIp);
        } else {
            validatedUuid = validateUuid(paramValue);
        }
        if (!validatedUuid) {
            auto clientIp = OR((req->ip), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Invalid ") + paramName + std::string(" from ") + clientIp + std::string(": ") + paramValue + string_empty);
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid ") + paramName + std::string(" format"));
        }
        req->params[paramName] = validatedUuid;
        next();
    };
};
std::function<std::function<void(any, any, any)>()> validateChannelIdMiddleware = [=]() mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        auto channelId = req->params->channelId;
        auto clientIp = OR((req->ip), (std::string("unknown")));
        if (!channelId) {
            return sendError(res, 400, std::string("MISSING_CHANNEL_ID"), std::string("Channel ID is required"));
        }
        auto validatedChannelId = validateChannelId(channelId, clientIp);
        if (!validatedChannelId) {
            logger->warn(std::string("[SECURITY] Failed channel ID validation from ") + clientIp + std::string(": ") + channelId + string_empty);
            return sendError(res, 400, std::string("INVALID_CHANNEL_ID"), std::string("Invalid channel ID format"));
        }
        req->params->channelId = validatedChannelId;
        next();
    };
};
std::function<std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<AuthenticatedRequest>, any, any)>(std::function<std::shared_ptr<Promise<array<any>>>(any)>)> requireChannelParticipant = [=](auto getParticipants) mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        requireAuthOrApiKey(req, res, [=]() mutable
        {
            auto channelId = req->params->channelId;
            if (!channelId) {
                return sendError(res, 400, std::string("MISSING_CHANNEL_ID"), std::string("Channel ID is required"));
            }
            if (req->isServerAuthenticated) {
                return next();
            }
            auto userId = req->userId;
            if (!userId) {
                return sendError(res, 401, std::string("UNAUTHORIZED"), std::string("Authentication required"));
            }
            try
            {
                auto participants = std::async([=]() { getParticipants(as<std::shared_ptr<UUID>>(as<any>(channelId))); });
                if (!participants->includes(as<std::shared_ptr<UUID>>(as<any>(userId)))) {
                    return sendError(res, 403, std::string("FORBIDDEN"), std::string("You are not a participant of this channel"));
                }
                next();
            }
            catch (const any& error)
            {
                logger->error(std::string("[SECURITY] Error verifying channel participation:"), (is<Error>(error)) ? error->message : String(error));
                return sendError(res, 500, std::string("INTERNAL_ERROR"), std::string("Failed to verify channel participation"));
            }
        }
        );
    };
};
std::function<std::function<any(std::shared_ptr<AuthenticatedRequest>, any, any)>()> requireAuthenticated = [=]() mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        return requireAuthOrApiKey(req, res, next);
    };
};
std::function<std::function<any(any, any, any)>()> validateContentTypeMiddleware = [=]() mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        if ((array<string>{ std::string("POST"), std::string("PUT"), std::string("PATCH") })->includes(req->method)) {
            shared contentType = req->get(std::string("Content-Type"));
            auto contentLength = req->get(std::string("Content-Length"));
            if (OR((!contentLength), (contentLength == std::string("0")))) {
                return next();
            }
            auto validTypes = array<string>{ std::string("application/json"), std::string("multipart/form-data"), std::string("application/x-www-form-urlencoded") };
            if (OR((!contentType), (!validTypes->some([=](auto type) mutable
            {
                return contentType->includes(type);
            }
            )))) {
                return sendError(res, 400, std::string("INVALID_CONTENT_TYPE"), std::string("Invalid or missing Content-Type header"));
            }
        }
        next();
    };
};

void Main(void)
{
}

MAIN
