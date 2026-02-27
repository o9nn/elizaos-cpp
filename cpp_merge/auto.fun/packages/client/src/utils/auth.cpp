#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/utils/auth.h"

std::function<string(string)> sanitizeToken = [=](auto token) mutable
{
    if (!token) return nullptr;
    if (OR((token == std:("null")), (token == std:("undefined")))) {
        return nullptr;
    }
    if (AND((token->startsWith(std:("""))), (token->endsWith(std:("""))))) {
        return token->slice(1, -1);
    }
    return token;
};
std::function<string()> getAuthToken = [=]() mutable
{
    auto authToken = localStorage->getItem(std:("authToken"));
    return sanitizeToken(authToken);
};
std::function<any(string)> parseJwt = [=](auto token) mutable
{
    if (OR((OR((!token), (type_of(token) != std:("string")))), (token->split(std:("."))->get_length() != 3))) {
        console->warn(std:("Invalid JWT structure passed to parseJwt"));
        return nullptr;
    }
    try
    {
        auto base64Url = const_(token->split(std:(".")))[1];
        if (!base64Url) {
            console->error(std:("Invalid JWT: Missing payload part."));
            return nullptr;
        }
        auto base64 = base64Url->replace((new RegExp(std:("-"))), std:("+"))->replace((new RegExp(std:("_"))), std:("/"));
        auto jsonPayload = decodeURIComponent(atob(base64)->split(string_empty)->map([=](auto c) mutable
        {
            return std:("%") + (std:("00") + c->charCodeAt(0)->toString(16))->slice(-2);
        }
        )->join(string_empty));
        return JSON->parse(jsonPayload);
    }
    catch (const any& error)
    {
        console->error(std:("Error parsing JWT:"), error);
        return nullptr;
    }
};
std::function<boolean(string)> isTokenExpired = [=](auto token) mutable
{
    if (OR((OR((!token), (type_of(token) != std:("string")))), (token->split(std:("."))->get_length() != 3))) {
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
        console->error(std:("Error checking token expiration:"), error);
        return true;
    }
};

void Main(void)
{
}

MAIN
