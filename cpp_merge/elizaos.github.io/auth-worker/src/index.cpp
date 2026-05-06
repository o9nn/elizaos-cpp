#include "index.hpp"
#include <string>

std::shared_ptr<Promise<std::shared_ptr<Response>>> handleCallback(std::shared_ptr<Request> request, std::shared_ptr<Env> env)
{
    auto url = std::make_shared<URL>(request->url);
    auto code = url->searchParams->get(std::string("code"));
    if (!code) {
        return std::make_shared<Response>(JSON->stringify(object{
            object::pair{std::string("error"), std::string("Missing authorization code")}
        }), object{
            object::pair{std::string("status"), 400}, 
            object::pair{std::string("headers"), utils::assign(object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }, getCorsHeaders(env))}
        });
    }
    try
    {
        auto tokenResponse = std::async([=]() { fetch(std::string("https://github.com/login/oauth/access_token"), object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}, 
                object::pair{std::string("Accept"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("client_id"), env->GITHUB_CLIENT_ID}, 
                object::pair{std::string("client_secret"), env->GITHUB_CLIENT_SECRET}, 
                object::pair{std::string("code"), std::string("code")}
            })}
        }); });
        if (!tokenResponse->ok) {
            throw any(std::make_shared<Error>(std::string("GitHub token exchange failed: ") + tokenResponse->status + string_empty));
        }
        auto tokenData = as<object>((std::async([=]() { tokenResponse->json(); })));
        console->log(std::string("Token data:"), tokenData);
        if (tokenData["error"]) {
            return std::make_shared<Response>(JSON->stringify(object{
                object::pair{std::string("error"), tokenData["error"]}, 
                object::pair{std::string("description"), tokenData["error_description"]}
            }), object{
                object::pair{std::string("status"), 400}, 
                object::pair{std::string("headers"), utils::assign(object{
                    object::pair{std::string("Content-Type"), std::string("application/json")}
                }, getCorsHeaders(env))}
            });
        }
        if (!tokenData["scope"]->includes(std::string("read:user"))) {
            return std::make_shared<Response>(JSON->stringify(object{
                object::pair{std::string("error"), std::string("Insufficient permissions. The 'read:user' scope is required.")}
            }), object{
                object::pair{std::string("status"), 403}, 
                object::pair{std::string("headers"), utils::assign(object{
                    object::pair{std::string("Content-Type"), std::string("application/json")}
                }, getCorsHeaders(env))}
            });
        }
        auto expiresInMilliseconds = 3 * 60 * 60 * 1000;
        auto expiresAt = Date->now() + expiresInMilliseconds;
        return std::make_shared<Response>(JSON->stringify(object{
            object::pair{std::string("access_token"), tokenData["access_token"]}, 
            object::pair{std::string("token_type"), tokenData["token_type"]}, 
            object::pair{std::string("scope"), tokenData["scope"]}, 
            object::pair{std::string("expires_at"), expiresAt}
        }), object{
            object::pair{std::string("status"), 200}, 
            object::pair{std::string("headers"), utils::assign(object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }, getCorsHeaders(env))}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("Error exchanging code for token:"), error);
        return std::make_shared<Response>(JSON->stringify(object{
            object::pair{std::string("error"), std::string("Failed to exchange authorization code for access token")}
        }), object{
            object::pair{std::string("status"), 500}, 
            object::pair{std::string("headers"), utils::assign(object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }, getCorsHeaders(env))}
        });
    }
};


std::shared_ptr<Response> handleStatus(std::shared_ptr<Env> env)
{
    return std::make_shared<Response>(JSON->stringify(object{
        object::pair{std::string("status"), std::string("ok")}
    }), object{
        object::pair{std::string("status"), 200}, 
        object::pair{std::string("headers"), utils::assign(object{
            object::pair{std::string("Content-Type"), std::string("application/json")}
        }, getCorsHeaders(env))}
    });
};


std::shared_ptr<Response> handleCors(std::shared_ptr<Request> request, std::shared_ptr<Env> env)
{
    return std::make_shared<Response>(nullptr, object{
        object::pair{std::string("status"), 204}, 
        object::pair{std::string("headers"), utils::assign(object{
            , 
            object::pair{std::string("Access-Control-Allow-Methods"), std::string("GET, POST, OPTIONS")}, 
            object::pair{std::string("Access-Control-Allow-Headers"), std::string("Content-Type, Authorization")}, 
            object::pair{std::string("Access-Control-Max-Age"), std::string("86400")}
        }, getCorsHeaders(env))}
    });
};


object worker = object{
};

void Main(void)
{
}

MAIN
