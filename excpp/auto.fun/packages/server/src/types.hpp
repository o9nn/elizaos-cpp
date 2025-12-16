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

/**
 * Media generation types
 */
struct MediaGeneration {
    std::string id;
    std::string mint;
    std::string type;
    std::string prompt;
    std::string mediaUrl;
    std::optional<std::string> negativePrompt;
    std::optional<double> numInferenceSteps;
    std::optional<double> seed;
    std::optional<double> numFrames;
    std::optional<double> fps;
    std::optional<double> motionBucketId;
    std::optional<double> duration;
    std::optional<double> durationSeconds;
    std::optional<double> bpm;
    std::optional<std::string> creator;
    std::string timestamp;
    std::optional<double> dailyGenerationCount;
    std::optional<std::string> lastGenerationReset;
};


using TTokenStatus = std::variant<, "pending", "active", "withdrawn", "migrating", "migrated", "locked", "harvested", "migration_failed">;

struct IToken {
    std::string id;
    std::string name;
    std::string ticker;
    std::string mint;
    std::string creator;
    TTokenStatus status;
    std::string createdAt;
    double tokenPriceUSD;
    double marketCapUSD;
    double volume24h;
};


struct ITokenHolder {
    std::string id;
    std::string mint;
    std::string address;
    double amount;
    double percentage;
    std::string lastUpdated;
};


} // namespace elizaos
