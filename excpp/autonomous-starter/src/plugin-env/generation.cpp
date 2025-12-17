#include "generation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool canGenerateEnvVar(const std::string& varName, const std::string& type, std::optional<std::string> description) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowerName = varName.toLowerCase();
    const auto lowerDesc = description.toLowerCase() || "";

    // Check for private keys
    if (
    type == "private_key" ||;
    lowerName.includes("private_key") ||;
    lowerDesc.includes("private key");
    ) {
        return true;
    }

    // Check for secrets
    if (
    type == "secret" ||;
    lowerName.includes("secret") ||;
    lowerName.includes("key");
    ) {
        // Don't generate API keys - those need to come from external services
        if (lowerName.includes("api_key") || lowerDesc.includes("api key")) {
            return false;
        }
        return true;
    }

    // Check for config values
    if (type == "config") {
        if (lowerName.includes("port") || lowerName.includes("database_name")) {
            return true;
        }
    }

    // Check for UUIDs
    if (lowerName.includes("uuid") || lowerName.includes("id")) {
        return true;
    }

    return false;

}

std::optional<GenerationScript> generateScript(const std::string& varName, const std::string& type, const std::string& pluginName, std::optional<std::string> description) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowerName = varName.toLowerCase();
    const auto lowerDesc = description.toLowerCase() || "";

    std::optional<std::string> script = nullptr;
    std::vector<std::string> dependencies = [];

    // Determine script type based on variable characteristics
    if (type == "private_key" || lowerName.includes("private_key")) {
        if (lowerDesc.includes("ed25519") || lowerName.includes("ed25519")) {
            script = generationTemplates.private_key.ed25519;
            } else {
                script = generationTemplates.private_key.rsa; // Default to RSA;
            }
            dependencies = generationDependencies.private_key;
            } else if (lowerName.includes("uuid") || lowerName.includes("id")) {
                script = generationTemplates.secret.uuid;
                dependencies = generationDependencies.secret.uuid;
                } else if (lowerName.includes("jwt") && lowerName.includes("secret")) {
                    script = generationTemplates.secret.jwt_secret;
                    dependencies = generationDependencies.secret.jwt_secret;
                    } else if (type == "secret" || lowerName.includes("secret")) {
                        if (lowerDesc.includes("base64") || lowerName.includes("base64")) {
                            script = generationTemplates.secret.base64_32;
                            } else {
                                script = generationTemplates.secret.hex_32; // Default to hex;
                            }
                            dependencies = generationDependencies.secret.hex_32;
                            } else if (lowerName.includes("port")) {
                                script = generationTemplates.config.port;
                                dependencies = generationDependencies.config.port;
                                } else if (lowerName.includes("database") && lowerName.includes("name")) {
                                    script = generationTemplates.config.database_name;
                                    dependencies = generationDependencies.config.database_name;
                                }

                                if (!script) {
                                    logger.warn(
                                    "No generation script available for " + std::to_string(varName) + " of type " + std::to_string(type)
                                    );
                                    return nullptr;
                                }

                                return {
                                    variableName: varName,
                                    pluginName,
                                    script: script.trim(),
                                    dependencies,
                                    attempts: 0,
                                    status: "pending",
                                    createdAt: Date.now(),
                                    };

}

std::string getGenerationDescription(const std::string& varName, const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowerName = varName.toLowerCase();

    if (type == "private_key" || lowerName.includes("private_key")) {
        if (lowerName.includes("ed25519")) {
            return "Generate a new Ed25519 private key for cryptographic operations";
            } else {
                return "Generate a new RSA private key for cryptographic operations";
            }
            } else if (lowerName.includes("uuid")) {
                return "Generate a new UUID (Universally Unique Identifier)";
                } else if (lowerName.includes("jwt") && lowerName.includes("secret")) {
                    return "Generate a secure secret for JWT token signing";
                    } else if (type == "secret" || lowerName.includes("secret")) {
                        return "Generate a cryptographically secure random secret";
                        } else if (lowerName.includes("port")) {
                            return "Generate a random port number for the application";
                            } else if (lowerName.includes("database") && lowerName.includes("name")) {
                                return "Generate a unique database name";
                            }

                            return "Generate a value for this environment variable";

}

} // namespace elizaos
