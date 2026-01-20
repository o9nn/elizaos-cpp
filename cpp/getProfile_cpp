#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/user/getProfile.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    auto session = std::async([=]() { getServerSession(req, res, authOptions); });
    if (OR((OR((!session), (!session->user))), (!session->user->connections))) {
        return res->status(401)->json(object{
            object::pair{std::string("error"), std::string("Unauthorized: No connections found")}
        });
    }
    auto userConnections = session->user->connections;
    if (!userConnections) {
        return res->status(401)->json(object{
            object::pair{std::string("error"), std::string("Unauthorized: No connections found")}
        });
    }
    auto connection = Object->values(userConnections)->find([=](auto connection) mutable
    {
        return connection->accessToken;
    }
    );
    if (!connection) {
        return res->status(401)->json(object{
            object::pair{std::string("error"), std::string("Unauthorized: No Telegram or Discord connection found")}
        });
    }
    if (OR((!connection), (!connection->accessToken))) {
        return res->status(401)->json(object{
            object::pair{std::string("error"), std::string("Unauthorized: No access token found")}
        });
    }
    shared accessToken = connection->accessToken;
    try
    {
        auto response = std::async([=]() { fetch(string_empty + process->env->NEST_API_URL + std::string("/user/profile"), object{
            object::pair{std::string("method"), std::string("GET")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Authorization"), std::string("Bearer ") + accessToken + string_empty}, 
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}
        }); });
        if (!response->ok) {
            auto error = std::async([=]() { response->json(); });
            return res->status(response->status)->json(object{
                object::pair{std::string("error"), OR((error["message"]), (std::string("Failed to fetch profile")))}
            });
        }
        auto profileData = std::async([=]() { response->json(); });
        return res->status(200)->json(profileData);
    }
    catch (const any& error)
    {
        console->error(std::string("Error fetching profile:"), error);
        return res->status(500)->json(object{
            object::pair{std::string("error"), std::string("Internal Server Error")}
        });
    }
};


