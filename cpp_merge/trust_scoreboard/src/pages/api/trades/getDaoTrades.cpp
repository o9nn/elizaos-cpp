#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/trades/getDaoTrades.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    try
    {
        auto URL = string_empty + process->env->DAO_API_URL + string_empty;
        auto response = std::async([=]() { fetch(URL, object{
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}
        }); });
        if (!response->ok) {
            auto error = std::async([=]() { response->json(); });
            return res->status(response->status)->json(object{
                object::pair{std:("error"), OR((error["message"]), (std:("Failed to fetch users")))}
            });
        }
        auto data = std::async([=]() { response->json(); });
        return res->status(200)->json(data);
    }
    catch (const any& error)
    {
        console->error(std:("Error:"), error);
        return res->status(500)->json(object{
            object::pair{std:("error"), std:("Internal Server Error")}
        });
    }
};


