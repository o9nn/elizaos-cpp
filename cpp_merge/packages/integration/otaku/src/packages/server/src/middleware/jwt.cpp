#include "jwt.hpp"

string generateAuthToken(string userId, string email, string username, boolean isAdmin)
{
    if (!JWT_SECRET) {
        throw any(std::make_shared<Error>(std:("JWT_SECRET not configured")));
    }
    auto adminEmails = OR((process->env->ADMIN_EMAILS->split(std:(","))->map([=](auto e) mutable
    {
        return e->trim()->toLowerCase();
    }
    )), (array<any>()));
    auto computedIsAdmin = OR((isAdmin), (adminEmails->includes(email->toLowerCase())));
    auto payload = utils::assign(object{
        object::pair{std:("userId"), std:("userId")}, 
        object::pair{std:("email"), std:("email")}, 
        object::pair{std:("username"), std:("username")}
    }, (AND((computedIsAdmin), (object{
        object::pair{std:("isAdmin"), true}
    }))));
    return jwt->sign(payload, JWT_SECRET, object{
        object::pair{std:("expiresIn"), std:("7d")}
    });
};


any requireAuth(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    if (!JWT_SECRET) {
        logger->error(std:("[Auth] JWT_SECRET not configured - cannot verify tokens"));
        return res->status(500)->json(object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("SERVER_MISCONFIGURED")}, 
                object::pair{std:("message"), std:("Authentication system not properly configured")}
            }}
        });
    }
    auto authHeader = req->headers->authorization;
    if (OR((!authHeader), (!authHeader->startsWith(std:("Bearer "))))) {
        return res->status(401)->json(object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("UNAUTHORIZED")}, 
                object::pair{std:("message"), std:("Authentication required. Please provide a valid Bearer token.")}
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
        logger->debug(std:("[Auth] Authenticated request from user: ") + decoded->username + std:(" (") + decoded->userId->substring(0, 8) + std:("...)") + (req->isAdmin) ? std:(" [ADMIN]") : string_empty + string_empty);
        next();
    }
    catch (const any& error)
    {
        logger->warn(std:("[Auth] Token verification failed: ") + error["message"] + string_empty);
        if (error["name"] == std:("TokenExpiredError")) {
            return res->status(401)->json(object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("TOKEN_EXPIRED")}, 
                    object::pair{std:("message"), std:("Authentication token has expired. Please sign in again.")}
                }}
            });
        }
        return res->status(401)->json(object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("INVALID_TOKEN")}, 
                object::pair{std:("message"), std:("Invalid authentication token.")}
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
    if (OR((!authHeader), (!authHeader->startsWith(std:("Bearer "))))) {
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
        logger->debug(std:("[Auth] Optional auth - invalid token ignored"));
    }
    next();
};


any requireAuthOrApiKey(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    auto authHeader = req->headers->authorization;
    auto serverAuthToken = process->env->ELIZA_SERVER_AUTH_TOKEN;
    if (AND((authHeader), (authHeader->startsWith(std:("Bearer "))))) {
        if (!JWT_SECRET) {
            logger->error(std:("[Auth] JWT_SECRET not configured - cannot verify tokens"));
            return res->status(500)->json(object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("SERVER_MISCONFIGURED")}, 
                    object::pair{std:("message"), std:("Authentication system not properly configured")}
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
            logger->debug(std:("[Auth] Authenticated via JWT: ") + decoded->username + std:(" (") + decoded->userId->substring(0, 8) + std:("...)") + (req->isAdmin) ? std:(" [ADMIN]") : string_empty + string_empty);
            return next();
        }
        catch (const any& error)
        {
            logger->warn(std:("[Auth] JWT verification failed in requireAuthOrApiKey: ") + error["message"] + string_empty);
        }
    }
    auto apiKey = OR(((as<any>(const_(req->headers)[std:("x-api-key")]))), (undefined));
    if (AND((AND((serverAuthToken), (apiKey))), (apiKey == serverAuthToken))) {
        req->isServerAuthenticated = true;
        logger->debug(std:("[Auth] Authenticated via X-API-KEY (server)"));
        return next();
    }
    return res->status(401)->json(object{
        object::pair{std:("success"), false}, 
        object::pair{std:("error"), object{
            object::pair{std:("code"), std:("UNAUTHORIZED")}, 
            object::pair{std:("message"), std:("Authentication required (Bearer token or X-API-KEY).")}
        }}
    });
};


any requireAdmin(std::shared_ptr<AuthenticatedRequest> req, std::shared_ptr<Response> res, std::shared_ptr<NextFunction> next)
{
    if (!req->isAdmin) {
        logger->warn(std:("[Auth] Non-admin user ") + req->username + std:(" (") + req->userId->substring(0, 8) + std:("...) attempted admin operation"));
        return res->status(403)->json(object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("FORBIDDEN")}, 
                object::pair{std:("message"), std:("Administrator privileges required for this operation")}
            }}
        });
    }
    next();
};


string JWT_SECRET = process->env->JWT_SECRET;

void Main(void)
{
    if (!JWT_SECRET) {
        logger->warn(std:("[Auth] JWT_SECRET not set - authentication will not work. Set JWT_SECRET environment variable."));
    }
}

MAIN
