#include "index.hpp"

std::shared_ptr<express::Router> teeRouter()
{
    auto router = express->Router();
    return router;
};


