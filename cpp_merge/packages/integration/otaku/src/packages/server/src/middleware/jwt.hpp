#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_JWT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_JWT_H
#include "core.h"
#include "jsonwebtoken.h"
using jwt = _default;
#include "express.h"
#include "@elizaos/core.h"

class AuthTokenPayload;
class AuthenticatedRequest;

extern string JWT_SECRET;
class AuthTokenPayload : public object, public std::enable_shared_from_this<AuthTokenPayload> {
public:
    using std::enable_shared_from_this<AuthTokenPayload>::shared_from_this;
    string userId;

    string email;

    string username;

    boolean isAdmin;

    double iat;

    double exp;
};

class AuthenticatedRequest : public Request, public std::enable_shared_from_this<AuthenticatedRequest> {
public:
    using std::enable_shared_from_this<AuthenticatedRequest>::shared_from_this;
    string userId;

    string userEmail;

    string username;

    boolean isAdmin;

    boolean isServerAuthenticated;
};

string generateAuthToken(string userId, string email, string username, boolean isAdmin = undefined);

any requireAuth(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next);

any optionalAuth(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<NextFunction> next);

any requireAuthOrApiKey(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next);

any requireAdmin(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next);

#endif
