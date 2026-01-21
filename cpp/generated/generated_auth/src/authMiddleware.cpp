#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/authMiddleware.h"

any apiKeyAuthMiddleware(std::shared_ptr<Request> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    auto serverAuthToken = process->env->ELIZA_SERVER_AUTH_TOKEN;
    if (!serverAuthToken) {
        return next();
    }
    if (req->method == std::string("OPTIONS")) {
        return next();
    }
    auto apiKey = const_(req->headers)[std::string("x-api-key")];
    if (OR((!apiKey), (apiKey != serverAuthToken))) {
        logger->warn(std::string("Unauthorized access attempt: Missing or invalid X-API-KEY from ") + req->ip + string_empty);
        return res->status(401)->send(std::string("Unauthorized: Invalid or missing X-API-KEY"));
    }
    next();
};


