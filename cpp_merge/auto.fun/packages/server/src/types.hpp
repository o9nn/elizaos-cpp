#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_TYPES_H
#include "core.hpp"

typedef any TTokenStatus;

class MediaGeneration;
class IToken;
class ITokenHolder;

class MediaGeneration : public object, public std::enable_shared_from_this<MediaGeneration> {
public:
    using std::enable_shared_from_this<MediaGeneration>::shared_from_this;
    string id;

    string mint;

    string type;

    string prompt;

    string mediaUrl;

    string negativePrompt;

    double numInferenceSteps;

    double seed;

    double numFrames;

    double fps;

    double motionBucketId;

    double duration;

    double durationSeconds;

    double bpm;

    string creator;

    string timestamp;

    double dailyGenerationCount;

    string lastGenerationReset;
};

class IToken : public object, public std::enable_shared_from_this<IToken> {
public:
    using std::enable_shared_from_this<IToken>::shared_from_this;
    string id;

    string name;

    string ticker;

    string mint;

    string creator;

    TTokenStatus status;

    string createdAt;

    double tokenPriceUSD;

    double marketCapUSD;

    double volume24h;
};

class ITokenHolder : public object, public std::enable_shared_from_this<ITokenHolder> {
public:
    using std::enable_shared_from_this<ITokenHolder>::shared_from_this;
    string id;

    string mint;

    string address;

    double amount;

    double percentage;

    string lastUpdated;
};

#endif
