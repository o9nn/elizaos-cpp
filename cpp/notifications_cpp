#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/lib/notifications.h"

std::shared_ptr<Promise<boolean>> sendWelcomeNotification(double fid)
{
    try
    {
        auto response = std::async([=]() { fetch(std::string("/api/notifications/send"), object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("fid"), std::string("fid")}, 
                object::pair{std::string("title"), std::string("Welcome to Eliza OTC Desk")}, 
                object::pair{std::string("body"), std::string("Start trading with AI-powered negotiation on Base, BSC, and Solana")}
            })}
        }); });
        auto result = std::async([=]() { response->json(); });
        return result["state"] == std::string("success");
    }
    catch (const any& error)
    {
        console->error(std::string("Failed to send welcome notification:"), error);
        return false;
    }
};


