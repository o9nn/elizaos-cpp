#include "utils.hpp"
#include <future>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void upgradeDoubleToTriple(auto tpl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUpgradeDoubleToTriple(tpl);

}

std::optional<UUID> validateUuid(const std:& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreValidateUuid(value);

}

UUID stringToUuid(const std::variant<std:, double>& target) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreStringToUuid(target);

}

std: truncateToCompleteSentence(const std:& text, double maxLength) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreTruncateToCompleteSentence(text, maxLength);

}

std::optional<std::unordered_map<std:, std:>> parseKeyValueXml(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreParseKeyValueXml(text);

}

std::optional<std::unordered_map<std:, std:>> parseJSONObjectFromText(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreParseJSONObjectFromText(text);

}

std::optional<bool> parseBooleanFromText(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreParseBooleanFromText(text);

}

std::function<std:(std:, std:)> safeReplacer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreSafeReplacer();

}

std::future<std:> trimTokens(const std:& text, double maxTokens, CoreIAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreTrimTokens(text, maxTokens, runtime);

}

} // namespace elizaos
