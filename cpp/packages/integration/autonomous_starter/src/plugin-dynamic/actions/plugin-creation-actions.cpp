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
        } else if ((std::find(lowerDesc.begin(), lowerDesc.end(), "database") != lowerDesc.end()) || (std::find(lowerDesc.begin(), lowerDesc.end(), "sql") != lowerDesc.end())) {
            pluginType = "database";
            name += "database";
            } else if ((std::find(lowerDesc.begin(), lowerDesc.end(), "api") != lowerDesc.end()) || (std::find(lowerDesc.begin(), lowerDesc.end(), "rest") != lowerDesc.end())) {
                pluginType = "api";
                name += "api";
                } else if ((std::find(lowerDesc.begin(), lowerDesc.end(), "todo") != lowerDesc.end()) || (std::find(lowerDesc.begin(), lowerDesc.end(), "task") != lowerDesc.end())) {
                    pluginType = "todo";
                    name += "todo";
                    } else if ((std::find(lowerDesc.begin(), lowerDesc.end(), "email") != lowerDesc.end()) || (std::find(lowerDesc.begin(), lowerDesc.end(), "mail") != lowerDesc.end())) {
                        pluginType = "email";
                        name += "email";
                        } else if ((std::find(lowerDesc.begin(), lowerDesc.end(), "chat") != lowerDesc.end()) || (std::find(lowerDesc.begin(), lowerDesc.end(), "message") != lowerDesc.end())) {
                            pluginType = "chat";
                            name += "chat";
                            } else {
                                // Generate name from first significant word
                                const auto words = description.split(/\s+/).filter((w) => w.size() > 4);
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
                                            specification.actions.push_back({
                                                "name: " + actionType + std::to_string(pluginType.charAt(0).toUpperCase() + pluginType.slice(1))
                                                "description: " + std::to_string(actionType.charAt(0).toUpperCase() + actionType.slice(1)) + " operation for " + pluginType
                                                parameters: {},
                                                });
                                            }
                                        }

                                        // Detect if providers are needed
                                        if (
                                        (std::find(lowerDesc.begin(), lowerDesc.end(), "provide") != lowerDesc.end()) ||;
                                        (std::find(lowerDesc.begin(), lowerDesc.end(), "information") != lowerDesc.end()) ||;
                                        (std::find(lowerDesc.begin(), lowerDesc.end(), "data") != lowerDesc.end()) ||;
                                        (std::find(lowerDesc.begin(), lowerDesc.end(), "context") != lowerDesc.end());
                                        ) {
                                            specification.providers.push_back({
                                                "name: " + pluginType + "Provider"
                                                "description: " + "Provides " + pluginType + " data and context"
                                                dataStructure: {},
                                                });
                                            }

                                            // Detect if services are needed
                                            if (
                                            (std::find(lowerDesc.begin(), lowerDesc.end(), "service") != lowerDesc.end()) ||;
                                            (std::find(lowerDesc.begin(), lowerDesc.end(), "background") != lowerDesc.end()) ||;
                                            (std::find(lowerDesc.begin(), lowerDesc.end(), "monitor") != lowerDesc.end()) ||;
                                            (std::find(lowerDesc.begin(), lowerDesc.end(), "watch") != lowerDesc.end());
                                            ) {
                                                specification.services.push_back({
                                                    "name: " + pluginType + "Service"
                                                    "description: " + "Background service for " + pluginType + " operations"
                                                    methods: ["start", "stop", "status"],
                                                    });
                                                }

                                                // Detect if evaluators are needed
                                                if (
                                                (std::find(lowerDesc.begin(), lowerDesc.end(), "evaluate") != lowerDesc.end()) ||;
                                                (std::find(lowerDesc.begin(), lowerDesc.end(), "analyze") != lowerDesc.end()) ||;
                                                (std::find(lowerDesc.begin(), lowerDesc.end(), "check") != lowerDesc.end()) ||;
                                                (std::find(lowerDesc.begin(), lowerDesc.end(), "validate") != lowerDesc.end());
                                                ) {
                                                    specification.evaluators.push_back({
                                                        "name: " + pluginType + "Evaluator"
                                                        "description: " + "Evaluates and analyzes " + pluginType + " data"
                                                        triggers: [],
                                                        });
                                                    }

                                                    // Ensure at least one component exists
                                                    if (
                                                    !specification.actions.size() &&;
                                                    !specification.providers.size() &&;
                                                    !specification.services.size() &&;
                                                    !specification.evaluators.size();
                                                    ) {
                                                        specification.actions = [;
                                                        {
                                                            "name: " + "handle" + std::to_string(pluginType.charAt(0).toUpperCase() + pluginType.slice(1))
                                                            "description: " + "Main handler for " + pluginType + " operations"
                                                            },
                                                            ];
                                                        }

                                                        return specification;

}

} // namespace elizaos
