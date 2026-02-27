#include "notifications.hpp"

std::shared_ptr<Promise<boolean>> sendWelcomeNotification(double fid)
{
    try
    {
        auto response = std::async([=]() { fetch(std:("/api/notifications/send"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("fid"), std:("fid")}, 
                object::pair{std:("title"), std:("Welcome to Eliza OTC Desk")}, 
                object::pair{std:("body"), std:("Start trading with AI-powered negotiation on Base, BSC, and Solana")}
            })}
        }); });
        auto result = std::async([=]() { response->json(); });
        return result["state"] == std:("success");
    }
    catch (const any& error)
    {
        console->error(std:("Failed to send welcome notification:"), error);
        return false;
    }
};


