#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

import type { ToolPart } from "@/components/action-tool"

/**
 * Convert an agent action message to ToolPart format for display
 * This handles messages with type/sourceType 'agent_action' and parses their rawMessage
 */


  // Map actionStatus to ToolPart state
  const mapActionStatusToState = (status: string): ToolPart["state"] => {
    switch (status) {
      case "pending":
      case "executing":
      case "running":
        return "input-streaming"
      case "completed":
      case "success":
        return "output-available"
      case "failed":
      case "error":
        return "output-error"
      default:
        return "input-available"
    }
  }

  // Get the primary action name (first action or fallback to message type)
  const actionName = rawMessage.actions?.[0] || rawMessage.action || "ACTION"
  const actionStatus = rawMessage.actionStatus || "completed"
  const actionId = rawMessage.actionId

  // Create input data from available action properties
  const inputData: Record<string, unknown> = {}
  inputData.input = rawMessage?.actionResult?.input || {};

  // Create output data based on status and content
  const outputData: Record<string, unknown> = {}
  if (rawMessage.text || message.content) {
    outputData.text = rawMessage.text || message.content
  }
  if (actionStatus) outputData.status = actionStatus
  if (rawMessage.actionResult) outputData.result = rawMessage.actionResult

  // Handle error cases
  const isError = actionStatus === "failed" || actionStatus === "error"
  const errorText = isError ? rawMessage.text || message.content || "Action failed" : undefined

  return {
    type: actionName,
    state: mapActionStatusToState(actionStatus),
    toolCallId: actionId,
    input: Object.keys(inputData).length > 0 ? inputData : undefined,
    output: Object.keys(outputData).length > 0 ? outputData : undefined,
    errorText,
  }
}

/**
 * Check if a message is an agent action message
 */



} // namespace elizaos
