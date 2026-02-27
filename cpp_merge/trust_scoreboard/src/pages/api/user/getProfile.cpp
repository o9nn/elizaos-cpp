#include "getProfile.hpp"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    auto session = std::async([=]() { getServerSession(req, res, authOptions); });
    if (OR((OR((!session), (!session->user))), (!session->user->connections))) {
        return res->status(401)->json(object{
            object::pair{std:("error"), std:("Unauthorized: No connections found")}
        });
    }
    auto userConnections = session->user->connections;
    if (!userConnections) {
        return res->status(401)->json(object{
            object::pair{std:("error"), std:("Unauthorized: No connections found")}
        });
    }
    auto connection = Object->values(userConnections)->find([=](auto connection) mutable
    {
        return connection->accessToken;
    }
    );
    if (!connection) {
        return res->status(401)->json(object{
            object::pair{std:("error"), std:("Unauthorized: No Telegram or Discord connection found")}
        });
    }
    if (OR((!connection), (!connection->accessToken))) {
        return res->status(401)->json(object{
            object::pair{std:("error"), std:("Unauthorized: No access token found")}
        });
    }
    shared accessToken = connection->accessToken;
    try
    {
        auto response = std::async([=]() { fetch(string_empty + process->env->NEST_API_URL + std:("/user/profile"), object{
            object::pair{std:("method"), std:("GET")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Authorization"), std:("Bearer ") + accessToken + string_empty}, 
                object::pair{std:("Content-Type"), std:("application/json")}
            }}
        }); });
        if (!response->ok) {
            auto error = std::async([=]() { response->json(); });
            return res->status(response->status)->json(object{
                object::pair{std:("error"), OR((error["message"]), (std:("Failed to fetch profile")))}
            });
        }
        auto profileData = std::async([=]() { response->json(); });
        return res->status(200)->json(profileData);
    }
    catch (const any& error)
    {
        console->error(std:("Error fetching profile:"), error);
        return res->status(500)->json(object{
            object::pair{std:("error"), std:("Internal Server Error")}
        });
    }
};


