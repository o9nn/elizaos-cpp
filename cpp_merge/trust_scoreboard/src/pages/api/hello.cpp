#include "hello.hpp"

void handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse<Data>> res)
{
    res->status(200)->json(object{
        object::pair{std:("name"), std:("John Doe")}
    });
};


