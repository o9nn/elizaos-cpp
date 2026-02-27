#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/trades/getDaoTrades.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    try
    {
        auto URL = string_empty + process->env->DAO_API_URL + string_empty;
        auto response = std::async([=]() { fetch(URL, object{
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}
        }); });
        if (!response->ok) {
            auto error = std::async([=]() { response->json(); });
            return res->status(response->status)->json(object{
                object::pair{std::string("error"), OR((error["message"]), (std::string("Failed to fetch users")))}
            });
        }
        auto data = std::async([=]() { response->json(); });
        return res->status(200)->json(data);
    }
    catch (const any& error)
    {
        console->error(std::string("Error:"), error);
        return res->status(500)->json(object{
            object::pair{std::string("error"), std::string("Internal Server Error")}
        });
    }
};


