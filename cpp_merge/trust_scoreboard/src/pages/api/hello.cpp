#include "hello.hpp"
#include <string>

void handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse<Data>> res)
{
    res->status(200)->json(object{
        object::pair{std::string("name"), std::string("John Doe")}
    });
};


