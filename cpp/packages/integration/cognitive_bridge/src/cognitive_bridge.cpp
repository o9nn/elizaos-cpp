#include "elizaos/cognitive_bridge.hpp"

#include <algorithm>
#include <iterator>
#include <utility>

namespace elizaos {

namespace {

template <typename Deque>
void pushBounded(Deque& dq, typename Deque::value_type&& v, size_t cap) {
    dq.emplace_back(std::move(v));
    while (dq.size() > cap) dq.pop_front();
}

template <typename Deque>
std::vector<typename Deque::value_type> tail(const Deque& dq, size_t limit) {
    if (dq.empty() || limit == 0) return {};
    const size_t n = std::min(dq.size(), limit);
    std::vector<typename Deque::value_type> out;
    out.reserve(n);
    auto it = dq.end();
    std::advance(it, -static_cast<long>(n));
    for (; it != dq.end(); ++it) out.push_back(*it);
    return out;
}

} // namespace

CognitiveBridge::CognitiveBridge(std::string bridgeId)
    : bridgeId_(std::move(bridgeId)) {}

CognitiveBridge::~CognitiveBridge() {
    stopEchobeats();
}

// -------------------- Publishing --------------------

void CognitiveBridge::publishCognitiveState(const CognitiveState& state) {
    std::vector<CognitiveStateHandler> handlers;
    {
        std::lock_guard<std::mutex> lk(cognitiveSubs_.mu);
        handlers.reserve(cognitiveSubs_.map.size());
        for (auto& [_, h] : cognitiveSubs_.map) handlers.push_back(h);
    }
    {
        std::lock_guard<std::mutex> lk(historyMu_);
        CognitiveState copy = state;
        pushBounded(cognitiveHistory_, std::move(copy), kHistoryCap);
    }
    {
        std::lock_guard<std::mutex> lk(statsMu_);
        ++stats_.cognitivePublished;
    }
    for (auto& h : handlers) h(state);
}

void CognitiveBridge::publishSensoryInput(const SensoryInput& input) {
    std::vector<SensoryInputHandler> handlers;
    {
        std::lock_guard<std::mutex> lk(sensorySubs_.mu);
        handlers.reserve(sensorySubs_.map.size());
        for (auto& [_, h] : sensorySubs_.map) handlers.push_back(h);
    }
    {
        std::lock_guard<std::mutex> lk(historyMu_);
        SensoryInput copy = input;
        pushBounded(sensoryHistory_, std::move(copy), kHistoryCap);
    }
    {
        std::lock_guard<std::mutex> lk(statsMu_);
        ++stats_.sensoryPublished;
    }
    for (auto& h : handlers) h(input);
}

void CognitiveBridge::publishSpeechOutput(const SpeechOutput& speech) {
    std::vector<SpeechOutputHandler> handlers;
    {
        std::lock_guard<std::mutex> lk(speechSubs_.mu);
        handlers.reserve(speechSubs_.map.size());
        for (auto& [_, h] : speechSubs_.map) handlers.push_back(h);
    }
    {
        std::lock_guard<std::mutex> lk(historyMu_);
        SpeechOutput copy = speech;
        pushBounded(speechHistory_, std::move(copy), kHistoryCap);
    }
    {
        std::lock_guard<std::mutex> lk(statsMu_);
        ++stats_.speechPublished;
    }
    for (auto& h : handlers) h(speech);
}

// -------------------- Subscriptions --------------------

SubscriptionId CognitiveBridge::subscribeCognitiveState(CognitiveStateHandler handler) {
    const SubscriptionId id = nextId_.fetch_add(1);
    std::lock_guard<std::mutex> lk(cognitiveSubs_.mu);
    cognitiveSubs_.map.emplace(id, std::move(handler));
    return id;
}

SubscriptionId CognitiveBridge::subscribeSensoryInput(SensoryInputHandler handler) {
    const SubscriptionId id = nextId_.fetch_add(1);
    std::lock_guard<std::mutex> lk(sensorySubs_.mu);
    sensorySubs_.map.emplace(id, std::move(handler));
    return id;
}

SubscriptionId CognitiveBridge::subscribeSpeechOutput(SpeechOutputHandler handler) {
    const SubscriptionId id = nextId_.fetch_add(1);
    std::lock_guard<std::mutex> lk(speechSubs_.mu);
    speechSubs_.map.emplace(id, std::move(handler));
    return id;
}

void CognitiveBridge::unsubscribeCognitiveState(SubscriptionId id) {
    std::lock_guard<std::mutex> lk(cognitiveSubs_.mu);
    cognitiveSubs_.map.erase(id);
}

void CognitiveBridge::unsubscribeSensoryInput(SubscriptionId id) {
    std::lock_guard<std::mutex> lk(sensorySubs_.mu);
    sensorySubs_.map.erase(id);
}

void CognitiveBridge::unsubscribeSpeechOutput(SubscriptionId id) {
    std::lock_guard<std::mutex> lk(speechSubs_.mu);
    speechSubs_.map.erase(id);
}

// -------------------- Echobeats cycle --------------------

int CognitiveBridge::phaseForStep(int step) {
    if (step <= 0) return 0;
    int s = ((step - 1) % 12) + 1;
    return (s - 1) % 4;
}

int CognitiveBridge::currentPhase() const {
    return phaseForStep(currentStep_.load());
}

void CognitiveBridge::startEchobeats(std::chrono::milliseconds stepInterval, std::string agentId) {
    if (echobeatsRunning_.exchange(true)) return;
    echobeatsThread_ = std::thread(&CognitiveBridge::echobeatsLoop, this, stepInterval, std::move(agentId));
}

void CognitiveBridge::stopEchobeats() {
    if (!echobeatsRunning_.exchange(false)) return;
    if (echobeatsThread_.joinable()) echobeatsThread_.join();
}

void CognitiveBridge::echobeatsLoop(std::chrono::milliseconds stepInterval, std::string agentId) {
    int step = 0;
    while (echobeatsRunning_.load()) {
        step = (step % 12) + 1;
        currentStep_.store(step);
        {
            std::lock_guard<std::mutex> lk(statsMu_);
            ++stats_.echobeatsTicks;
        }

        CognitiveState s;
        s.agentId = agentId;
        s.echobeatsStep = step;
        s.echobeatsPhase = phaseForStep(step);
        s.mood = "ticking";
        s.focus = "echobeats";
        publishCognitiveState(s);

        // Sleep in small slices so stop is responsive.
        const auto endTime = std::chrono::steady_clock::now() + stepInterval;
        while (echobeatsRunning_.load() && std::chrono::steady_clock::now() < endTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

// -------------------- History --------------------

std::vector<CognitiveState> CognitiveBridge::recentCognitiveStates(size_t limit) const {
    std::lock_guard<std::mutex> lk(historyMu_);
    return tail(cognitiveHistory_, limit);
}

std::vector<SensoryInput> CognitiveBridge::recentSensoryInputs(size_t limit) const {
    std::lock_guard<std::mutex> lk(historyMu_);
    return tail(sensoryHistory_, limit);
}

std::vector<SpeechOutput> CognitiveBridge::recentSpeechOutputs(size_t limit) const {
    std::lock_guard<std::mutex> lk(historyMu_);
    return tail(speechHistory_, limit);
}

CognitiveBridge::Stats CognitiveBridge::stats() const {
    std::lock_guard<std::mutex> lk(statsMu_);
    return stats_;
}

} // namespace elizaos
