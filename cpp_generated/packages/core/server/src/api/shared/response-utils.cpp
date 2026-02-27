#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/shared/response-utils.h"

std::function<void(any, double, string, string, string)> sendError = [=](auto res, auto status, auto code, auto message, auto details = undefined) mutable
{
    res->status(status)->json(object{
        object::pair{std::string("success"), false}, 
        object::pair{std::string("error"), utils::assign(object{
            object::pair{std::string("code"), std::string("code")}, 
            object::pair{std::string("message"), std::string("message")}
        }, (AND((details), (object{
            object::pair{std::string("details"), std::string("details")}
        }))))}
    });
};
std::function<void(any, any, double)> sendSuccess = [=](auto res, auto data, auto status = 200) mutable
{
    res->status(status)->json(object{
        object::pair{std::string("success"), true}, 
        object::pair{std::string("data"), std::string("data")}
    });
};

void Main(void)
{
}

MAIN
