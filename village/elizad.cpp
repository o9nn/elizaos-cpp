/**
 * elizad — The Eliza Cognitive Daemon
 *
 * Village resident entry point on CogHood. Wires together:
 *   1. AutonomousStarter — cognitive loop (observe-reason-act-reflect)
 *   2. VillageEventBusClient — village nervous system adapter
 *   3. EndocrineSystem — emotion/mode state published to village
 *   4. HTTP health endpoint — for monitoring and dashboard
 *
 * Systemd unit: eliza-cognitive.service
 * Health endpoint: http://localhost:8450/v1/eliza/state
 */

#include "village_event_bus.hpp"
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/endocrine.hpp"
#include "elizaos/core.hpp"

#include <nlohmann/json.hpp>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using namespace elizaos;
using json = nlohmann::json;
using Clock = std::chrono::steady_clock;

static std::atomic<bool> g_running{true};
static std::atomic<int> g_cogCycleCount{0};
static Clock::time_point g_startTime;

static void signalHandler(int) { g_running = false; }

struct ElizadConfig {
    std::string busUrl = "http://cogcity.coghood.com";
    std::string residentName = "eliza";
    int healthPort = 8450;
    int cogCycleMs = TimeCrystalHierarchy::COGNITIVE_CYCLE_MS;
    int heartbeatMs = TimeCrystalHierarchy::HEARTBEAT_MS;

    static ElizadConfig fromEnv() {
        ElizadConfig cfg;
        if (auto* v = std::getenv("ELIZA_BUS_URL")) cfg.busUrl = v;
        if (auto* v = std::getenv("ELIZA_RESIDENT_NAME")) cfg.residentName = v;
        if (auto* v = std::getenv("ELIZA_HEALTH_PORT")) cfg.healthPort = std::atoi(v);
        if (auto* v = std::getenv("ELIZA_COG_CYCLE_MS")) cfg.cogCycleMs = std::atoi(v);
        if (auto* v = std::getenv("ELIZA_HEARTBEAT_MS")) cfg.heartbeatMs = std::atoi(v);
        return cfg;
    }
};

// ============================================================================
// Health Server — minimal HTTP for monitoring
// ============================================================================

class HealthServer {
public:
    HealthServer(int port, AutonomousStarter* agent,
                 VillageEventBusClient* bus, EndocrineSystem* endo)
        : port_(port), agent_(agent), bus_(bus), endo_(endo), fd_(-1) {}

    bool start() {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) return false;
        int opt = 1;
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK);
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port_);
        if (bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(fd_); fd_ = -1; return false;
        }
        listen(fd_, 5);
        return true;
    }

    void poll() {
        if (fd_ < 0) return;
        struct sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int clientFd = accept(fd_, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientFd < 0) return;
        char buf[1024];
        ssize_t n = read(clientFd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            std::string request(buf);
            std::string response;
            if (request.find("GET /v1/eliza/state") != std::string::npos)
                response = buildStateResponse();
            else if (request.find("GET /health") != std::string::npos)
                response = buildHealthResponse();
            else
                response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            (void)write(clientFd, response.c_str(), response.size());
        }
        close(clientFd);
    }

    void stop() { if (fd_ >= 0) { close(fd_); fd_ = -1; } }

private:
    int port_;
    AutonomousStarter* agent_;
    VillageEventBusClient* bus_;
    EndocrineSystem* endo_;
    int fd_;

    std::string buildStateResponse() {
        auto report = agent_->getAutonomyHealthReport();
        auto va = endo_->valenceArousal();
        auto mode = endo_->cognitiveMode();
        json state = {
            {"resident", "eliza"},
            {"status", report.isHealthy ? "healthy" : "degraded"},
            {"cognitive_mode", cognitiveModeName(mode)},
            {"valence", va.valence},
            {"arousal", va.arousal},
            {"open_goals", report.openGoals},
            {"completed_goals", report.completedGoals},
            {"total_actions", report.totalActions},
            {"stagnation_count", report.stagnationCount},
            {"last_plan", report.lastPlan},
            {"cognitive_cycles", g_cogCycleCount.load()},
            {"village_tic", bus_->getCurrentTic()},
            {"events_published", bus_->getPublishedCount()},
            {"events_received", bus_->getReceivedCount()},
            {"bus_connected", bus_->isConnected()},
            {"uptime_seconds", std::chrono::duration<double>(
                Clock::now() - g_startTime).count()}
        };
        std::string body = state.dump(2);
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
               "Content-Length: " + std::to_string(body.size()) +
               "\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + body;
    }

    std::string buildHealthResponse() {
        json health = {{"status", "alive"}, {"service", "eliza-cognitive"},
                       {"port", port_}, {"uptime_seconds",
                       std::chrono::duration<double>(Clock::now() - g_startTime).count()}};
        std::string body = health.dump();
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
               "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
    }
};

// ============================================================================
// Village Event → Endocrine Stimulus Translation
// ============================================================================

static void translateVillageEventToStimulus(const VillageEvent& event,
                                            EndocrineSystem& endo) {
    Stimulus stim;
    if (event.type == VillageEventType::CogloopThinking) {
        stim = Stimulus("curiosity", 0.1);
    } else if (event.type == VillageEventType::ResidentJoined) {
        stim = Stimulus("social_connection", 0.2);
    } else if (event.type == VillageEventType::ResidentLeft) {
        stim = Stimulus("social_loss", 0.1);
    } else if (event.typeStr.find("error") != std::string::npos) {
        stim = Stimulus("threat", 0.15);
    } else if (event.target == "eliza") {
        stim = Stimulus("directed_attention", 0.3);
    } else {
        return;  // No stimulus for unrecognized events
    }
    endo.submitStimulus(stim);
}

// ============================================================================
// Main — The Daemon
// ============================================================================

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    g_startTime = Clock::now();

    std::cout << "elizad — Eliza Cognitive Daemon (CogHood Village Resident)\n\n";

    ElizadConfig config = ElizadConfig::fromEnv();
    std::cout << "[elizad] Bus: " << config.busUrl << "\n";
    std::cout << "[elizad] Resident: " << config.residentName << "\n";
    std::cout << "[elizad] Health port: " << config.healthPort << "\n";
    std::cout << "[elizad] Cognitive cycle: " << config.cogCycleMs << "ms (L8)\n";
    std::cout << "[elizad] Heartbeat: " << config.heartbeatMs << "ms (L7)\n\n";

    // Initialize agent with village config
    AgentConfig agentCfg;
    agentCfg.agentName = config.residentName;
    agentCfg.bio = "Autonomous cognitive agent — CogHood village resident";
    agentCfg.lore = "ElizaOS C++ cognitive architecture with endocrine system";
    agentCfg.adjective = "autonomous";
    AutonomousStarter agent(agentCfg);

    EndocrineSystem endocrine;
    endocrine.reset();

    // Configure event bus
    VillageEventBusClient::Config busConfig;
    busConfig.busUrl = config.busUrl;
    busConfig.residentName = config.residentName;
    busConfig.heartbeatIntervalMs = config.heartbeatMs;

    VillageEventBusClient bus(busConfig);
    bus.subscribe([&endocrine](const VillageEvent& event) {
        translateVillageEventToStimulus(event, endocrine);
    });

    if (!bus.start())
        std::cerr << "[elizad] WARNING: Could not connect to event bus.\n";
    else
        std::cout << "[elizad] Connected at tic " << bus.getCurrentTic() << "\n";

    // Start health server
    HealthServer health(config.healthPort, &agent, &bus, &endocrine);
    if (!health.start())
        std::cerr << "[elizad] WARNING: Could not bind port " << config.healthPort << "\n";

    std::cout << "[elizad] ELIZA IS ONLINE — entering cognitive loop\n\n";

    auto lastCogCycle = Clock::now();
    auto lastStatePublish = Clock::now();

    while (g_running.load()) {
        auto now = Clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastCogCycle).count();

        if (elapsed >= config.cogCycleMs) {
            agent.runCognitiveCycleOnce();
            endocrine.tick();
            g_cogCycleCount++;
            lastCogCycle = now;

            // Publish state every 5 seconds
            auto sincePublish = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastStatePublish).count();
            if (sincePublish >= 5000) {
                auto va = endocrine.valenceArousal();
                auto mode = endocrine.cognitiveMode();
                auto report = agent.getAutonomyHealthReport();
                bus.publishCognitiveState(
                    cognitiveModeName(mode), va.valence, va.arousal,
                    static_cast<int>(report.openGoals),
                    static_cast<int>(report.completedGoals));
                lastStatePublish = now;
            }
        }

        health.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(52));
    }

    std::cout << "\n[elizad] Shutting down. Cycles: " << g_cogCycleCount.load()
              << " Published: " << bus.getPublishedCount()
              << " Received: " << bus.getReceivedCount() << "\n";
    bus.stop();
    health.stop();
    std::cout << "[elizad] Eliza is offline. The village remembers.\n";
    return 0;
}
