#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void upgradeDoubleToTriple(auto tpl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUpgradeDoubleToTriple(tpl);

}

std::optional<UUID> validateUuid(unknown value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreValidateUuid(value);

}

UUID stringToUuid(const std::variant<std::string, double>& target) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreStringToUuid(target);

}

std::string truncateToCompleteSentence(const std::string& text, double maxLength) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreTruncateToCompleteSentence(text, maxLength);

}

std::optional<std::unordered_map<std::string, std::any>> parseKeyValueXml(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreParseKeyValueXml(text);

}

std::optional<std::unordered_map<std::string, std::any>> parseJSONObjectFromText(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreParseJSONObjectFromText(text);

}

std::optional<bool> parseBooleanFromText(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreParseBooleanFromText(text);

}

std::function<std::any(std::string, std::any)> safeReplacer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreSafeReplacer();

}

std::future<std::string> trimTokens(const std::string& text, double maxTokens, CoreIAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreTrimTokens(text, maxTokens, runtime);

}

} // namespace elizaos
