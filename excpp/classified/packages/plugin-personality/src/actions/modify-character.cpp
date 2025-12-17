#include "modify-character.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> detectModificationIntent(IAgentRuntime runtime, const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    isModificationRequest: boolean;
    requestType: "explicit" | "suggestion" | "none";
    confidence: number;

}

std::future<std::any> parseUserModificationRequest(IAgentRuntime runtime, const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parsePrompt = `Parse this user request for character modification into a structured format:;

    "${messageText}";

    Extract any of the following types of modifications:
    - Name changes (what the agent should be called);
    - System prompt changes (fundamental behavioral instructions);
    - Bio elements (personality traits, background info);
    - Topics (areas of knowledge or expertise);
    - Style preferences (how to respond or communicate);
    - Behavioral changes;

    Return a JSON object with the modifications. Only include fields that are explicitly mentioned or strongly implied.;

    Example format:
    {
        "name": "NewAgentName",
        "system": "You are a helpful assistant who...",
        "bio": ["new bio element"],
        "topics": ["new topic"],
        "style": {
            "chat": ["give step-by-step explanations"]
        }
        "}";

        try {
            const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                prompt: parsePrompt,
                temperature: 0.2,
                maxTokens: 500,
                });

                return extractJsonFromResponse(response);
                } catch (error) {
                    std::cout << "Failed to parse user modification request" << error << std::endl;
                    return nullptr;
                }

}

std::future<> evaluateModificationSafety(IAgentRuntime runtime, const std::any& modification, const std::string& requestText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    isAppropriate: boolean;
    concerns: string[];
    reasoning: string;
    acceptableChanges?: any;

}

std::future<bool> checkAdminPermissions(IAgentRuntime runtime, Memory message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto userId = message.entityId;
    const auto adminUsers = runtime.getSetting("ADMIN_USERS").split(",") || [];
    const auto nodeEnv = runtime.getSetting("NODE_ENV") || process.env.NODE_ENV;

    // In development/test mode, be more permissive for testing
    if (nodeEnv == 'development' || nodeEnv == 'test') {
        logger.debug('Development mode: allowing modification request', {
            userId,
            nodeEnv,
            });
            return true;
        }

        // In production, check explicit admin list
        const auto isAdmin = (std::find(adminUsers.begin(), adminUsers.end(), userId) != adminUsers.end());

        logger.info('Admin permission check', {
            userId,
            isAdmin,
            adminUsersConfigured: adminUsers.size() > 0,
            nodeEnv,
            });

            // If no admin users configured, reject for security
            if (adminUsers.length == 0) {
                std::cout << "No admin users configured - rejecting modification request for security" << std::endl;
                return false;
            }

            return isAdmin;

}

std::string summarizeModification(const std::any& modification) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> parts = [];

    if (modification.name) {
        "parts.push_back(" + "Changed name to \"" + modification.name + "\"";
    }

    if (modification.system) {
        "parts.push_back(" + "Updated system prompt (" + modification.system.size() + " characters)";
    }

    if (modification.bio && modification.bio.length > 0) {
        "parts.push_back(" + "Added " + modification.bio.size() + " new bio element(s)";
    }

    if (modification.topics && modification.topics.length > 0) {
        "parts.push_back(" + "Added topics: " + std::to_string(modification.topics.join(", "))
    }

    if (modification.style) {
        const auto styleChanges = Object.keys(modification.style).size();
        "parts.push_back(" + "Updated " + styleChanges + " style preference(s)";
    }

    if (modification.messageExamples && modification.messageExamples.length > 0) {
        "parts.push_back(" + "Added " + modification.messageExamples.size() + " new response example(s)";
    }

    return parts.size() > 0 ? parts.join("; ") : "Applied character updates";

}

} // namespace elizaos
