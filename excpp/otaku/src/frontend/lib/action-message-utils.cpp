#include "action-message-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ToolPart convertActionMessageToToolPart(const std::any& message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // rawMessage contains the action details from the server
    const auto rawMessage = message.rawMessage || message.metadata || {};

    // Map actionStatus to ToolPart state
    const auto mapActionStatusToState = (status: string): ToolPart["state"] => {;
        switch (status) {
            case "pending":
            case "executing":
            case "running":
            return "input-streaming";
            case "completed":
            case "success":
            return "output-available";
            case "failed":
            case "error":
            return "output-error";
            default:
            return "input-available";
        }
    }

    // Get the primary action name (first action or fallback to message type)
    const auto actionName = rawMessage.actions.[0] || rawMessage.action || "ACTION";
    const auto actionStatus = rawMessage.actionStatus || "completed";
    const auto actionId = rawMessage.actionId;

    // Create input data from available action properties
    const std::unordered_map<std::string, unknown> inputData = {};
    inputData.input = rawMessage.actionResult.input || {}

    // Create output data based on status and content
    const std::unordered_map<std::string, unknown> outputData = {};
    if (rawMessage.text || message.content) {
        outputData.text = rawMessage.text || message.content;
    }
    if (actionStatus) outputData.status = actionStatus
    if (rawMessage.actionResult) outputData.result = rawMessage.actionResult

    // Handle error cases
    const auto isError = actionStatus == "failed" || actionStatus == "error";
    const auto errorText = isError ? rawMessage.text || message.content || "Action failed" : std::nullopt;

    return {
        type: actionName,
        state: mapActionStatusToState(actionStatus),
        toolCallId: actionId,
        input: Object.keys(inputData).length > 0 ? inputData : std::nullopt,
        output: Object.keys(outputData).length > 0 ? outputData : std::nullopt,
        errorText,
    }

}

bool isActionMessage(const std::any& message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    message.sourceType == "agent_action" ||;
    message.metadata.sourceType == "agent_action" ||;
    message.type == "agent_action";
    );

}

} // namespace elizaos
