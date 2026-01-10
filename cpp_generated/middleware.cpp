#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/shared/middleware.h"

std::function<std::function<void(any, any, any)>(std::shared_ptr<Map<any, any>>)> agentExistsMiddleware = [=](auto agents) mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        auto agentId = validateUuid(req->params->agentId);
        if (!agentId) {
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        auto runtime = agents->get(agentId);
        if (!runtime) {
            return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found"));
        }
        (as<any>(req))["runtime"] = runtime;
        (as<any>(req))["agentId"] = agentId;
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
std::function<std::function<void(any, any, any)>()> securityMiddleware = [=]() mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        res->setHeader(std::string("X-Content-Type-Options"), std::string("nosniff"));
        res->setHeader(std::string("X-Frame-Options"), std::string("SAMEORIGIN"));
        res->setHeader(std::string("X-XSS-Protection"), std::string("1; mode=block"));
        res->setHeader(std::string("Referrer-Policy"), std::string("no-referrer"));
        res->removeHeader(std::string("X-Powered-By"));
        res->removeHeader(std::string("Server"));
        auto userAgent = req->get(std::string("User-Agent"));
        auto forwarded = req->get(std::string("X-Forwarded-For"));
        auto realIp = req->get(std::string("X-Real-IP"));
        auto clientIp = OR((OR((forwarded), (realIp))), (req->ip));
        if (AND((userAgent), ((OR((userAgent->includes(std::string(".."))), (userAgent->includes(std::string("<script")))))))) {
            logger->warn(std::string("[SECURITY] Suspicious User-Agent from ") + clientIp + std::string(": ") + userAgent + string_empty);
        }
        auto url = OR((req->originalUrl), (req->url));
        auto queryString = JSON->stringify(req->query);
        auto suspiciousIndicators = array<object>{ object{
            object::pair{std::string("pattern"), std::string("..")}, 
            object::pair{std::string("name"), std::string("Path traversal")}
        }, object{
            object::pair{std::string("pattern"), std::string("<script")}, 
            object::pair{std::string("name"), std::string("XSS attempt")}
        }, object{
            object::pair{std::string("pattern"), std::string("javascript:")}, 
            object::pair{std::string("name"), std::string("JavaScript injection")}
        } };
        auto sqlKeywords = array<string>{ std::string("union"), std::string("select"), std::string("drop"), std::string("delete"), std::string("insert"), std::string("update") };
        auto hasSqlPattern = false;
        auto lowerUrl = url["toLowerCase"]();
        auto lowerQuery = queryString->toLowerCase();
        for (auto i = 0; i < sqlKeywords->get_length() - 1; i++)
        {
            auto keyword1 = const_(sqlKeywords)[i];
            for (auto j = i + 1; j < sqlKeywords->get_length(); j++)
            {
                auto keyword2 = const_(sqlKeywords)[j];
                if (OR(((AND((lowerUrl["includes"](keyword1)), (lowerUrl["includes"](keyword2))))), ((AND((lowerQuery->includes(keyword1)), (lowerQuery->includes(keyword2))))))) {
                    hasSqlPattern = true;
                    break;
                }
            }
            if (hasSqlPattern) break;
        }
        for (auto& indicator : suspiciousIndicators)
        {
            if (OR((url["includes"](indicator["pattern"])), (queryString->includes(indicator["pattern"])))) {
                logger->warn(std::string("[SECURITY] ") + indicator["name"] + std::string(" detected from ") + clientIp + std::string(": ") + url + string_empty);
                break;
            }
        }
        if (hasSqlPattern) {
            logger->warn(std::string("[SECURITY] SQL injection pattern detected from ") + clientIp + std::string(": ") + url + string_empty);
        }
        next();
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
std::function<any()> createApiRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 15 * 60 * 1000}, 
        object::pair{std::string("max"), 1000}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many requests. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Rate limit exceeded for IP: ") + clientIp + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many requests. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createFileSystemRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 5 * 60 * 1000}, 
        object::pair{std::string("max"), 100}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("FILE_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many file operations. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] File system rate limit exceeded for IP: ") + clientIp + std::string(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("FILE_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many file operations. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createUploadRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 15 * 60 * 1000}, 
        object::pair{std::string("max"), 50}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many upload attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Upload rate limit exceeded for IP: ") + clientIp + std::string(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many upload attempts. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createChannelValidationRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 10 * 60 * 1000}, 
        object::pair{std::string("max"), 200}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many channel validation attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("skip"), [=](auto req) mutable
        {
            auto channelId = req["params"]["channelId"];
            if (channelId) {
                auto validatedChannelId = validateChannelId(channelId);
                return !!validatedChannelId;
            }
            return false;
        }
        }, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            auto channelId = OR((req["params"]["channelId"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Channel validation rate limit exceeded for IP: ") + clientIp + std::string(", attempted channel: ") + channelId + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many channel validation attempts. Please try again later.")}
                }}
            });
        }
        }
    });
};

void Main(void)
{
}

MAIN
