#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/scripts/testSolSniffer.h"

void testSolSnifferAPI()
{
    auto testAddresses = array<string>{ std::string("Gu3LDkn7Vx3bmCzLafYNKcDxv2mH7YN44NJZFXnypump"), std::string("wUtwjNmjCP9TTTtoc5Xn5h5sZ2cYJm5w2w44b79yr2o") };
    auto apiKey = process->env->SOLSNIFFER_API_KEY;
    console->log(std::string("Testing with addresses:"), testAddresses);
    console->log(std::string("API Key available:"), !!apiKey);
    console->log(std::string("API Key:"), apiKey->substring(0, 5) + std::string("..."));
    try
    {
        auto response = std::async([=]() { axios["post"](std::string("https://solsniffer.com/api/v2/tokens"), object{
            object::pair{std::string("addresses"), testAddresses}
        }, object{
            object::pair{std::string("headers"), object{
                object::pair{std::string("accept"), std::string("application/json")}, 
                object::pair{std::string("X-API-KEY"), apiKey}, 
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}
        }); });
        console->log(std::string("Response status:"), response["status"]);
        console->log(std::string("Response data:"), JSON->stringify(response["data"], nullptr, 2));
    }
    catch (const any& error)
    {
        if (axios["isAxiosError"](error)) {
            console->error(std::string("Error response:"), error["response"]["status"]);
            console->error(std::string("Error data:"), error["response"]["data"]);
            console->error(std::string("Full error:"), error["message"]);
        } else {
            console->error(std::string("Error:"), error["message"]);
        }
    }
};


any dotenv = require(std::string("dotenv"));
any axios = require(std::string("axios"));
any path = require(std::string("path"));

void Main(void)
{
    dotenv["config"](object{
        object::pair{std::string("path"), path["resolve"](process->cwd(), std::string(".env"))}
    });
    dotenv["config"](object{
        object::pair{std::string("path"), path["resolve"](process->cwd(), std::string(".env.local"))}
    });
    testSolSnifferAPI();
}

MAIN
