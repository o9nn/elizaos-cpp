#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/middleware/jwt.h"

string generateAuthToken(string userId, string email, string username, boolean isAdmin)
{
    if (!JWT_SECRET) {
        throw any(std::make_shared<Error>(std::string("JWT_SECRET not configured")));
    }
    auto adminEmails = OR((process->env->ADMIN_EMAILS->split(std::string(","))->map([=](auto e) mutable
    {
        return e->trim()->toLowerCase();
    }
    )), (array<any>()));
    auto computedIsAdmin = OR((isAdmin), (adminEmails->includes(email->toLowerCase())));
    auto payload = utils::assign(object{
        object::pair{std::string("userId"), std::string("userId")}, 
        object::pair{std::string("email"), std::string("email")}, 
        object::pair{std::string("username"), std::string("username")}
    }, (AND((computedIsAdmin), (object{
        object::pair{std::string("isAdmin"), true}
    }))));
    return jwt->sign(payload, JWT_SECRET, object{
        object::pair{std::string("expiresIn"), std::string("7d")}
    });
};


any requireAuth(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    if (!JWT_SECRET) {
        logger->error(std::string("[Auth] JWT_SECRET not configured - cannot verify tokens"));
        return res->status(500)->json(object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("SERVER_MISCONFIGURED")}, 
                object::pair{std::string("message"), std::string("Authentication system not properly configured")}
            }}
        });
    }
    auto authHeader = req->headers->authorization;
    if (OR((!authHeader), (!authHeader->startsWith(std::string("Bearer "))))) {
        return res->status(401)->json(object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("UNAUTHORIZED")}, 
                object::pair{std::string("message"), std::string("Authentication required. Please provide a valid Bearer token.")}
            }}
        });
    }
    auto token = authHeader->substring(7);
    try
    {
        auto decoded = as<std::shared_ptr<AuthTokenPayload>>(jwt->verify(token, JWT_SECRET));
        req->userId = decoded->userId;
        req->userEmail = decoded->email;
        req->username = decoded->username;
        req->isAdmin = OR((decoded->isAdmin), (false));
        logger->debug(std::string("[Auth] Authenticated request from user: ") + decoded->username + std::string(" (") + decoded->userId->substring(0, 8) + std::string("...)") + (req->isAdmin) ? std::string(" [ADMIN]") : string_empty + string_empty);
        next();
    }
    catch (const any& error)
    {
        logger->warn(std::string("[Auth] Token verification failed: ") + error["message"] + string_empty);
        if (error["name"] == std::string("TokenExpiredError")) {
            return res->status(401)->json(object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("TOKEN_EXPIRED")}, 
                    object::pair{std::string("message"), std::string("Authentication token has expired. Please sign in again.")}
                }}
            });
        }
        return res->status(401)->json(object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("INVALID_TOKEN")}, 
                object::pair{std::string("message"), std::string("Invalid authentication token.")}
            }}
        });
    }
};


any optionalAuth(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<NextFunction> next)
{
    if (!JWT_SECRET) {
        return next();
    }
    auto authHeader = req->headers->authorization;
    if (OR((!authHeader), (!authHeader->startsWith(std::string("Bearer "))))) {
        return next();
    }
    auto token = authHeader->substring(7);
    try
    {
        auto decoded = as<std::shared_ptr<AuthTokenPayload>>(jwt->verify(token, JWT_SECRET));
        req->userId = decoded->userId;
        req->userEmail = decoded->email;
        req->username = decoded->username;
        req->isAdmin = OR((decoded->isAdmin), (false));
    }
    catch (const any& error)
    {
        logger->debug(std::string("[Auth] Optional auth - invalid token ignored"));
    }
    next();
};


any requireAuthOrApiKey(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    auto authHeader = req->headers->authorization;
    auto serverAuthToken = process->env->ELIZA_SERVER_AUTH_TOKEN;
    if (AND((authHeader), (authHeader->startsWith(std::string("Bearer "))))) {
        if (!JWT_SECRET) {
            logger->error(std::string("[Auth] JWT_SECRET not configured - cannot verify tokens"));
            return res->status(500)->json(object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("SERVER_MISCONFIGURED")}, 
                    object::pair{std::string("message"), std::string("Authentication system not properly configured")}
                }}
            });
        }
        auto token = authHeader->substring(7);
        try
        {
            auto decoded = as<std::shared_ptr<AuthTokenPayload>>(jwt->verify(token, JWT_SECRET));
            req->userId = decoded->userId;
            req->userEmail = decoded->email;
            req->username = decoded->username;
            req->isAdmin = OR((decoded->isAdmin), (false));
            logger->debug(std::string("[Auth] Authenticated via JWT: ") + decoded->username + std::string(" (") + decoded->userId->substring(0, 8) + std::string("...)") + (req->isAdmin) ? std::string(" [ADMIN]") : string_empty + string_empty);
            return next();
        }
        catch (const any& error)
        {
            logger->warn(std::string("[Auth] JWT verification failed in requireAuthOrApiKey: ") + error["message"] + string_empty);
        }
    }
    auto apiKey = OR(((as<any>(const_(req->headers)[std::string("x-api-key")]))), (undefined));
    if (AND((AND((serverAuthToken), (apiKey))), (apiKey == serverAuthToken))) {
        req->isServerAuthenticated = true;
        logger->debug(std::string("[Auth] Authenticated via X-API-KEY (server)"));
        return next();
    }
    return res->status(401)->json(object{
        object::pair{std::string("success"), false}, 
        object::pair{std::string("error"), object{
            object::pair{std::string("code"), std::string("UNAUTHORIZED")}, 
            object::pair{std::string("message"), std::string("Authentication required (Bearer token or X-API-KEY).")}
        }}
    });
};


any requireAdmin(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    if (!req->isAdmin) {
        logger->warn(std::string("[Auth] Non-admin user ") + req->username + std::string(" (") + req->userId->substring(0, 8) + std::string("...) attempted admin operation"));
        return res->status(403)->json(object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("FORBIDDEN")}, 
                object::pair{std::string("message"), std::string("Administrator privileges required for this operation")}
            }}
        });
    }
    next();
};


string JWT_SECRET = process->env->JWT_SECRET;

void Main(void)
{
    if (!JWT_SECRET) {
        logger->warn(std::string("[Auth] JWT_SECRET not set - authentication will not work. Set JWT_SECRET environment variable."));
    }
}

MAIN
