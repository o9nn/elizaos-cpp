#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/middleware/security.h"

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

void Main(void)
{
}

MAIN
