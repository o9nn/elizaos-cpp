#include "village_event_bus.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iostream>
#include <sstream>

namespace elizaos {

using json = nlohmann::json;
using Clock = std::chrono::steady_clock;

static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb,
                                 std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

VillageEventBusClient::VillageEventBusClient(const Config& config)
    : config_(config) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

VillageEventBusClient::~VillageEventBusClient() {
    stop();
    curl_global_cleanup();
}

VillageEventBusClient::VillageEventBusClient(VillageEventBusClient&& other) noexcept
    : config_(std::move(other.config_)),
      running_(other.running_.load()),
      connected_(other.connected_.load()),
      lastKnownTic_(other.lastKnownTic_.load()),
      publishedCount_(other.publishedCount_.load()),
      receivedCount_(other.receivedCount_.load()) {
    other.running_ = false;
}

VillageEventBusClient& VillageEventBusClient::operator=(VillageEventBusClient&& other) noexcept {
    if (this != &other) {
        stop();
        config_ = std::move(other.config_);
        running_ = other.running_.load();
        connected_ = other.connected_.load();
        lastKnownTic_ = other.lastKnownTic_.load();
        publishedCount_ = other.publishedCount_.load();
        receivedCount_ = other.receivedCount_.load();
        other.running_ = false;
    }
    return *this;
}

bool VillageEventBusClient::start() {
    if (running_.load()) return true;

    std::string health = getBusHealth();
    if (health.empty()) {
        std::cerr << "[village-bus] Cannot reach event bus at " << config_.busUrl << std::endl;
    } else {
        connected_ = true;
        try {
            auto j = json::parse(health);
            if (j.contains("tic")) lastKnownTic_ = j["tic"].get<int64_t>();
        } catch (...) {}
    }

    running_ = true;

    json onlinePayload = {
        {"resident", config_.residentName},
        {"capabilities", json::array({"cognitive_loop", "endocrine_system", "homework_loop", "mcp_gateway"})},
        {"version", "elizaos-cpp/1.0"}
    };
    publish("resident.online", onlinePayload.dump());

    if (config_.enableHeartbeat) {
        heartbeatThread_ = std::thread(&VillageEventBusClient::heartbeatLoop, this);
    }
    if (config_.enableWebSocket) {
        wsThread_ = std::thread(&VillageEventBusClient::wsSubscriptionLoop, this);
    }
    return true;
}

void VillageEventBusClient::stop() {
    if (!running_.load()) return;

    json offlinePayload = {
        {"resident", config_.residentName},
        {"published_total", publishedCount_.load()},
        {"received_total", receivedCount_.load()}
    };
    publish("resident.offline", offlinePayload.dump());

    running_ = false;
    connected_ = false;

    if (heartbeatThread_.joinable()) heartbeatThread_.join();
    if (wsThread_.joinable()) wsThread_.join();
}

bool VillageEventBusClient::isConnected() const {
    return connected_.load();
}

int64_t VillageEventBusClient::publish(const std::string& eventType,
                                        const std::string& payload,
                                        const std::string& target) {
    json body;
    body["type"] = eventType;
    body["source"] = config_.residentName;

    try {
        body["payload"] = json::parse(payload);
    } catch (...) {
        body["payload"] = {{"message", payload}};
    }

    if (!target.empty()) body["target"] = target;

    std::string url = config_.busUrl + "/api/events/publish";
    std::string response = httpPost(url, body.dump());

    if (response.empty()) { connected_ = false; return -1; }

    connected_ = true;
    publishedCount_++;

    try {
        auto j = json::parse(response);
        if (j.contains("tic")) {
            int64_t tic = j["tic"].get<int64_t>();
            lastKnownTic_ = tic;
            return tic;
        }
    } catch (...) {}
    return -1;
}

int64_t VillageEventBusClient::publishCognitiveState(const std::string& mode,
                                                       double valence, double arousal,
                                                       int openGoals, int completedGoals) {
    json payload = {
        {"cognitive_mode", mode}, {"valence", valence}, {"arousal", arousal},
        {"open_goals", openGoals}, {"completed_goals", completedGoals},
        {"timestamp_local", std::chrono::duration<double>(Clock::now().time_since_epoch()).count()}
    };
    return publish("eliza.cognitive_state", payload.dump());
}

int64_t VillageEventBusClient::publishHealthReport(const std::string& reportJson) {
    return publish("eliza.health_report", reportJson);
}

int64_t VillageEventBusClient::publishGoalEvent(const std::string& goalId,
                                                  const std::string& action,
                                                  const std::string& topic) {
    json payload = {{"goal_id", goalId}, {"action", action}, {"topic", topic}};
    return publish("eliza.goal." + action, payload.dump());
}

int64_t VillageEventBusClient::publishThought(const std::string& thought,
                                                const std::string& plan) {
    json payload = {{"thought", thought}, {"plan", plan}};
    return publish("eliza.cogloop.thought", payload.dump());
}

void VillageEventBusClient::subscribe(VillageEventCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    globalCallbacks_.push_back(std::move(callback));
}

void VillageEventBusClient::subscribeToType(VillageEventType type,
                                             VillageEventCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    typedCallbacks_[static_cast<int>(type)].push_back(std::move(callback));
}

void VillageEventBusClient::dispatchEvent(const VillageEvent& event) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    for (auto& cb : globalCallbacks_) { try { cb(event); } catch (...) {} }
    auto it = typedCallbacks_.find(static_cast<int>(event.type));
    if (it != typedCallbacks_.end()) {
        for (auto& cb : it->second) { try { cb(event); } catch (...) {} }
    }
}

std::vector<VillageEvent> VillageEventBusClient::getRecentEvents(int limit, int64_t sinceTic) {
    std::string url = config_.busUrl + "/api/events/events?limit=" +
                      std::to_string(limit) + "&since_tic=" + std::to_string(sinceTic);
    std::string response = httpGet(url);
    std::vector<VillageEvent> events;
    if (response.empty()) return events;
    try {
        auto arr = json::parse(response);
        for (auto& item : arr) events.push_back(parseEvent(item.dump()));
    } catch (...) {}
    return events;
}

std::string VillageEventBusClient::getBusHealth() {
    return httpGet(config_.busUrl + "/api/events/health");
}

void VillageEventBusClient::heartbeatLoop() {
    while (running_.load()) {
        json payload = {
            {"resident", config_.residentName},
            {"published", publishedCount_.load()},
            {"received", receivedCount_.load()},
            {"connected", connected_.load()}
        };
        publish("heartbeat", payload.dump());
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.heartbeatIntervalMs));
    }
}

void VillageEventBusClient::wsSubscriptionLoop() {
    int64_t lastSeenTic = lastKnownTic_.load();
    while (running_.load()) {
        std::string url = config_.busUrl + "/api/events/events?limit=20&since_tic=" +
                          std::to_string(lastSeenTic);
        std::string response = httpGet(url);
        if (!response.empty()) {
            connected_ = true;
            try {
                auto arr = json::parse(response);
                for (auto& item : arr) {
                    VillageEvent event = parseEvent(item.dump());
                    if (event.source != config_.residentName) {
                        receivedCount_++;
                        dispatchEvent(event);
                    }
                    if (event.tic > lastSeenTic) {
                        lastSeenTic = event.tic;
                        lastKnownTic_ = lastSeenTic;
                    }
                }
            } catch (...) {}
        } else {
            connected_ = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(330));
    }
}

std::string VillageEventBusClient::httpPost(const std::string& url, const std::string& body) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? response : "";
}

std::string VillageEventBusClient::httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? response : "";
}

VillageEvent VillageEventBusClient::parseEvent(const std::string& jsonStr) {
    VillageEvent event;
    try {
        auto j = json::parse(jsonStr);
        event.id = j.value("id", int64_t(0));
        event.timestamp = j.value("timestamp", 0.0);
        event.typeStr = j.value("event_type", "unknown");
        event.type = parseEventType(event.typeStr);
        event.source = j.value("source", "");
        event.target = j.value("target", "");
        event.hash = j.value("hash", "");
        event.tic = j.value("tic", int64_t(0));
        if (j.contains("payload")) event.payload = j["payload"].dump();
    } catch (...) {}
    return event;
}

VillageEventType VillageEventBusClient::parseEventType(const std::string& typeStr) {
    static const std::unordered_map<std::string, VillageEventType> typeMap = {
        {"heartbeat", VillageEventType::Heartbeat},
        {"eliza.cognitive_state", VillageEventType::CognitiveState},
        {"eliza.goal.created", VillageEventType::GoalUpdate},
        {"eliza.goal.completed", VillageEventType::GoalUpdate},
        {"eliza.goal.failed", VillageEventType::GoalUpdate},
        {"eliza.health_report", VillageEventType::HealthReport},
        {"resident.online", VillageEventType::ResidentJoined},
        {"resident.offline", VillageEventType::ResidentLeft},
        {"bridge.sync", VillageEventType::BridgeSync},
        {"cogloop.thinking", VillageEventType::CogloopThinking},
        {"cogloop.thought", VillageEventType::CogloopThought},
    };
    auto it = typeMap.find(typeStr);
    return (it != typeMap.end()) ? it->second : VillageEventType::Unknown;
}

} // namespace elizaos
