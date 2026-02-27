#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/middleware/validation.h"

std::function<std::function<void(any, any, any)>(any)> agentExistsMiddleware = [=](auto elizaOS) mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        auto agentId = validateUuid(req->params->agentId);
        if (!agentId) {
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        auto runtime = elizaOS->getAgent(agentId);
        if (!runtime) {
            return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
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
        if (paramName == std:("channelId")) {
            auto clientIp = OR((req->ip), (std:("unknown")));
            validatedUuid = validateChannelId(paramValue, clientIp);
        } else {
            validatedUuid = validateUuid(paramValue);
        }
        if (!validatedUuid) {
            auto clientIp = OR((req->ip), (std:("unknown")));
            logger->warn(std:("[SECURITY] Invalid ") + paramName + std:(" from ") + clientIp + std:(": ") + paramValue + string_empty);
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid ") + paramName + std:(" format"));
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
        auto clientIp = OR((req->ip), (std:("unknown")));
        if (!channelId) {
            return sendError(res, 400, std:("MISSING_CHANNEL_ID"), std:("Channel ID is required"));
        }
        auto validatedChannelId = validateChannelId(channelId, clientIp);
        if (!validatedChannelId) {
            logger->warn(std:("[SECURITY] Failed channel ID validation from ") + clientIp + std:(": ") + channelId + string_empty);
            return sendError(res, 400, std:("INVALID_CHANNEL_ID"), std:("Invalid channel ID format"));
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
                return sendError(res, 400, std:("MISSING_CHANNEL_ID"), std:("Channel ID is required"));
            }
            if (req->isServerAuthenticated) {
                return next();
            }
            auto userId = req->userId;
            if (!userId) {
                return sendError(res, 401, std:("UNAUTHORIZED"), std:("Authentication required"));
            }
            try
            {
                auto participants = std::async([=]() { getParticipants(as<std::shared_ptr<UUID>>(as<any>(channelId))); });
                if (!participants->includes(as<std::shared_ptr<UUID>>(as<any>(userId)))) {
                    return sendError(res, 403, std:("FORBIDDEN"), std:("You are not a participant of this channel"));
                }
                next();
            }
            catch (const any& error)
            {
                logger->error(std:("[SECURITY] Error verifying channel participation:"), (is<Error>(error)) ? error->message : String(error));
                return sendError(res, 500, std:("INTERNAL_ERROR"), std:("Failed to verify channel participation"));
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
        if ((array<string>{ std:("POST"), std:("PUT"), std:("PATCH") })->includes(req->method)) {
            shared contentType = req->get(std:("Content-Type"));
            auto contentLength = req->get(std:("Content-Length"));
            if (OR((!contentLength), (contentLength == std:("0")))) {
                return next();
            }
            auto validTypes = array<string>{ std:("application/json"), std:("multipart/form-data"), std:("application/x-www-form-urlencoded") };
            if (OR((!contentType), (!validTypes->some([=](auto type) mutable
            {
                return contentType->includes(type);
            }
            )))) {
                return sendError(res, 400, std:("INVALID_CONTENT_TYPE"), std:("Invalid or missing Content-Type header"));
            }
        }
        next();
    };
};

void Main(void)
{
}

MAIN
