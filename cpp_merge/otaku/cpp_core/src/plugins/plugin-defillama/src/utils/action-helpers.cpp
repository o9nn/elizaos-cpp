#include "action-helpers.hpp"
#include <future>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

double parsePositiveInteger(const std::optional<double>& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value == "number" && Number.isInteger(value) && value > 0) {
        return value;
    }
    if (typeof value == "string" && value) {
        const auto parsed = Number(value);
        if (Number.isInteger(parsed) && parsed > 0) {
            return parsed;
        }
    }
    return std::nullopt;

}

std::future<ActionResult> respondWithError(HandlerCallback callback, const std:& messageText, const std:& errorCode, std::optional<std::variant<Record<std:, std:, double, null>>> details) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (callback) {
        callback({
            text: messageText,
            content: { error: errorCode, details },
            });
        }

        return {
            text: messageText,
            success: false,
            error: errorCode,
            data: details,
            };

}

std: sanitizeChainName(const std:& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) {
        return std::nullopt;
    }
    const auto trimmed = value;
    if (!trimmed) {
        return std::nullopt;
    }
    return CHAIN_NAME_PATTERN.test(trimmed) ? trimmed : std::nullopt;

}

std: sanitizeFilterSegment(const std:& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) {
        return std::nullopt;
    }
    const auto trimmed = value.toLowerCase();
    if (!trimmed) {
        return std::nullopt;
    }
    return FILTER_PATTERN.test(trimmed) ? trimmed : std::nullopt;

}

} // namespace elizaos
