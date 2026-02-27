#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/setup.h"

void Main(void)
{
    mock->module(std:("@elizaos/server"), [=]() mutable
    {
        return (object{
            object::pair{std:("AgentServer"), mock([=]() mutable
            {
                return (object{
                    object::pair{std:("initialize"), mock()}, 
                    object::pair{std:("startAgent"), mock()}, 
                    object::pair{std:("stopAgent"), mock()}, 
                    object::pair{std:("loadCharacterTryPath"), mock()}, 
                    object::pair{std:("jsonToCharacter"), mock()}
                });
            }
            )}, 
            object::pair{std:("expandTildePath"), mock([=](auto path) mutable
            {
                return path;
            }
            )}, 
            object::pair{std:("resolvePgliteDir"), mock([=](auto dir = undefined) mutable
            {
                return OR((dir), (std:("./.elizadb")));
            }
            )}
        });
    }
    );
    mock->module(std:("socket.io"), [=]() mutable
    {
        return (object{
            object::pair{std:("Server"), mock([=]() mutable
            {
                return (object{
                    object::pair{std:("on"), mock()}, 
                    object::pair{std:("emit"), mock()}, 
                    object::pair{std:("use"), mock()}, 
                    object::pair{std:("engine"), object{
                        object::pair{std:("on"), mock()}
                    }}
                });
            }
            )}
        });
    }
    );
    mock->module(std:("express"), [=]() mutable
    {
        shared mockApp = object{
            object::pair{std:("use"), mock()}, 
            object::pair{std:("get"), mock()}, 
            object::pair{std:("post"), mock()}, 
            object::pair{std:("put"), mock()}, 
            object::pair{std:("delete"), mock()}, 
            object::pair{std:("listen"), mock()}, 
            object::pair{std:("set"), mock()}
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
                object::pair{std:("use"), mock()}, 
                object::pair{std:("get"), mock()}, 
                object::pair{std:("post"), mock()}, 
                object::pair{std:("put"), mock()}, 
                object::pair{std:("delete"), mock()}
            });
        }
        );
        return utils::assign(object{
            object::pair{std:("default"), mockExpress}
        }, mockExpress);
    }
    );
    mock->module(std:("body-parser"), [=]() mutable
    {
        return (object{
            object::pair{std:("json"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}, 
            object::pair{std:("urlencoded"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}, 
            object::pair{std:("text"), mock([=]() mutable
            {
                return [=](auto req, auto res, auto next) mutable
                {
                    return next();
                };
            }
            )}, 
            object::pair{std:("raw"), mock([=]() mutable
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
    mock->module(std:("helmet"), [=]() mutable
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
            object::pair{std:("default"), helmet}
        }, helmet);
    }
    );
    mock->module(std:("cors"), [=]() mutable
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
            object::pair{std:("default"), cors}
        }, cors);
    }
    );
    global->console = utils::assign(object{
        , 
        object::pair{std:("debug"), console->log}, 
        object::pair{std:("trace"), console->log}
    }, console);
}

MAIN
