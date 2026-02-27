#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/setup.h"

void Main(void)
{
    mock->module(std::string("@elizaos/server"), [=]() mutable
    {
        return (object{
            object::pair{std::string("AgentServer"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("initialize"), mock()}, 
                    object::pair{std::string("startAgent"), mock()}, 
                    object::pair{std::string("stopAgent"), mock()}, 
                    object::pair{std::string("loadCharacterTryPath"), mock()}, 
                    object::pair{std::string("jsonToCharacter"), mock()}
                });
            }
            )}, 
            object::pair{std::string("expandTildePath"), mock([=](auto path) mutable
            {
                return path;
            }
            )}, 
            object::pair{std::string("resolvePgliteDir"), mock([=](auto dir = undefined) mutable
            {
                return OR((dir), (std::string("./.elizadb")));
            }
            )}
        });
    }
    );
    mock->module(std::string("socket.io"), [=]() mutable
    {
        return (object{
            object::pair{std::string("Server"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("on"), mock()}, 
                    object::pair{std::string("emit"), mock()}, 
                    object::pair{std::string("use"), mock()}, 
                    object::pair{std::string("engine"), object{
                        object::pair{std::string("on"), mock()}
                    }}
                });
            }
            )}
        });
    }
    );
    mock->module(std::string("express"), [=]() mutable
    {
        shared mockApp = object{
            object::pair{std::string("use"), mock()}, 
            object::pair{std::string("get"), mock()}, 
            object::pair{std::string("post"), mock()}, 
            object::pair{std::string("put"), mock()}, 
            object::pair{std::string("delete"), mock()}, 
            object::pair{std::string("listen"), mock()}, 
            object::pair{std::string("set"), mock()}
        };
        auto mockExpress = mock([=]() mutable
        {
            return mockApp;
        }
        );
        mockExpress->static = mock();
        mockExpress->json = mock();
        mockExpress->urlencoded = mock();
        mockExpress->Router = mock([=]() mutable
        {
            return (object{
                object::pair{std::string("use"), mock()}, 
                object::pair{std::string("get"), mock()}, 
                object::pair{std::string("post"), mock()}, 
                object::pair{std::string("put"), mock()}, 
                object::pair{std::string("delete"), mock()}
            });
        }
        );
        return utils::assign(object{
            object::pair{std::string("default"), mockExpress}
        }, mockExpress);
    }
    );
    mock->module(std::string("body-parser"), [=]() mutable
    {
        return (object{
            object::pair{std::string("json"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}, 
            object::pair{std::string("urlencoded"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}, 
            object::pair{std::string("text"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}, 
            object::pair{std::string("raw"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}
        });
    }
    );
    mock->module(std::string("helmet"), [=]() mutable
    {
        auto helmet = mock([=]() mutable
        {
            return [=](auto req, auto res, auto next) mutable
            {
                return next();
            };
        }
        );
        return utils::assign(object{
            object::pair{std::string("default"), helmet}
        }, helmet);
    }
    );
    mock->module(std::string("cors"), [=]() mutable
    {
        auto cors = mock([=]() mutable
        {
            return [=](auto req, auto res, auto next) mutable
            {
                return next();
            };
        }
        );
        return utils::assign(object{
            object::pair{std::string("default"), cors}
        }, cors);
    }
    );
    global->console = utils::assign(object{
        , 
        object::pair{std::string("debug"), console->log}, 
        object::pair{std::string("trace"), console->log}
    }, console);
}

MAIN
