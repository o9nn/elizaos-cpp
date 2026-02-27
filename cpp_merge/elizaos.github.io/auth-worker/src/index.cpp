#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/auth-worker/src/index.h"

std::shared_ptr<Promise<std::shared_ptr<Response>>> handleCallback(std::shared_ptr<Request> request, std::shared_ptr<Env> env)
{
    auto url = std::make_shared<URL>(request->url);
    auto code = url->searchParams->get(std:("code"));
    if (!code) {
        return std::make_shared<Response>(JSON->stringify(object{
            object::pair{std:("error"), std:("Missing authorization code")}
        }), object{
            object::pair{std:("status"), 400}, 
            object::pair{std:("headers"), utils::assign(object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }, getCorsHeaders(env))}
        });
    }
    try
    {
        auto tokenResponse = std::async([=]() { fetch(std:("https://github.com/login/oauth/access_token"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}, 
                object::pair{std:("Accept"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("client_id"), env->GITHUB_CLIENT_ID}, 
                object::pair{std:("client_secret"), env->GITHUB_CLIENT_SECRET}, 
                object::pair{std:("code"), std:("code")}
            })}
        }); });
        if (!tokenResponse->ok) {
            throw any(std::make_shared<Error>(std:("GitHub token exchange failed: ") + tokenResponse->status + string_empty));
        }
        auto tokenData = as<object>((std::async([=]() { tokenResponse->json(); })));
        console->log(std:("Token data:"), tokenData);
        if (tokenData["error"]) {
            return std::make_shared<Response>(JSON->stringify(object{
                object::pair{std:("error"), tokenData["error"]}, 
                object::pair{std:("description"), tokenData["error_description"]}
            }), object{
                object::pair{std:("status"), 400}, 
                object::pair{std:("headers"), utils::assign(object{
                    object::pair{std:("Content-Type"), std:("application/json")}
                }, getCorsHeaders(env))}
            });
        }
        if (!tokenData["scope"]->includes(std:("read:user"))) {
            return std::make_shared<Response>(JSON->stringify(object{
                object::pair{std:("error"), std:("Insufficient permissions. The 'read:user' scope is required.")}
            }), object{
                object::pair{std:("status"), 403}, 
                object::pair{std:("headers"), utils::assign(object{
                    object::pair{std:("Content-Type"), std:("application/json")}
                }, getCorsHeaders(env))}
            });
        }
        auto expiresInMilliseconds = 3 * 60 * 60 * 1000;
        auto expiresAt = Date->now() + expiresInMilliseconds;
        return std::make_shared<Response>(JSON->stringify(object{
            object::pair{std:("access_token"), tokenData["access_token"]}, 
            object::pair{std:("token_type"), tokenData["token_type"]}, 
            object::pair{std:("scope"), tokenData["scope"]}, 
            object::pair{std:("expires_at"), expiresAt}
        }), object{
            object::pair{std:("status"), 200}, 
            object::pair{std:("headers"), utils::assign(object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }, getCorsHeaders(env))}
        });
    }
    catch (const any& error)
    {
        console->error(std:("Error exchanging code for token:"), error);
        return std::make_shared<Response>(JSON->stringify(object{
            object::pair{std:("error"), std:("Failed to exchange authorization code for access token")}
        }), object{
            object::pair{std:("status"), 500}, 
            object::pair{std:("headers"), utils::assign(object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }, getCorsHeaders(env))}
        });
    }
};


std::shared_ptr<Response> handleStatus(std::shared_ptr<Env> env)
{
    return std::make_shared<Response>(JSON->stringify(object{
        object::pair{std:("status"), std:("ok")}
    }), object{
        object::pair{std:("status"), 200}, 
        object::pair{std:("headers"), utils::assign(object{
            object::pair{std:("Content-Type"), std:("application/json")}
        }, getCorsHeaders(env))}
    });
};


std::shared_ptr<Response> handleCors(std::shared_ptr<Request> request, std::shared_ptr<Env> env)
{
    return std::make_shared<Response>(nullptr, object{
        object::pair{std:("status"), 204}, 
        object::pair{std:("headers"), utils::assign(object{
            , 
            object::pair{std:("Access-Control-Allow-Methods"), std:("GET, POST, OPTIONS")}, 
            object::pair{std:("Access-Control-Allow-Headers"), std:("Content-Type, Authorization")}, 
            object::pair{std:("Access-Control-Max-Age"), std:("86400")}
        }, getCorsHeaders(env))}
    });
};


object worker = object{
};

void Main(void)
{
}

MAIN
