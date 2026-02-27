#include "auth.hpp"
#include <string>

any authRouter = std::make_shared<Hono<object>>();

void Main(void)
{
    authRouter->post(std::string("/register"), [=](auto c) mutable
    {
        try
        {
            auto body = std::async([=]() { c["req"]["json"](); });
            if (OR((OR((!body["address"]), (body["address"]["length"] < 32))), (body["address"]["length"] > 44))) {
                return c["json"](object{
                    object::pair{std::string("error"), std::string("Invalid address")}
                }, 400);
            }
            auto db = getDB();
            auto existingUser = std::async([=]() { db["select"]()["from"](users)["where"](eq(users->address, body["address"]))["limit"](1); });
            any user;
            if (existingUser["length"] == 0) {
                auto userData = object{
                    object::pair{std::string("id"), crypto->randomUUID()}, 
                    object::pair{std::string("name"), OR((body["name"]), (string_empty))}, 
                    object::pair{std::string("address"), body["address"]}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>()}
                };
                std::async([=]() { db["insert"](users)["values"](userData)["onConflictDoNothing"](); });
                awardUserPoints(userData["address"], object{
                    object::pair{std::string("type"), std::string("wallet_connected")}
                }, std::string("User registered"));
                user = userData;
                logger["log"](std::string("New user registered: ") + user["address"] + string_empty);
            } else {
                user = const_(existingUser)[0];
                logger["log"](std::string("Existing user logged in: ") + user["address"] + string_empty);
            }
            return c["json"](object{
                object::pair{std::string("user"), std::string("user")}
            });
        }
        catch (const any& error)
        {
            logger["error"](std::string("Error registering user:"), error);
            return c["json"](object{
                object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) (std::string("Unknown error"))}
            }, 500);
        }
    }
    );
    authRouter->post(std::string("/authenticate"), [=](auto c) mutable
    {
        return authenticate(c);
    }
    );
    authRouter->post(std::string("/generate-nonce"), [=](auto c) mutable
    {
        return generateNonce(c);
    }
    );
    authRouter->post(std::string("/logout"), [=](auto c) mutable
    {
        return logout(c);
    }
    );
    authRouter->get(std::string("/auth-status"), [=](auto c) mutable
    {
        return authStatus(c);
    }
    );
}

MAIN
