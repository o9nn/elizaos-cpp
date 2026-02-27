#include "auth.hpp"

any authRouter = std::make_shared<Hono<object>>();

void Main(void)
{
    authRouter->post(std:("/register"), [=](auto c) mutable
    {
        try
        {
            auto body = std::async([=]() { c["req"]["json"](); });
            if (OR((OR((!body["address"]), (body["address"]["length"] < 32))), (body["address"]["length"] > 44))) {
                return c["json"](object{
                    object::pair{std:("error"), std:("Invalid address")}
                }, 400);
            }
            auto db = getDB();
            auto existingUser = std::async([=]() { db["select"]()["from"](users)["where"](eq(users->address, body["address"]))["limit"](1); });
            any user;
            if (existingUser["length"] == 0) {
                auto userData = object{
                    object::pair{std:("id"), crypto->randomUUID()}, 
                    object::pair{std:("name"), OR((body["name"]), (string_empty))}, 
                    object::pair{std:("address"), body["address"]}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>()}
                };
                std::async([=]() { db["insert"](users)["values"](userData)["onConflictDoNothing"](); });
                awardUserPoints(userData["address"], object{
                    object::pair{std:("type"), std:("wallet_connected")}
                }, std:("User registered"));
                user = userData;
                logger["log"](std:("New user registered: ") + user["address"] + string_empty);
            } else {
                user = const_(existingUser)[0];
                logger["log"](std:("Existing user logged in: ") + user["address"] + string_empty);
            }
            return c["json"](object{
                object::pair{std:("user"), std:("user")}
            });
        }
        catch (const any& error)
        {
            logger["error"](std:("Error registering user:"), error);
            return c["json"](object{
                object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
            }, 500);
        }
    }
    );
    authRouter->post(std:("/authenticate"), [=](auto c) mutable
    {
        return authenticate(c);
    }
    );
    authRouter->post(std:("/generate-nonce"), [=](auto c) mutable
    {
        return generateNonce(c);
    }
    );
    authRouter->post(std:("/logout"), [=](auto c) mutable
    {
        return logout(c);
    }
    );
    authRouter->get(std:("/auth-status"), [=](auto c) mutable
    {
        return authStatus(c);
    }
    );
}

MAIN
