#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/index.h"

std::shared_ptr<SocketIOServer> setupSocketIO(std::shared_ptr<http::Server> server, std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto io = std::make_shared<SocketIOServer>(server, object{
        object::pair{std:("cors"), object{
            object::pair{std:("origin"), std:("*")}, 
            object::pair{std:("methods"), array<string>{ std:("GET"), std:("POST") }}
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
    auto destination = const_(loggerInstance)[Symbol->for(std:("pino-destination"))];
    if (AND((destination), (type_of(destination["write"]) == std:("function")))) {
        shared originalWrite = destination["write"]["bind"](destination);
        destination["write"] = [=](P0 data) mutable
        {
            originalWrite(data);
            try
            {
                any logEntry;
                if (type_of(data) == std:("string")) {
                    try
                    {
                        logEntry = JSON->parse(data);
                    }
                    catch (const any& parseError)
                    {
                        logEntry = object{
                            object::pair{std:("message"), data}, 
                            object::pair{std:("level"), std:("info")}
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
        logger->debug(std:("Handling plugin request in the plugin route handler"), std:("path: ") + req["path"] + std:(", method: ") + req["method"] + string_empty, object{
            object::pair{std:("path"), req["path"]}, 
            object::pair{std:("method"), req["method"]}, 
            object::pair{std:("query"), req["query"]}
        });
        auto agentApiRoutePattern = (new RegExp(std:("^\/agents\/[a-f0-9-]{36}\/(?!plugins\/)")));
        if (agentApiRoutePattern->test(req["path"])) {
            logger->debug(std:("Skipping agent API route in plugin handler: ") + req["path"] + string_empty);
            return next();
        }
        if (req["path"]["startsWith"](std:("/api/messages/"))) {
            return next();
        }
        auto isPluginRoute = OR(((new RegExp(std:("^\/api\/agents\/[^\/]+\/plugins\/")))->test(req["path"])), ((new RegExp(std:("^\/agents\/[^\/]+\/plugins\/")))->test(req["path"])));
        if (!isPluginRoute) {
            auto clientRoutePattern = (new RegExp(std:("^\/(chat|settings|agents|profile|dashboard|login|register|admin|home|about)\b")));
            if (clientRoutePattern->test(req["path"])) {
                logger->debug(std:("Skipping client-side route in plugin handler: ") + req["path"] + string_empty);
                return next();
            }
        }
        if (OR((OR((req["path"]["endsWith"](std:(".js"))), (req["path"]["includes"](std:(".js?"))))), (req["path"]["match"]((new RegExp(std:("index-[A-Za-z0-9]{8}\.j"))))))) {
            logger->debug(std:("JavaScript request in plugin handler: ") + req["method"] + std:(" ") + req["path"] + string_empty);
            res["setHeader"](std:("Content-Type"), std:("application/javascript"));
        }
        if (elizaOS->getAgents()->length == 0) {
            logger->debug(std:("No agents available, skipping plugin route handling."));
            return next();
        }
        shared handled = false;
        shared agentIdFromQuery = as<any>(req["query"]["agentId"]);
        shared reqPath = req["path"];
        shared baselessReqPath = reqPath["replace"]((new RegExp(std:("\/api\/agents\/[^\/]+\/plugin"))), string_empty);
        logger->debug(std:("Plugin Request Path"), baselessReqPath);
        auto findRouteInRuntime = [=](auto runtime) mutable
        {
            for (auto& route : runtime->routes)
            {
                if (handled) break;
                auto methodMatches = req["method"]["toLowerCase"]() == route->type->toLowerCase();
                if (!methodMatches) continue;
                auto routePath = route->path;
                if (routePath->endsWith(std:("/*"))) {
                    auto baseRoute = routePath->slice(0, -1);
                    if (baselessReqPath["startsWith"](baseRoute)) {
                        logger->debug(std:("Agent ") + runtime->character->name + std:(" plugin wildcard route: [") + route->type->toUpperCase() + std:("] ") + routePath + std:(" for request: ") + reqPath + string_empty);
                        try
                        {
                            if (route->handler) {
                                route->handler(req, res, runtime);
                                handled = true;
                            }
                        }
                        catch (const any& error)
                        {
                            logger->error(std:("Error handling plugin wildcard route for agent ") + agentIdFromQuery + std:(": ") + routePath + string_empty, (is<Error>(error)) ? error->message : String(error), object{
                                object::pair{std:("path"), reqPath}, 
                                object::pair{std:("agent"), agentIdFromQuery}
                            });
                            if (!res["headersSent"]) {
                                auto status = (OR(((AND((AND((is<Error>(error)), (in(std:("code"), error)))), (error->code == std:("ENOENT"))))), ((AND((is<Error>(error)), (error->message->includes(std:("not found")))))))) ? 404 : 500;
                                res["status"](status)["json"](object{
                                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Error processing wildcard route"))}
                                });
                            }
                            handled = true;
                        }
                    }
                } else {
                    logger->debug(std:("Agent ") + runtime->character->name + std:(" attempting plugin route match: [") + route->type->toUpperCase() + std:("] ") + routePath + std:(" vs request path: ") + baselessReqPath + string_empty);
                    std::shared_ptr<MatchFunction<any>> matcher;
                    try
                    {
                        matcher = match(routePath, object{
                            object::pair{std:("decode"), decodeURIComponent}
                        });
                    }
                    catch (const any& err)
                    {
                        logger->error(std:("Invalid plugin route path syntax for agent ") + agentIdFromQuery + std:(": "") + routePath + std:("""), (is<Error>(err)) ? err->message : String(err));
                        continue;
                    }
                    auto matched = matcher(baselessReqPath);
                    if (matched) {
                        logger->debug(std:("Agent ") + runtime->character->name + std:(" plugin route matched: [") + route->type->toUpperCase() + std:("] ") + routePath + std:(" vs request path: ") + reqPath + string_empty);
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
                            logger->error(std:("Error handling plugin route for agent ") + agentIdFromQuery + std:(": ") + routePath + string_empty, (is<Error>(error)) ? error->message : String(error), object{
                                object::pair{std:("path"), reqPath}, 
                                object::pair{std:("agent"), agentIdFromQuery}, 
                                object::pair{std:("params"), req["params"]}
                            });
                            if (!res["headersSent"]) {
                                auto status = (OR(((AND((AND((is<Error>(error)), (in(std:("code"), error)))), (error->code == std:("ENOENT"))))), ((AND((is<Error>(error)), (error->message->includes(std:("not found")))))))) ? 404 : 500;
                                res["status"](status)["json"](object{
                                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Error processing route"))}
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
                logger->debug(std:("Agent-scoped request for Agent ID: ") + agentIdFromQuery + std:(" from query. Path: ") + reqPath + string_empty);
                handled = findRouteInRuntime(runtime);
            } else {
                logger->warn(std:("Agent ID ") + agentIdFromQuery + std:(" provided in query, but agent runtime not found. Path: ") + reqPath + std:("."));
                if (reqPath["startsWith"](std:("/api/"))) {
                    res["status"](404)["json"](object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), object{
                            object::pair{std:("message"), std:("Agent not found")}, 
                            object::pair{std:("code"), std:("AGENT_NOT_FOUND")}
                        }}
                    });
                    return any();
                } else {
                    return next();
                }
            }
        } else if (AND((agentIdFromQuery), (!validateUuid(agentIdFromQuery)))) {
            logger->warn(std:("Invalid Agent ID format in query: ") + agentIdFromQuery + std:(". Path: ") + reqPath + std:("."));
            if (reqPath["startsWith"](std:("/api/"))) {
                res["status"](400)["json"](object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), object{
                        object::pair{std:("message"), std:("Invalid agent ID format")}, 
                        object::pair{std:("code"), std:("INVALID_AGENT_ID")}
                    }}
                });
                return any();
            } else {
                return next();
            }
        } else {
            logger->debug(std:("No valid agentId in query. Trying global match for path: ") + reqPath + string_empty);
            for (auto& runtime : elizaOS->getAgents())
            {
                if (handled) break;
                handled = findRouteInRuntime(runtime);
            }
        }
        if (handled) {
            return any();
        }
        logger->debug(std:("No plugin route handled ") + req["method"] + std:(" ") + req["path"] + std:(", passing to next middleware."));
        next();
    };
};


std::shared_ptr<express::Router> createApiRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(helmet(object{
        object::pair{std:("contentSecurityPolicy"), false}, 
        object::pair{std:("crossOriginResourcePolicy"), object{
            object::pair{std:("policy"), std:("cross-origin")}
        }}, 
        object::pair{std:("referrerPolicy"), object{
            object::pair{std:("policy"), std:("no-referrer")}
        }}
    }));
    router->use(cors(object{
        object::pair{std:("origin"), OR((OR((process->env->API_CORS_ORIGIN), (process->env->CORS_ORIGIN))), (false))}, 
        object::pair{std:("credentials"), true}, 
        object::pair{std:("methods"), array<string>{ std:("GET"), std:("POST"), std:("PUT"), std:("DELETE"), std:("PATCH"), std:("OPTIONS") }}, 
        object::pair{std:("allowedHeaders"), array<string>{ std:("Content-Type"), std:("Authorization"), std:("X-API-KEY"), std:("X-PAYMENT"), std:("X-PAYMENT-RESPONSE") }}, 
        object::pair{std:("exposedHeaders"), array<string>{ std:("X-Total-Count"), std:("X-PAYMENT-RESPONSE") }}, 
        object::pair{std:("maxAge"), 86400}
    }));
    router->use(createApiRateLimit());
    router->use(securityMiddleware());
    router->use(std:("/media"), mediaRouter());
    router->use(validateContentTypeMiddleware());
    router->use(std:("/agents"), agentsRouter(elizaOS, serverInstance));
    router->use(std:("/messaging"), messagingRouter(elizaOS, serverInstance));
    router->use(std:("/memory"), memoryRouter(elizaOS, serverInstance));
    router->use(std:("/entities"), entitiesRouter(serverInstance));
    router->use(std:("/auth"), createAuthRouter());
    router->use(std:("/cdp"), cdpRouter(serverInstance));
    router->use(std:("/audio"), audioRouter(elizaOS));
    router->use(std:("/server"), runtimeRouter(elizaOS, serverInstance));
    router->use(std:("/tee"), teeRouter());
    router->use(std:("/system"), systemRouter());
    router->use(createPluginRouteHandler(elizaOS));
    return router;
};


