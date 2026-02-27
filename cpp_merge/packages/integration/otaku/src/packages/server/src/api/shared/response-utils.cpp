#include "response-utils.h"

std::function<void(any, double, string, string, string)> sendError = [=](auto res, auto status, auto code, auto message, auto details = undefined) mutable
{
    res->status(status)->json(object{
        object::pair{std:("success"), false}, 
        object::pair{std:("error"), utils::assign(object{
            object::pair{std:("code"), std:("code")}, 
            object::pair{std:("message"), std:("message")}
        }, (AND((details), (object{
            object::pair{std:("details"), std:("details")}
        }))))}
    });
};
std::function<void(any, any, double)> sendSuccess = [=](auto res, auto data, auto status = 200) mutable
{
    res->status(status)->json(object{
        object::pair{std:("success"), true}, 
        object::pair{std:("data"), std:("data")}
    });
};

void Main(void)
{
}

MAIN
