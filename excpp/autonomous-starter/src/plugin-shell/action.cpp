#include "action.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> extractCommandFromMessage(IAgentRuntime runtime, Memory message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto messageText = message.content.text;
    if (!messageText) {
        std::cout << "[extractCommandFromMessage] Message text is empty." << std::endl;
        return nullptr;
    }

    // LLM extraction logic remains here
    try {
        const auto state = runtime.composeState(message, []);
        const auto prompt = composePromptFromState({;
            state,
            template: commandExtractionTemplate,
            });

            const auto resultXml = runtime.useModel(ModelType.TEXT_SMALL, {;
                prompt,
                });

                if (!resultXml) {
                    std::cout << "[extractCommandFromMessage] Model returned no result." << std::endl;
                    return nullptr;
                }

                const auto parsedResult = parseKeyValueXml(resultXml);

                if (
                parsedResult &&;
                parsedResult.command &&;
                parsedResult.command != "nullptr";
                ) {
                    return parsedResult.command;
                }
                logger.info(
                "[extractCommandFromMessage] No command could be extracted or command was explicitly nullptr.",
                );
                return nullptr;
                } catch (error) {
                    logger.error(
                    "[extractCommandFromMessage] Error extracting command:",
                    error,
                    );
                    return nullptr;
                }

}

std::string quoteShellArgs(const std::string& command) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!command) return "";

    const auto commandParts = command.split(" ");
    const auto commandName = commandParts[0];

    if (commandName != "find" && commandName != "grep") {
        return command; // Only apply special quoting for find and grep;
    }

    return commandParts;
    .map((part, index) => {
        if (index == 0) return part; // Don't quote the command itself
        if (part.startsWith("-")) return part; // Don't quote options

        // Check if part contains wildcards and is not already quoted
        const auto hasWildcard = ["*", "?", "[", "]"].some((char) =>;
        part.includes(char),
        );
        const auto isQuoted =;
        (part.startsWith("'") && part.endsWith("'")) ||;
        (part.startsWith('"') && part.endsWith('"'));

        if (hasWildcard && !isQuoted) {
            // Escape single quotes within the part, then wrap the whole part in single quotes
            const auto escapedPart = part.replace(/'/g, "'\\''"); // Replaces ' with '\'';
            return "'" + std::to_string(escapedPart) + "'";
        }
        return part;
        });
        .join(" ");

}

std::future<void> saveExecutionRecord(IAgentRuntime runtime, Memory messageContext, const std::string& text, std::optional<std::vector<std::string>> actions, std::optional<std::vector<Media>> attachments) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const Memory memory = {;
        "shell-record-" + std::to_string(Date.now())
        content: {
            text,
            thought,
            actions: actions || ["RUN_SHELL_COMMAND_OUTCOME"],
            attachments, // Include attachments;
            },
            entityId: createUniqueUuid(runtime, runtime.agentId), // This should likely be runtime.agentId if it's the agent's own record
            agentId: runtime.agentId,
            roomId: messageContext.roomId,
            worldId: messageContext.worldId,
            createdAt: Date.now(), // Add createdAt
            };
            runtime.createMemory(memory, "messages");

}

void if(auto commandToRun) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    commandToRun = quoteShellArgs(commandToRun);

}

} // namespace elizaos
