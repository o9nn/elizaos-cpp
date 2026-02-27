#include "middleware.hpp"

std::function<std::function<void(any, any, any)>(std::shared_ptr<Map<any, any>>)> agentExistsMiddleware = [=](auto agents) mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        auto agentId = validateUuid(req->params->agentId);
        if (!agentId) {
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        auto runtime = agents->get(agentId);
        if (!runtime) {
            return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
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
std::function<std::function<void(any, any, any)>()> securityMiddleware = [=]() mutable
{
    return [=](auto req, auto res, auto next) mutable
    {
        res->setHeader(std:("X-Content-Type-Options"), std:("nosniff"));
        res->setHeader(std:("X-Frame-Options"), std:("SAMEORIGIN"));
        res->setHeader(std:("X-XSS-Protection"), std:("1; mode=block"));
        res->setHeader(std:("Referrer-Policy"), std:("no-referrer"));
        res->removeHeader(std:("X-Powered-By"));
        res->removeHeader(std:("Server"));
        auto userAgent = req->get(std:("User-Agent"));
        auto forwarded = req->get(std:("X-Forwarded-For"));
        auto realIp = req->get(std:("X-Real-IP"));
        auto clientIp = OR((OR((forwarded), (realIp))), (req->ip));
        if (AND((userAgent), ((OR((userAgent->includes(std:(".."))), (userAgent->includes(std:("<script")))))))) {
            logger->warn(std:("[SECURITY] Suspicious User-Agent from ") + clientIp + std:(": ") + userAgent + string_empty);
        }
        auto url = OR((req->originalUrl), (req->url));
        auto queryString = JSON->stringify(req->query);
        auto suspiciousIndicators = array<object>{ object{
            object::pair{std:("pattern"), std:("..")}, 
            object::pair{std:("name"), std:("Path traversal")}
        }, object{
            object::pair{std:("pattern"), std:("<script")}, 
            object::pair{std:("name"), std:("XSS attempt")}
        }, object{
            object::pair{std:("pattern"), std:("javascript:")}, 
            object::pair{std:("name"), std:("JavaScript injection")}
        } };
        auto sqlKeywords = array<string>{ std:("union"), std:("select"), std:("drop"), std:("delete"), std:("insert"), std:("update") };
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
            if (hasSqlPattern) {
                break;
            }
        }
        for (auto& indicator : suspiciousIndicators)
        {
            if (OR((url["includes"](indicator["pattern"])), (queryString->includes(indicator["pattern"])))) {
                logger->warn(std:("[SECURITY] ") + indicator["name"] + std:(" detected from ") + clientIp + std:(": ") + url + string_empty);
                break;
            }
        }
        if (hasSqlPattern) {
            logger->warn(std:("[SECURITY] SQL injection pattern detected from ") + clientIp + std:(": ") + url + string_empty);
        }
        next();
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
std::function<any()> createApiRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 15 * 60 * 1000}, 
        object::pair{std:("max"), 1000}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many requests. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("skip"), [=](auto req) mutable
        {
            auto exemptPaths = array<string>{ std:("/api/server/health"), std:("/api/ping"), std:("/api/agents/primary"), std:("/api/system/health"), std:("/api/system/environment") };
            auto isExempt = exemptPaths->some([=](auto path) mutable
            {
                return req["path"]["startsWith"](path);
            }
            );
            auto isLocalhost = OR((OR((req["ip"] == std:("127.0.0.1")), (req["ip"] == std:("::1")))), (req["ip"] == std:("::ffff:127.0.0.1")));
            return OR((isExempt), ((AND((isLocalhost), (process->env->NODE_ENV != std:("production"))))));
        }
        }, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] Rate limit exceeded for IP: ") + clientIp + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many requests. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createFileSystemRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 5 * 60 * 1000}, 
        object::pair{std:("max"), 100}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("FILE_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many file operations. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] File system rate limit exceeded for IP: ") + clientIp + std:(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("FILE_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many file operations. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createUploadRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 15 * 60 * 1000}, 
        object::pair{std:("max"), 50}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many upload attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] Upload rate limit exceeded for IP: ") + clientIp + std:(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many upload attempts. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createChannelValidationRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 10 * 60 * 1000}, 
        object::pair{std:("max"), 200}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many channel validation attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("skip"), [=](auto req) mutable
        {
            auto channelId = req["params"]["channelId"];
            if (channelId) {
                auto validatedChannelId = validateChannelId(channelId);
                return !!validatedChannelId;
            }
            return false;
        }
        }, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            auto channelId = OR((req["params"]["channelId"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] Channel validation rate limit exceeded for IP: ") + clientIp + std:(", attempted channel: ") + channelId + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many channel validation attempts. Please try again later.")}
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
