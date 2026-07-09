#pragma once
/**
 * ElizaOS C++ — Village Event Bus Adapter
 *
 * Connects the autonomous cognitive loop to the CogCity event bus,
 * enabling Eliza to participate as a village resident. This adapter:
 *
 *   1. Publishes cognitive state updates (endocrine mode, valence, arousal)
 *   2. Subscribes to village events via HTTP polling for incoming stimuli
 *   3. Translates village events into Stimulus objects for the endocrine system
 *   4. Maintains a heartbeat at the L7 cadence (500ms) of the time-crystal hierarchy
 *
 * The event bus protocol is simple JSON over HTTP POST (publish) and
 * HTTP GET polling (subscribe). See: cogcity.coghood.com/api/events/
 */

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Time-Crystal Hierarchy (manuscog L0..L8)
// ============================================================================

struct TimeCrystalHierarchy {
    static constexpr double L0 = 0.008;
    static constexpr double L1 = 0.026;
    static constexpr double L2 = 0.052;
    static constexpr double L3 = 0.110;
    static constexpr double L4 = 0.160;
    static constexpr double L5 = 0.250;
    static constexpr double L6 = 0.330;
    static constexpr double L7 = 0.500;
    static constexpr double L8 = 1.000;
    static constexpr int HEARTBEAT_MS = 500;
    static constexpr int COGNITIVE_CYCLE_MS = 1000;
};

// ============================================================================
// Village Event Types
// ============================================================================

enum class VillageEventType {
    Heartbeat,
    CognitiveState,
    GoalUpdate,
    HealthReport,
    StimulusReceived,
    ResidentJoined,
    ResidentLeft,
    BridgeSync,
    CogloopThinking,
    CogloopThought,
    Unknown
};

struct VillageEvent {
    int64_t id = 0;
    double timestamp = 0.0;
    VillageEventType type = VillageEventType::Unknown;
    std::string typeStr;
    std::string source;
    std::string target;
    std::string payload;
    std::string hash;
    int64_t tic = 0;
};

using VillageEventCallback = std::function<void(const VillageEvent&)>;

// ============================================================================
// Village Event Bus Client
// ============================================================================

class VillageEventBusClient {
public:
    struct Config {
        std::string busUrl = "http://cogcity.coghood.com";
        std::string residentName = "eliza";
        int heartbeatIntervalMs = TimeCrystalHierarchy::HEARTBEAT_MS;
        int reconnectIntervalMs = 5000;
        int maxRetries = 3;
        bool enableWebSocket = true;
        bool enableHeartbeat = true;
    };

    explicit VillageEventBusClient(const Config& config = Config{});
    ~VillageEventBusClient();

    VillageEventBusClient(const VillageEventBusClient&) = delete;
    VillageEventBusClient& operator=(const VillageEventBusClient&) = delete;
    VillageEventBusClient(VillageEventBusClient&&) noexcept;
    VillageEventBusClient& operator=(VillageEventBusClient&&) noexcept;

    bool start();
    void stop();
    bool isConnected() const;

    int64_t publish(const std::string& eventType,
                    const std::string& payload,
                    const std::string& target = "");
    int64_t publishCognitiveState(const std::string& mode,
                                   double valence, double arousal,
                                   int openGoals, int completedGoals);
    int64_t publishHealthReport(const std::string& reportJson);
    int64_t publishGoalEvent(const std::string& goalId,
                              const std::string& action,
                              const std::string& topic);
    int64_t publishThought(const std::string& thought, const std::string& plan);

    void subscribe(VillageEventCallback callback);
    void subscribeToType(VillageEventType type, VillageEventCallback callback);

    std::vector<VillageEvent> getRecentEvents(int limit = 50, int64_t sinceTic = 0);
    std::string getBusHealth();
    int64_t getCurrentTic() const { return lastKnownTic_.load(); }
    int64_t getPublishedCount() const { return publishedCount_.load(); }
    int64_t getReceivedCount() const { return receivedCount_.load(); }

private:
    Config config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> connected_{false};
    std::atomic<int64_t> lastKnownTic_{0};
    std::atomic<int64_t> publishedCount_{0};
    std::atomic<int64_t> receivedCount_{0};

    std::thread heartbeatThread_;
    void heartbeatLoop();

    std::thread wsThread_;
    void wsSubscriptionLoop();

    mutable std::mutex callbackMutex_;
    std::vector<VillageEventCallback> globalCallbacks_;
    std::unordered_map<int, std::vector<VillageEventCallback>> typedCallbacks_;
    void dispatchEvent(const VillageEvent& event);

    std::string httpPost(const std::string& url, const std::string& body);
    std::string httpGet(const std::string& url);

    static VillageEvent parseEvent(const std::string& json);
    static VillageEventType parseEventType(const std::string& typeStr);
};

} // namespace elizaos
