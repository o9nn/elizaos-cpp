#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/utils/auth.h"

std::function<string(string)> sanitizeToken = [=](auto token) mutable
{
    if (!token) return nullptr;
    if (OR((token == std::string("null")), (token == std::string("undefined")))) {
        return nullptr;
    }
    if (AND((token->startsWith(std::string("""))), (token->endsWith(std::string("""))))) {
        return token->slice(1, -1);
    }
    return token;
};
std::function<string()> getAuthToken = [=]() mutable
{
    auto authToken = localStorage->getItem(std::string("authToken"));
    return sanitizeToken(authToken);
};
std::function<any(string)> parseJwt = [=](auto token) mutable
{
    if (OR((OR((!token), (type_of(token) != std::string("string")))), (token->split(std::string("."))->get_length() != 3))) {
        console->warn(std::string("Invalid JWT structure passed to parseJwt"));
        return nullptr;
    }
    try
    {
        auto base64Url = const_(token->split(std::string(".")))[1];
        if (!base64Url) {
            console->error(std::string("Invalid JWT: Missing payload part."));
            return nullptr;
        }
        auto base64 = base64Url->replace((new RegExp(std::string("-"))), std::string("+"))->replace((new RegExp(std::string("_"))), std::string("/"));
        auto jsonPayload = decodeURIComponent(atob(base64)->split(string_empty)->map([=](auto c) mutable
        {
            return std::string("%") + (std::string("00") + c->charCodeAt(0)->toString(16))->slice(-2);
        }
        )->join(string_empty));
        return JSON->parse(jsonPayload);
    }
    catch (const any& error)
    {
        console->error(std::string("Error parsing JWT:"), error);
        return nullptr;
    }
};
std::function<boolean(string)> isTokenExpired = [=](auto token) mutable
{
    if (OR((OR((!token), (type_of(token) != std::string("string")))), (token->split(std::string("."))->get_length() != 3))) {
        return true;
    }
    try
    {
        auto payload = parseJwt(token);
        if (OR((!payload), (!payload["exp"]))) return true;
        return Date->now() >= payload["exp"] * 1000;
    }
    catch (const any& error)
    {
        console->error(std::string("Error checking token expiration:"), error);
        return true;
    }
};

void Main(void)
{
}

MAIN
