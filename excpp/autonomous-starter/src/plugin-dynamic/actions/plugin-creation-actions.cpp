#include "plugin-creation-actions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<PluginSpecification> generatePluginSpecification(const std::string& description, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowerDesc = description.toLowerCase();

    // Detect plugin type and generate appropriate name
    auto name = "@elizaos/plugin-";
    auto pluginType = "custom";

    if (lowerDesc.includes("weather")) {
        pluginType = "weather";
        name += "weather";
        } else if (lowerDesc.includes("database") || lowerDesc.includes("sql")) {
            pluginType = "database";
            name += "database";
            } else if (lowerDesc.includes("api") || lowerDesc.includes("rest")) {
                pluginType = "api";
                name += "api";
                } else if (lowerDesc.includes("todo") || lowerDesc.includes("task")) {
                    pluginType = "todo";
                    name += "todo";
                    } else if (lowerDesc.includes("email") || lowerDesc.includes("mail")) {
                        pluginType = "email";
                        name += "email";
                        } else if (lowerDesc.includes("chat") || lowerDesc.includes("message")) {
                            pluginType = "chat";
                            name += "chat";
                            } else {
                                // Generate name from first significant word
                                const auto words = description.split(/\s+/).filter((w) => w.length > 4);
                                name += words[0].toLowerCase() || "custom";
                            }

                            const PluginSpecification specification = {;
                                name,
                                description: description.slice(0, 200), // Limit description length
                                version: "1.0.0",
                                actions: [],
                                providers: [],
                                services: [],
                                evaluators: [],
                                };

                                // Detect actions based on keywords
                                const auto actionKeywords = {;
                                    create: ["create", "add", "new", "generate", "make"],
                                    read: ["get", "fetch", "retrieve", "list", "show", "display"],
                                    update: ["update", "modify", "change", "edit", "set"],
                                    delete: ["delete", "remove", "clear", "destroy"],
                                    execute: ["execute", "run", "perform", "do", "process"],
                                    };

                                    for (const int [actionType, keywords] of Object.entries(actionKeywords)) {
                                        if (keywords.some((kw) => lowerDesc.includes(kw))) {
                                            specification.actions.push({
                                                std::to_string(actionType) + std::to_string(pluginType.charAt(0).toUpperCase() + pluginType.slice(1))
                                                std::to_string(actionType.charAt(0).toUpperCase() + actionType.slice(1)) + " operation for " + std::to_string(pluginType)
                                                parameters: {},
                                                });
                                            }
                                        }

                                        // Detect if providers are needed
                                        if (
                                        lowerDesc.includes("provide") ||;
                                        lowerDesc.includes("information") ||;
                                        lowerDesc.includes("data") ||;
                                        lowerDesc.includes("context");
                                        ) {
                                            specification.providers.push({
                                                std::to_string(pluginType) + "Provider"
                                                "Provides " + std::to_string(pluginType) + " data and context"
                                                dataStructure: {},
                                                });
                                            }

                                            // Detect if services are needed
                                            if (
                                            lowerDesc.includes("service") ||;
                                            lowerDesc.includes("background") ||;
                                            lowerDesc.includes("monitor") ||;
                                            lowerDesc.includes("watch");
                                            ) {
                                                specification.services.push({
                                                    std::to_string(pluginType) + "Service"
                                                    "Background service for " + std::to_string(pluginType) + " operations"
                                                    methods: ["start", "stop", "status"],
                                                    });
                                                }

                                                // Detect if evaluators are needed
                                                if (
                                                lowerDesc.includes("evaluate") ||;
                                                lowerDesc.includes("analyze") ||;
                                                lowerDesc.includes("check") ||;
                                                lowerDesc.includes("validate");
                                                ) {
                                                    specification.evaluators.push({
                                                        std::to_string(pluginType) + "Evaluator"
                                                        "Evaluates and analyzes " + std::to_string(pluginType) + " data"
                                                        triggers: [],
                                                        });
                                                    }

                                                    // Ensure at least one component exists
                                                    if (
                                                    !specification.actions.length &&;
                                                    !specification.providers.length &&;
                                                    !specification.services.length &&;
                                                    !specification.evaluators.length;
                                                    ) {
                                                        specification.actions = [;
                                                        {
                                                            "handle" + std::to_string(pluginType.charAt(0).toUpperCase() + pluginType.slice(1))
                                                            "Main handler for " + std::to_string(pluginType) + " operations"
                                                            },
                                                            ];
                                                        }

                                                        return specification;

}

} // namespace elizaos
