#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/index.h"

std::shared_ptr<SocketIOServer> setupSocketIO(std::shared_ptr<http::Server> server, std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto io = std::make_shared<SocketIOServer>(server, object{
        object::pair{std::string("cors"), object{
            object::pair{std::string("origin"), std::string("*")}, 
            object::pair{std::string("methods"), array<string>{ std::string("GET"), std::string("POST") }}
        }}
    });
    auto centralSocketRouter = std::make_shared<SocketIORouter>(elizaOS, serverInstance);
    centralSocketRouter->setupListeners(io);
    setupLogStreaming(io, centralSocketRouter);
    return io;
};


void setupLogStreaming(std::shared_ptr<SocketIOServer> io, std::shared_ptr<SocketIORouter> router)
{
    auto loggerInstance = as<any>(logger);
    auto destination = const_(loggerInstance)[Symbol->for(std::string("pino-destination"))];
    if (AND((destination), (type_of(destination["write"]) == std::string("function")))) {
        shared originalWrite = destination["write"]["bind"](destination);
        destination["write"] = [=](P0 data) mutable
        {
            originalWrite(data);
            try
            {
                any logEntry;
                if (type_of(data) == std::string("string")) {
                    try
                    {
                        logEntry = JSON->parse(data);
                    }
                    catch (const any& parseError)
                    {
                        logEntry = object{
                            object::pair{std::string("message"), data}, 
                            object::pair{std::string("level"), std::string("info")}
                        };
                    }
                } else {
                    logEntry = data;
                }
                if (!logEntry["time"]) {
                    logEntry["time"] = Date->now();
                }
                router->broadcastLog(io, logEntry);
            }
            catch (const any& error)
            {
            }
        };
    }
};


std::shared_ptr<express::RequestHandler> createPluginRouteHandler(std::shared_ptr<ElizaOS> elizaOS)
{
    return [=](auto req, auto res, auto next) mutable
    {
        logger->debug(std::string("Handling plugin request in the plugin route handler"), std::string("path: ") + req["path"] + std::string(", method: ") + req["method"] + string_empty, object{
            object::pair{std::string("path"), req["path"]}, 
            object::pair{std::string("method"), req["method"]}, 
            object::pair{std::string("query"), req["query"]}
        });
        auto agentApiRoutePattern = (new RegExp(std::string("^\/agents\/[a-f0-9-]{36}\/(?!plugins\/)")));
        if (agentApiRoutePattern->test(req["path"])) {
            logger->debug(std::string("Skipping agent API route in plugin handler: ") + req["path"] + string_empty);
            return next();
        }
        if (req["path"]["startsWith"](std::string("/api/messages/"))) {
            return next();
        }
        auto isPluginRoute = OR(((new RegExp(std::string("^\/api\/agents\/[^\/]+\/plugins\/")))->test(req["path"])), ((new RegExp(std::string("^\/agents\/[^\/]+\/plugins\/")))->test(req["path"])));
        if (!isPluginRoute) {
            auto clientRoutePattern = (new RegExp(std::string("^\/(chat|settings|agents|profile|dashboard|login|register|admin|home|about)\b")));
            if (clientRoutePattern->test(req["path"])) {
                logger->debug(std::string("Skipping client-side route in plugin handler: ") + req["path"] + string_empty);
                return next();
            }
        }
        if (OR((OR((req["path"]["endsWith"](std::string(".js"))), (req["path"]["includes"](std::string(".js?"))))), (req["path"]["match"]((new RegExp(std::string("index-[A-Za-z0-9]{8}\.j"))))))) {
            logger->debug(std::string("JavaScript request in plugin handler: ") + req["method"] + std::string(" ") + req["path"] + string_empty);
            res["setHeader"](std::string("Content-Type"), std::string("application/javascript"));
        }
        if (elizaOS->getAgents()->length == 0) {
            logger->debug(std::string("No agents available, skipping plugin route handling."));
            return next();
        }
        shared handled = false;
        shared agentIdFromQuery = as<any>(req["query"]["agentId"]);
        shared reqPath = req["path"];
        shared baselessReqPath = reqPath["replace"]((new RegExp(std::string("\/api\/agents\/[^\/]+\/plugin"))), string_empty);
        logger->debug(std::string("Plugin Request Path"), baselessReqPath);
        auto findRouteInRuntime = [=](auto runtime) mutable
        {
            for (auto& route : runtime->routes)
            {
                if (handled) break;
                auto methodMatches = req["method"]["toLowerCase"]() == route->type->toLowerCase();
                if (!methodMatches) continue;
                auto routePath = route->path;
                if (routePath->endsWith(std::string("/*"))) {
                    auto baseRoute = routePath->slice(0, -1);
                    if (baselessReqPath["startsWith"](baseRoute)) {
                        logger->debug(std::string("Agent ") + runtime->character->name + std::string(" plugin wildcard route: [") + route->type->toUpperCase() + std::string("] ") + routePath + std::string(" for request: ") + reqPath + string_empty);
                        try
                        {
                            if (route->handler) {
                                route->handler(req, res, runtime);
                                handled = true;
                            }
                        }
                        catch (const any& error)
                        {
                            logger->error(std::string("Error handling plugin wildcard route for agent ") + agentIdFromQuery + std::string(": ") + routePath + string_empty, (is<Error>(error)) ? error->message : String(error), object{
                                object::pair{std::string("path"), reqPath}, 
                                object::pair{std::string("agent"), agentIdFromQuery}
                            });
                            if (!res["headersSent"]) {
                                auto status = (OR(((AND((AND((is<Error>(error)), (in(std::string("code"), error)))), (error->code == std::string("ENOENT"))))), ((AND((is<Error>(error)), (error->message->includes(std::string("not found")))))))) ? 404 : 500;
                                res["status"](status)["json"](object{
                                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Error processing wildcard route"))}
                                });
                            }
                            handled = true;
                        }
                    }
                } else {
                    logger->debug(std::string("Agent ") + runtime->character->name + std::string(" attempting plugin route match: [") + route->type->toUpperCase() + std::string("] ") + routePath + std::string(" vs request path: ") + baselessReqPath + string_empty);
                    std::shared_ptr<MatchFunction<any>> matcher;
                    try
                    {
                        matcher = match(routePath, object{
                            object::pair{std::string("decode"), decodeURIComponent}
                        });
                    }
                    catch (const any& err)
                    {
                        logger->error(std::string("Invalid plugin route path syntax for agent ") + agentIdFromQuery + std::string(": "") + routePath + std::string("""), (is<Error>(err)) ? err->message : String(err));
                        continue;
                    }
                    auto matched = matcher(baselessReqPath);
                    if (matched) {
                        logger->debug(std::string("Agent ") + runtime->character->name + std::string(" plugin route matched: [") + route->type->toUpperCase() + std::string("] ") + routePath + std::string(" vs request path: ") + reqPath + string_empty);
                        req["params"] = utils::assign(object{
                        }, (OR((matched->params), (object{}))));
                        try
                        {
                            if (route->handler) {
                                route->handler(req, res, runtime);
                                handled = true;
                            }
                        }
                        catch (const any& error)
                        {
                            logger->error(std::string("Error handling plugin route for agent ") + agentIdFromQuery + std::string(": ") + routePath + string_empty, (is<Error>(error)) ? error->message : String(error), object{
                                object::pair{std::string("path"), reqPath}, 
                                object::pair{std::string("agent"), agentIdFromQuery}, 
                                object::pair{std::string("params"), req["params"]}
                            });
                            if (!res["headersSent"]) {
                                auto status = (OR(((AND((AND((is<Error>(error)), (in(std::string("code"), error)))), (error->code == std::string("ENOENT"))))), ((AND((is<Error>(error)), (error->message->includes(std::string("not found")))))))) ? 404 : 500;
                                res["status"](status)["json"](object{
                                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Error processing route"))}
                                });
                            }
                            handled = true;
                        }
                    }
                }
            }
            return handled;
        };

        if (AND((agentIdFromQuery), (validateUuid(agentIdFromQuery)))) {
            auto runtime = elizaOS->getAgent(agentIdFromQuery);
            if (runtime) {
                logger->debug(std::string("Agent-scoped request for Agent ID: ") + agentIdFromQuery + std::string(" from query. Path: ") + reqPath + string_empty);
                handled = findRouteInRuntime(runtime);
            } else {
                logger->warn(std::string("Agent ID ") + agentIdFromQuery + std::string(" provided in query, but agent runtime not found. Path: ") + reqPath + std::string("."));
                if (reqPath["startsWith"](std::string("/api/"))) {
                    res["status"](404)["json"](object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), object{
                            object::pair{std::string("message"), std::string("Agent not found")}, 
                            object::pair{std::string("code"), std::string("AGENT_NOT_FOUND")}
                        }}
                    });
                    return any();
                } else {
                    return next();
                }
            }
        } else if (AND((agentIdFromQuery), (!validateUuid(agentIdFromQuery)))) {
            logger->warn(std::string("Invalid Agent ID format in query: ") + agentIdFromQuery + std::string(". Path: ") + reqPath + std::string("."));
            if (reqPath["startsWith"](std::string("/api/"))) {
                res["status"](400)["json"](object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), object{
                        object::pair{std::string("message"), std::string("Invalid agent ID format")}, 
                        object::pair{std::string("code"), std::string("INVALID_AGENT_ID")}
                    }}
                });
                return any();
            } else {
                return next();
            }
        } else {
            logger->debug(std::string("No valid agentId in query. Trying global match for path: ") + reqPath + string_empty);
            for (auto& runtime : elizaOS->getAgents())
            {
                if (handled) break;
                handled = findRouteInRuntime(runtime);
            }
        }
        if (handled) {
            return any();
        }
        logger->debug(std::string("No plugin route handled ") + req["method"] + std::string(" ") + req["path"] + std::string(", passing to next middleware."));
        next();
    };
};


std::shared_ptr<express::Router> createApiRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(helmet(object{
        object::pair{std::string("contentSecurityPolicy"), false}, 
        object::pair{std::string("crossOriginResourcePolicy"), object{
            object::pair{std::string("policy"), std::string("cross-origin")}
        }}, 
        object::pair{std::string("referrerPolicy"), object{
            object::pair{std::string("policy"), std::string("no-referrer")}
        }}
    }));
    router->use(cors(object{
        object::pair{std::string("origin"), OR((OR((process->env->API_CORS_ORIGIN), (process->env->CORS_ORIGIN))), (false))}, 
        object::pair{std::string("credentials"), true}, 
        object::pair{std::string("methods"), array<string>{ std::string("GET"), std::string("POST"), std::string("PUT"), std::string("DELETE"), std::string("PATCH"), std::string("OPTIONS") }}, 
        object::pair{std::string("allowedHeaders"), array<string>{ std::string("Content-Type"), std::string("Authorization"), std::string("X-API-KEY"), std::string("X-PAYMENT"), std::string("X-PAYMENT-RESPONSE") }}, 
        object::pair{std::string("exposedHeaders"), array<string>{ std::string("X-Total-Count"), std::string("X-PAYMENT-RESPONSE") }}, 
        object::pair{std::string("maxAge"), 86400}
    }));
    router->use(createApiRateLimit());
    router->use(securityMiddleware());
    router->use(std::string("/media"), mediaRouter());
    router->use(validateContentTypeMiddleware());
    router->use(std::string("/agents"), agentsRouter(elizaOS, serverInstance));
    router->use(std::string("/messaging"), messagingRouter(elizaOS, serverInstance));
    router->use(std::string("/memory"), memoryRouter(elizaOS, serverInstance));
    router->use(std::string("/entities"), entitiesRouter(serverInstance));
    router->use(std::string("/auth"), createAuthRouter());
    router->use(std::string("/cdp"), cdpRouter(serverInstance));
    router->use(std::string("/audio"), audioRouter(elizaOS));
    router->use(std::string("/server"), runtimeRouter(elizaOS, serverInstance));
    router->use(std::string("/tee"), teeRouter());
    router->use(std::string("/system"), systemRouter());
    router->use(createPluginRouteHandler(elizaOS));
    return router;
};


