#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Media generation types
 */
struct MediaGeneration {
    std: id;
    std: mint;
    std: type;
    std: prompt;
    std: mediaUrl;
    std::optional<std:> negativePrompt;
    std::optional<double> numInferenceSteps;
    std::optional<double> seed;
    std::optional<double> numFrames;
    std::optional<double> fps;
    std::optional<double> motionBucketId;
    std::optional<double> duration;
    std::optional<double> durationSeconds;
    std::optional<double> bpm;
    std::optional<std:> creator;
    std: timestamp;
    std::optional<double> dailyGenerationCount;
    std::optional<std:> lastGenerationReset;
};

using TTokenStatus = ;

struct IToken {
    std: id;
    std: name;
    std: ticker;
    std: mint;
    std: creator;
    TTokenStatus status;
    std: createdAt;
    double tokenPriceUSD;
    double marketCapUSD;
    double volume24h;
};

struct ITokenHolder {
    std: id;
    std: mint;
    std: address;
    double amount;
    double percentage;
    std: lastUpdated;
};


} // namespace elizaos
