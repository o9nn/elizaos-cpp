#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Buffer getWavHeader(double audioLength, double sampleRate, auto channelCount, auto bitsPerSample) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetWavHeader(audioLength, sampleRate, channelCount, bitsPerSample);

}

typeof Readable prependWavHeader(typeof Readable readable, double audioLength, double sampleRate, auto channelCount, auto bitsPerSample) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return corePrependWavHeader(readable, audioLength, sampleRate, channelCount, bitsPerSample);

}

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

} // namespace elizaos
