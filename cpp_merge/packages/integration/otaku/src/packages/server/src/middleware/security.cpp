#include "security.hpp"

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
            if (hasSqlPattern) break;
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

void Main(void)
{
}

MAIN
