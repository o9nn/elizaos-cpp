#include "example-usage.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createWeatherPlugin(Agent agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto weatherPluginSpec = {;
        name: "@elizaos/plugin-weather",
        description: "Weather information and forecasting plugin",
        version: "1.0.0",
        actions: [
        {
            name: "getCurrentWeather",
            description: "Get current weather for a location",
            parameters: {
                location: "string",
                units: "celsius | fahrenheit",
                },
                },
                {
                    name: "getWeatherForecast",
                    description: "Get weather forecast for the next 5 days",
                    parameters: {
                        location: "string",
                        days: "number",
                        },
                        },
                        ],
                        providers: [
                        {
                            name: "weatherData",
                            description: "Provides current weather data in context",
                            dataStructure: {
                                currentLocation: "string",
                                temperature: "number",
                                conditions: "string",
                                lastUpdated: "timestamp",
                                },
                                },
                                ],
                                dependencies: {
                                    axios: "^1.6.0",
                                    },
                                    environmentVariables: [
                                    {
                                        name: "WEATHER_API_KEY",
                                        description: "API key for weather service",
                                        required: true,
                                        sensitive: true,
                                        },
                                        ],
                                        };

                                        // Simulate agent conversation
                                        std::cout << "User: Create a weather plugin with the following spec:" << std::endl;
                                        std::cout << JSON.stringify(weatherPluginSpec, nullptr, 2) << std::endl;

                                        // Agent would process this through the createPlugin action
                                        // const result = await agent.processMessage("Create plugin: " + JSON.stringify(weatherPluginSpec));

}

std::future<void> createTodoPlugin(Agent agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto description = `;
    I need a plugin that helps manage todo lists. It should have:
    - An action to add new todos with title and optional due date;
    - An action to mark todos;
    - An action to list all todos with filtering options;
    - A provider that shows pending todo count;
    - Store todos in memory with persistence;
    `;

    std::cout << "User:" << description << std::endl;

    // Agent would process this through the createPluginFromDescription action
    // const result = await agent.processMessage(description);

}

std::future<void> monitorPluginCreation(Agent agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check status
    std::cout << "User: What's the status of my plugin creation?" << std::endl;

    // Agent would provide status through checkPluginCreationStatus action
    // Example response:
    console.log(`Agent: Plugin Creation Status: running
    Current Phase: testing
    Progress: 80%

    Recent logs:
    - Building plugin...;
    - Build successful;
    - Running tests...;
    - 12 tests passed;
    - Running validation...`);

}

void getDatabasePluginSpec() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        name: "@elizaos/plugin-postgres",
        description: "PostgreSQL database integration",
        version: "1.0.0",
        services: [
        {
            name: "PostgresService",
            description: "Manages PostgreSQL connections and queries",
            methods: ["connect", "disconnect", "query", "transaction"],
            },
            ],
            actions: [
            {
                name: "executeQuery",
                description: "Execute a SQL query",
                parameters: {
                    query: "string",
                    params: "array",
                    },
                    },
                    {
                        name: "getDatabaseStats",
                        description: "Get database statistics and health",
                        parameters: {},
                        },
                        ],
                        evaluators: [
                        {
                            name: "databaseHealth",
                            description: "Monitors database connection health",
                            triggers: ["*/5 * * * *"], // Every 5 minutes
                            },
                            ],
                            dependencies: {
                                pg: "^8.11.0",
                                "pg-pool": "^3.6.0",
                                },
                                environmentVariables: [
                                {
                                    name: "DATABASE_URL",
                                    description: "PostgreSQL connection string",
                                    required: true,
                                    sensitive: true,
                                    },
                                    {
                                        name: "DATABASE_POOL_SIZE",
                                        description: "Maximum connection pool size",
                                        required: false,
                                        sensitive: false,
                                        },
                                        ],
                                        };

}

void getSocialMediaPluginSpec() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        name: "@elizaos/plugin-social",
        description: "Social media integration plugin",
        version: "1.0.0",
        actions: [
        {
            name: "postToTwitter",
            description: "Post a message to Twitter/X",
            parameters: {
                message: "string",
                mediaUrls: "string[]",
                },
                },
                {
                    name: "schedulePost",
                    description: "Schedule a social media post",
                    parameters: {
                        platform: "twitter | instagram | linkedin",
                        message: "string",
                        scheduledTime: "timestamp",
                        },
                        },
                        ],
                        providers: [
                        {
                            name: "socialStats",
                            description: "Provides social media statistics",
                            dataStructure: {
                                followers: "number",
                                engagement: "number",
                                recentPosts: "array",
                                },
                                },
                                ],
                                };

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Initialize agent with plugin
    const auto agent = new Agent({;
        name: "PluginCreator",
        plugins: [pluginDynamic],
        // ... other configuration
        });

        // Example workflows
        std::cout << "== Weather Plugin Example ==" << std::endl;
        createWeatherPlugin(agent);

        std::cout << "\n== Todo Plugin from Description ==" << std::endl;
        createTodoPlugin(agent);

        std::cout << "\n== Database Plugin Specification ==" << std::endl;
        std::cout << JSON.stringify(getDatabasePluginSpec(), nullptr, 2) << std::endl;

        std::cout << "\n== Social Media Plugin Specification ==" << std::endl;
        std::cout << JSON.stringify(getSocialMediaPluginSpec(), nullptr, 2) << std::endl;

}

} // namespace elizaos
