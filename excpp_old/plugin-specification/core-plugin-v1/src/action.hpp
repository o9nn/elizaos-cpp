#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "actionExample.hpp"
#include "elizaos/core-plugin-v2.hpp"
#include "state.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Action interface for v1 compatibility
 */
using Action = ActionFromTypes;
using Handler = HandlerFromTypes;
using Validator = ValidatorFromTypes;

/**
 * Converts v2 Handler to v1 compatible Handler
 * V2 handlers have additional parameters like responses array
 */
Handler fromV2Handler(HandlerV2 handlerV2);

/**
 * Converts v1 Handler to v2 Handler
 * V1 handlers don't have responses parameter, so we ignore it
 */
HandlerV2 toV2Handler(Handler handler);

/**
 * Converts v2 Validator to v1 compatible Validator
 * Both have same signature, so just type cast with runtime conversion
 */
Validator fromV2Validator(ValidatorV2 validatorV2);

/**
 * Converts v1 Validator to v2 Validator
 * Both have same signature, so just type cast with state conversion
 */
ValidatorV2 toV2Validator(Validator validator);

/**
 * Converts v2 Action to v1 compatible Action
 * Main differences:
 * - V1 requires similes array, V2 has optional similes
 * - V1 has optional suppressInitialMessage, V2 doesn't
 * - Handler/Validator signatures differ slightly
 */
Action fromV2Action(ActionV2 actionV2);

/**
 * Converts v1 Action to v2 Action
 * Maps v1 action structure to v2 format
 */
ActionV2 toV2Action(Action action);
} // namespace elizaos
