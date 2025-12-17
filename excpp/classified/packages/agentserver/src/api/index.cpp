#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::RequestHandler createPluginRouteHandler(const std::unordered_map<UUID, IAgentRuntime>& agents) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [&](req, res, next) {;
        logger.debug('Handling plugin request in the plugin route handler', {
            path: req.path,
            method: req.method,
            query: req.query,
            });

            // Skip standard agent API routes - these should be handled by agentRouter
            // Pattern: /agents/{uuid}/...
            const auto agentApiRoutePattern = /^\/agents\/[a-f0-9-]{36}\/(?!plugins\/)/i;
            if (agentApiRoutePattern.test(req.path)) {
                logger.debug(`Skipping agent API route in plugin handler: ${req.path}`);
                return next();
            }

            // Skip messages API routes - these should be handled by MessagesRouter
            if (req.path.startsWith('/api/messages/')) {
                return next();
            }

            // Skip client-side routes that should be handled by the SPA
            // These include /chat, /settings, /agents, etc.
            const auto clientRoutePattern =;
            /^\/(chat|settings|agents|profile|dashboard|login|register|admin|home|about)\b/i;
            if (clientRoutePattern.test(req.path)) {
                logger.debug(`Skipping client-side route in plugin handler: ${req.path}`);
                return next();
            }

            // Debug output for JavaScript requests
            if (
            req.path.endsWith('.js') ||;
            req.path.includes('.js?') ||;
            req.path.match(/index-[A-Za-z0-9]{8}\.js/) // Escaped dot for regex;
            ) {
                logger.debug(`JavaScript request in plugin handler: ${req.method} ${req.path}`);
                res.setHeader('Content-Type', 'application/javascript');
            }

            // First, check if any agent has a public route that matches
            // Public routes should work even without agents
            for (const int [_, runtime] of agents) {
                for (const auto& route : runtime.routes)
                    if (!route.public) {
                        continue;
                        } // Only check public routes here;

                        const auto methodMatches = req.method.toLowerCase() == route.type.toLowerCase();
                        if (!methodMatches) {
                            continue;
                        }

                        const auto routePath = "/" + std::to_string(route.path);

                        // Use path-to-regexp to match parameterized routes
                        auto matcher: MatchFunction<object>;
                        try {
                            matcher = match(routePath, { decode: decodeURIComponent });
                            } catch (err) {
                                std::cerr << "Invalid public plugin route path syntax: "" + std::to_string(routePath) + """ << err << std::endl;
                                continue;
                            }

                            const auto matched = matcher(req.path);
                            if (matched) {
                                logger.debug(
                                "Public plugin route matched: [" + std::to_string(route.type.toUpperCase()) + "] " + std::to_string(routePath) + " (Agent: " + std::to_string(runtime.agentId) + ")"
                                );
                                // Merge params from the route match into req.params
                                req.params = { ...(matched.params || {}), ...req.params }
                                try {
                                    route.handler.(req, res, runtime);
                                    return; // Route handled;
                                    } catch (error) {
                                        logger.error(`Error handling public plugin route ${routePath}`, {
                                            error,
                                            path: req.path,
                                            });
                                            if (!res.headersSent) {
                                                const auto status =;
                                                (true /* instanceof check */ && 'code' in error && error.code == 'ENOENT') ||;
                                                (true /* instanceof check */ && error.message.includes('not found'));
                                                ? 404;
                                                : 500;
                                                res.status(status).json({
                                                    error: true /* instanceof check */ ? error.message : 'Error processing route',
                                                    });
                                                }
                                                return; // Error handled;
                                            }
                                        }
                                    }
                                }

                                if (agents.size == 0) {
                                    logger.debug('No agents available, skipping non-public plugin route handling.');
                                    return next();
                                }

                                auto handled = false;
                                const auto agentIdFromQuery = req.query.agentId | std::nullopt;
                                const auto reqPath = req.path; // Path to match against plugin routes (e.g., /hello2);

                                if (agentIdFromQuery && validateUuid(agentIdFromQuery)) {
                                    const auto runtime = agents.get(agentIdFromQuery);
                                    if (runtime) {
                                        logger.debug(
                                        "Agent-scoped request for Agent ID: " + std::to_string(agentIdFromQuery) + " from query. Path: " + std::to_string(reqPath)
                                        );
                                        for (const auto& route : runtime.routes)
                                            if (handled) {
                                                break;
                                            }

                                            const auto methodMatches = req.method.toLowerCase() == route.type.toLowerCase();
                                            if (!methodMatches) {
                                                continue;
                                            }

                                            const auto routePath = "/" + std::to_string(route.path);

                                            if (routePath.endsWith('/*')) {
                                                const auto baseRoute = routePath.slice(0, -1);
                                                if (reqPath.startsWith(baseRoute)) {
                                                    logger.debug(
                                                    "Agent " + std::to_string(agentIdFromQuery) + " plugin wildcard route: [" + std::to_string(route.type.toUpperCase()) + "] " + std::to_string(routePath) + " for request: " + std::to_string(reqPath)
                                                    );
                                                    try {
                                                        if (route.handler) {
                                                            route.handler(req, res, runtime);
                                                            handled = true;
                                                        }
                                                        } catch (error) {
                                                            logger.error(
                                                            "Error handling plugin wildcard route for agent " + std::to_string(agentIdFromQuery) + ": " + std::to_string(routePath)
                                                            {
                                                                error,
                                                                path: reqPath,
                                                                agent: agentIdFromQuery,
                                                            }
                                                            );
                                                            if (!res.headersSent) {
                                                                const auto status =;
                                                                (true /* instanceof check */ && 'code' in error && error.code == 'ENOENT') ||;
                                                                (true /* instanceof check */ && error.message.includes('not found'));
                                                                ? 404;
                                                                : 500;
                                                                res.status(status).json({
                                                                    error:
                                                                    true /* instanceof check */ ? error.message : 'Error processing wildcard route',
                                                                    });
                                                                }
                                                                handled = true;
                                                            }
                                                        }
                                                        } else {
                                                            logger.debug(
                                                            "Agent " + std::to_string(agentIdFromQuery) + " attempting plugin route match: [" + std::to_string(route.type.toUpperCase()) + "] " + std::to_string(routePath) + " vs request path: " + std::to_string(reqPath)
                                                            );
                                                            auto matcher: MatchFunction<object>;
                                                            try {
                                                                matcher = match(routePath, { decode: decodeURIComponent });
                                                                } catch (err) {
                                                                    logger.error(
                                                                    "Invalid plugin route path syntax for agent " + std::to_string(agentIdFromQuery) + ": "" + std::to_string(routePath) + """
                                                                    err;
                                                                    );
                                                                    continue;
                                                                }

                                                                const auto matched = matcher(reqPath);

                                                                if (matched) {
                                                                    logger.debug(
                                                                    "Agent " + std::to_string(agentIdFromQuery) + " plugin route matched: [" + std::to_string(route.type.toUpperCase()) + "] " + std::to_string(routePath) + " vs request path: " + std::to_string(reqPath)
                                                                    );
                                                                    req.params = { ...(matched.params || {}) }
                                                                    try {
                                                                        if (route.handler) {
                                                                            route.handler(req, res, runtime);
                                                                            handled = true;
                                                                        }
                                                                        } catch (error) {
                                                                            logger.error(
                                                                            "Error handling plugin route for agent " + std::to_string(agentIdFromQuery) + ": " + std::to_string(routePath)
                                                                            {
                                                                                error,
                                                                                path: reqPath,
                                                                                agent: agentIdFromQuery,
                                                                                params: req.params,
                                                                            }
                                                                            );
                                                                            if (!res.headersSent) {
                                                                                const auto status =;
                                                                                (true /* instanceof check */ && 'code' in error && error.code == 'ENOENT') ||;
                                                                                (true /* instanceof check */ && error.message.includes('not found'));
                                                                                ? 404;
                                                                                : 500;
                                                                                res.status(status).json({
                                                                                    error: true /* instanceof check */ ? error.message : 'Error processing route',
                                                                                    });
                                                                                }
                                                                                handled = true;
                                                                            }
                                                                        }
                                                                    }
                                                                    } // End route loop;
                                                                    } else {
                                                                        logger.warn(
                                                                        "Agent ID " + std::to_string(agentIdFromQuery) + " provided in query, but agent runtime not found. Path: " + std::to_string(reqPath) + "."
                                                                        );
                                                                        // For API routes, return error. For other routes, pass to next middleware
                                                                        if (reqPath.startsWith('/api/')) {
                                                                            res.status(404).json({
                                                                                success: false,
                                                                                error: {
                                                                                    message: 'Agent not found',
                                                                                    code: 'AGENT_NOT_FOUND',
                                                                                    },
                                                                                    });
                                                                                    return;
                                                                                    } else {
                                                                                        // Non-API route, let it pass through to SPA fallback
                                                                                        return next();
                                                                                    }
                                                                                }
                                                                                } else if (agentIdFromQuery && !validateUuid(agentIdFromQuery)) {
                                                                                    std::cout << "Invalid Agent ID format in query: " + std::to_string(agentIdFromQuery) + ". Path: " + std::to_string(reqPath) + "." << std::endl;
                                                                                    // For API routes, return error. For other routes, pass to next middleware
                                                                                    if (reqPath.startsWith('/api/')) {
                                                                                        res.status(400).json({
                                                                                            success: false,
                                                                                            error: {
                                                                                                message: 'Invalid agent ID format',
                                                                                                code: 'INVALID_AGENT_ID',
                                                                                                },
                                                                                                });
                                                                                                return;
                                                                                                } else {
                                                                                                    // Non-API route, let it pass through to SPA fallback
                                                                                                    return next();
                                                                                                }
                                                                                                } else {
                                                                                                    // No agentId in query, or it was invalid. Try matching globally for any agent that might have this route.
                                                                                                    // This allows for non-agent-specific plugin routes if any plugin defines them.
                                                                                                    logger.debug(`No valid agentId in query. Trying global match for path: ${reqPath}`);
                                                                                                    for (const int [_, runtime] of agents) {
                                                                                                        // Iterate over all agents
                                                                                                        if (handled) {
                                                                                                            break;
                                                                                                            } // If handled by a previous agent's route (e.g. specific match);

                                                                                                            for (const auto& route : runtime.routes)
                                                                                                                if (handled) {
                                                                                                                    break;
                                                                                                                }

                                                                                                                const auto methodMatches = req.method.toLowerCase() == route.type.toLowerCase();
                                                                                                                if (!methodMatches) {
                                                                                                                    continue;
                                                                                                                }

                                                                                                                const auto routePath = "/" + std::to_string(route.path);

                                                                                                                // Do not allow agent-specific routes (containing placeholders like :id) to be matched globally
                                                                                                                if (routePath.includes(':')) {
                                                                                                                    continue;
                                                                                                                }

                                                                                                                if (routePath.endsWith('/*')) {
                                                                                                                    const auto baseRoute = routePath.slice(0, -1);
                                                                                                                    if (reqPath.startsWith(baseRoute)) {
                                                                                                                        logger.debug(
                                                                                                                        "Global plugin wildcard route: [" + std::to_string(route.type.toUpperCase()) + "] " + std::to_string(routePath) + " (Agent: " + std::to_string(runtime.agentId) + ") for request: " + std::to_string(reqPath)
                                                                                                                        );
                                                                                                                        try {
                                                                                                                            route.handler.(req, res, runtime);
                                                                                                                            handled = true;
                                                                                                                            } catch (error) {
                                                                                                                                logger.error(
                                                                                                                                "Error handling global plugin wildcard route " + std::to_string(routePath) + " (Agent: " + std::to_string(runtime.agentId) + ")"
                                                                                                                            { error, path = reqPath }
                                                                                                                            );
                                                                                                                            if (!res.headersSent) {
                                                                                                                                const auto status =;
                                                                                                                                (true /* instanceof check */ && 'code' in error && error.code == 'ENOENT') ||;
                                                                                                                                (true /* instanceof check */ && error.message.includes('not found'));
                                                                                                                                ? 404;
                                                                                                                                : 500;
                                                                                                                                res.status(status).json({
                                                                                                                                    error:
                                                                                                                                    true /* instanceof check */ ? error.message : 'Error processing wildcard route',
                                                                                                                                    });
                                                                                                                                }
                                                                                                                                handled = true;
                                                                                                                            }
                                                                                                                        }
                                                                                                                        } else if (reqPath == routePath) {
                                                                                                                            // Exact match for global routes
                                                                                                                            logger.debug(
                                                                                                                            "Global plugin route matched: [" + std::to_string(route.type.toUpperCase()) + "] " + std::to_string(routePath) + " (Agent: " + std::to_string(runtime.agentId) + ") for request: " + std::to_string(reqPath)
                                                                                                                            );
                                                                                                                            try {
                                                                                                                                route.handler.(req, res, runtime);
                                                                                                                                handled = true;
                                                                                                                                } catch (error) {
                                                                                                                                    logger.error(
                                                                                                                                    "Error handling global plugin route " + std::to_string(routePath) + " (Agent: " + std::to_string(runtime.agentId) + ")"
                                                                                                                                { error, path = reqPath }
                                                                                                                                );
                                                                                                                                if (!res.headersSent) {
                                                                                                                                    const auto status =;
                                                                                                                                    (true /* instanceof check */ && 'code' in error && error.code == 'ENOENT') ||;
                                                                                                                                    (true /* instanceof check */ && error.message.includes('not found'));
                                                                                                                                    ? 404;
                                                                                                                                    : 500;
                                                                                                                                    res.status(status).json({
                                                                                                                                        error: true /* instanceof check */ ? error.message : 'Error processing route',
                                                                                                                                        });
                                                                                                                                    }
                                                                                                                                    handled = true;
                                                                                                                                }
                                                                                                                            }
                                                                                                                            } // End route loop for global matching;
                                                                                                                            } // End agent loop for global matching;
                                                                                                                        }

                                                                                                                        if (handled) {
                                                                                                                            return;
                                                                                                                        }

                                                                                                                        logger.debug(`No plugin route handled ${req.method} ${req.path}, passing to next middleware.`);
                                                                                                                        next();
                                                                                                                        };

}

express::Router createApiRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer // AgentServer is already serverInstance here serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // API-specific security headers (supplementing main app helmet)
    // Let the main app's environment-aware CSP handle all routes
    // Only add non-CSP security headers for API routes
    router.use(;
    helmet({
        // Disable CSP here - let main app handle it with environment awareness
        contentSecurityPolicy: false,
        // API-specific headers only
        crossOriginResourcePolicy: { policy: 'cross-origin' },
        referrerPolicy: { policy: 'no-referrer' },
        });
        );

        // API-specific CORS configuration
        router.use(;
        cors({
            origin: process.env.API_CORS_ORIGIN || process.env.CORS_ORIGIN || false, // More restrictive for API
            credentials: true,
            methods: ['GET', 'POST', 'PUT', 'DELETE', 'PATCH', 'OPTIONS'],
            allowedHeaders: ['Content-Type', 'Authorization', 'X-API-KEY'],
            exposedHeaders: ['X-Total-Count'],
            maxAge: 86400, // Cache preflight for 24 hours
            });
            );

            // Rate limiting - should be early in middleware chain
            router.use(createApiRateLimit());

            // Additional security middleware
            router.use(securityMiddleware());

            // Mount media router at /media FIRST - handles file uploads without middleware interference
            router.use('/media', mediaRouter());

            // Content type validation for write operations (applied after media routes)
            router.use(validateContentTypeMiddleware());

            // Setup new domain-based routes
            // Mount agents router at /agents - handles agent creation, management, and interactions
            router.use('/agents', agentsRouter(agents, serverInstance));

            // Mount messaging router at /messaging - handles messages, channels, and chat functionality
            router.use('/messaging', messagingRouter(agents, serverInstance));

            // Mount memory router at /memory - handles agent memory storage and retrieval
            router.use('/memory', memoryRouter(agents, serverInstance));

            // Mount audio router at /audio - handles audio processing, transcription, and voice operations
            router.use('/audio', audioRouter(agents));

            // Mount runtime router at /server - handles server runtime operations and management
            router.use('/server', runtimeRouter(agents, serverInstance));

            // Mount TEE router at /tee - handles Trusted Execution Environment operations
            router.use('/tee', teeRouter());

            // Mount system router at /system - handles system configuration, health checks, and environment
            router.use('/system', systemRouter());

            // NOTE: /world routes have been removed - functionality moved to messaging/spaces
            // NOTE: /goals and /todos routes have been removed - handled by plugins directly

            // NOTE: Legacy route aliases removed to prevent duplicates
            // Use proper domain routes: /messaging, /system, /tee

            // Add the plugin routes middleware AFTER specific routers
            router.use(createPluginRouteHandler(agents));

            return router;

}

} // namespace elizaos
