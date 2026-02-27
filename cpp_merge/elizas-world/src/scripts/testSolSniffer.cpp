#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/scripts/testSolSniffer.h"

void testSolSnifferAPI()
{
    auto testAddresses = array<string>{ std:("Gu3LDkn7Vx3bmCzLafYNKcDxv2mH7YN44NJZFXnypump"), std:("wUtwjNmjCP9TTTtoc5Xn5h5sZ2cYJm5w2w44b79yr2o") };
    auto apiKey = process->env->SOLSNIFFER_API_KEY;
    console->log(std:("Testing with addresses:"), testAddresses);
    console->log(std:("API Key available:"), !!apiKey);
    console->log(std:("API Key:"), apiKey->substring(0, 5) + std:("..."));
    try
    {
        auto response = std::async([=]() { axios["post"](std:("https://solsniffer.com/api/v2/tokens"), object{
            object::pair{std:("addresses"), testAddresses}
        }, object{
            object::pair{std:("headers"), object{
                object::pair{std:("accept"), std:("application/json")}, 
                object::pair{std:("X-API-KEY"), apiKey}, 
                object::pair{std:("Content-Type"), std:("application/json")}
            }}
        }); });
        console->log(std:("Response status:"), response["status"]);
        console->log(std:("Response data:"), JSON->stringify(response["data"], nullptr, 2));
    }
    catch (const any& error)
    {
        if (axios["isAxiosError"](error)) {
            console->error(std:("Error response:"), error["response"]["status"]);
            console->error(std:("Error data:"), error["response"]["data"]);
            console->error(std:("Full error:"), error["message"]);
        } else {
            console->error(std:("Error:"), error["message"]);
        }
    }
};


any dotenv = require(std:("dotenv"));
any axios = require(std:("axios"));
any path = require(std:("path"));

void Main(void)
{
    dotenv["config"](object{
        object::pair{std:("path"), path["resolve"](process->cwd(), std:(".env"))}
    });
    dotenv["config"](object{
        object::pair{std:("path"), path["resolve"](process->cwd(), std:(".env.local"))}
    });
    testSolSnifferAPI();
}

MAIN
