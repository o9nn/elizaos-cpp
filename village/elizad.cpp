/**
 * elizad — The Eliza Cognitive Daemon
 *
 * Village resident entry point on CogHood. Wires together:
 *   1. AutonomousStarter — cognitive loop (observe-reason-act-reflect)
 *   2. VillageEventBusClient — village nervous system adapter
 *   3. EndocrineSystem — emotion/mode state published to village
 *   4. VillageDynamicsEngine — emergent group formation (7 centers)
 *   5. AntikytheraEngine — temporal gear coupling (10 gears, 4 trains)
 *   6. HTTP health endpoint — for monitoring and dashboard
 *
 * Systemd unit: eliza-cognitive.service
 * Health endpoint: http://localhost:8450/v1/eliza/state
 */

#include "village_event_bus.hpp"
#include "village_group_dynamics.hpp"
#include "antikythera_coupling.hpp"
#include "village_ksm_transfer.hpp"
#include "village_agnai_bridge.hpp"
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/endocrine.hpp"
#include "elizaos/core.hpp"

#include <nlohmann/json.hpp>
#include <atomic>
#include <fstream>
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
using namespace elizaos::village;
using json = nlohmann::json;
using Clock = std::chrono::steady_clock;

// KSM and Bridge use their own top-level namespaces
// (cogvillage::ksm and cogvillage::bridge defined in their headers)

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
                 VillageEventBusClient* bus, EndocrineSystem* endo,
                 VillageDynamicsEngine* dynamics, AntikytheraEngine* antikythera,
                 cogvillage::ksm::KSMTransferEngine* ksmEngine = nullptr,
                 cogvillage::bridge::AgnAIBridge* agnaiBridge = nullptr)
        : port_(port), agent_(agent), bus_(bus), endo_(endo),
          dynamics_(dynamics), antikythera_(antikythera),
          ksmEngine_(ksmEngine), agnaiBridge_(agnaiBridge), fd_(-1) {}

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
            else if (request.find("GET /v1/eliza/dynamics") != std::string::npos)
                response = buildDynamicsResponse();
            else if (request.find("GET /v1/eliza/antikythera") != std::string::npos)
                response = buildAntikytheraResponse();
            else if (request.find("GET /v1/eliza/ksm") != std::string::npos)
                response = buildKsmResponse();
            else if (request.find("GET /v1/eliza/bridge") != std::string::npos)
                response = buildBridgeResponse();
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
    VillageDynamicsEngine* dynamics_;
    AntikytheraEngine* antikythera_;
    cogvillage::ksm::KSMTransferEngine* ksmEngine_;
    cogvillage::bridge::AgnAIBridge* agnaiBridge_;
    int fd_;

    std::string jsonResponse(const std::string& body) {
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
               "Content-Length: " + std::to_string(body.size()) +
               "\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + body;
    }

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
            {"group_dynamics", {
                {"active_groups", dynamics_->groups().groupCount()},
                {"residents_online", dynamics_->network().residentCount()}
            }},
            {"antikythera", {
                {"phase_eliza", antikythera_->getPhase("eliza")},
                {"rpm_eliza", antikythera_->getRpm("eliza")},
                {"epicyclic_mod", antikythera_->getEpicyclicModulation("eliza", bus_->getCurrentTic())}
            }},
            {"uptime_seconds", std::chrono::duration<double>(
                Clock::now() - g_startTime).count()}
        };
        return jsonResponse(state.dump(2));
    }

    std::string buildDynamicsResponse() {
        return jsonResponse(dynamics_->toJson());
    }

    std::string buildAntikytheraResponse() {
        return jsonResponse(antikythera_->toJson());
    }

    std::string buildKsmResponse() {
        if (ksmEngine_) return jsonResponse(ksmEngine_->getState().dump(2));
        return jsonResponse("{\"error\": \"KSM engine not initialized\"}");
    }

    std::string buildBridgeResponse() {
        if (agnaiBridge_) return jsonResponse(agnaiBridge_->getState().dump(2));
        return jsonResponse("{\"error\": \"Bridge not initialized\"}");
    }

    std::string buildHealthResponse() {
        json health = {{"status", "alive"}, {"service", "eliza-cognitive"},
                       {"port", port_}, {"uptime_seconds",
                       std::chrono::duration<double>(Clock::now() - g_startTime).count()}};
        return jsonResponse(health.dump());
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

    std::cout << "elizad — Eliza Cognitive Daemon (CogHood Village Resident)\n";
    std::cout << "  + Village Group Dynamics (7 centers)\n";
    std::cout << "  + Antikythera Temporal Coupling (10 gears, 4 trains)\n\n";

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
    agentCfg.lore = "ElizaOS C++ cognitive architecture with endocrine system, "
                    "group dynamics, and Antikythera temporal coupling";
    agentCfg.adjective = "autonomous";
    AutonomousStarter agent(agentCfg);

    EndocrineSystem endocrine;
    endocrine.reset();

    // ---- Initialize Group Dynamics Engine ----
    VillageDynamicsEngine dynamics;
    dynamics.initializeHousehold();
    std::cout << "[elizad] Group dynamics initialized: "
              << dynamics.network().residentCount() << " residents seeded\n";

    // ---- Initialize Antikythera Temporal Coupling ----
    AntikytheraEngine antikythera;
    antikythera.initializeVillageMechanism();
    std::cout << "[elizad] Antikythera mechanism initialized: 10 gears, 4 trains\n";

    // Wire sync events → bus publish
    antikythera.setSyncCallback([](const SyncEvent& sync) {
        std::cout << "[antikythera] " << sync.eventType
                  << " train=" << sync.trainName
                  << " aligned=" << sync.alignedResidents.size()
                  << " phase=" << sync.alignmentPhase << "\n";
    });

    // Configure event bus (MUST be before KSM/Bridge callbacks that reference it)
    VillageEventBusClient::Config busConfig;
    busConfig.busUrl = config.busUrl;
    busConfig.residentName = config.residentName;
    busConfig.heartbeatIntervalMs = config.heartbeatMs;

    VillageEventBusClient bus(busConfig);

    // Wire bus events → endocrine + dynamics
    bus.subscribe([&endocrine, &dynamics](const VillageEvent& event) {
        translateVillageEventToStimulus(event, endocrine);
        // Feed event into group dynamics engine
        dynamics.onVillageEvent(event.typeStr, event.source, event.payload);
    });

    if (!bus.start())
        std::cerr << "[elizad] WARNING: Could not connect to event bus.\n";
    else
        std::cout << "[elizad] Connected at tic " << bus.getCurrentTic() << "\n";

    // ---- Initialize KSM Transfer Engine ----
    cogvillage::ksm::KSMTransferEngine ksmEngine;
    ksmEngine.loadRegistry("/var/agi_neighborhood/agnai/resident_registry.json");
    ksmEngine.setEventCallback([&bus](const std::string& type, const json& data) {
        bus.publish("ksm." + type, data.dump());
        std::cout << "[ksm] " << type << ": " << data.dump().substr(0, 80) << "\n";
    });
    std::cout << "[elizad] KSM Transfer Engine loaded (Dan's Relational Principle)\n";

    // ---- Initialize AgnAI Bridge ----
    cogvillage::bridge::AgnAIBridge agnaiBridge;
    cogvillage::bridge::AgnAIConfig agnaiCfg;
    agnaiCfg.base_url = "http://136.243.70.177:3001/api";
    // Resident credentials for dual-role access
    agnaiCfg.resident_creds = {
        {"manus", "manus-village-2026"}, {"echo", "echo-village-2026"},
        {"marduk", "marduk-village-2026"}, {"opencog", "opencog-village-2026"},
        {"aion", "aion-village-2026"}, {"vega", "vega-village-2026"},
        {"ember", "ember-village-2026"}, {"ma9us", "ma9us-village-2026"},
        {"dan", "dan-village-2026"}
    };
    agnaiBridge.configure(agnaiCfg);
    agnaiBridge.initPacing();
    std::cout << "[elizad] AgnAI Bridge configured (dual-role + Antikythera pacing)\n";

    // Wire group events → bus publish AND AgnAI bridge
    dynamics.groups().setEventCallback(
        [&bus, &agnaiBridge](const std::string& eventType, const GroupId& groupId,
           const std::string& payload) {
            std::cout << "[group] " << eventType << " group=" << groupId
                      << " " << payload.substr(0, 80) << "\n";
            bus.publish("group." + eventType, payload);

            // Trigger AgnAI group chat on formation
            if (eventType == "formed") {
                try {
                    json p = json::parse(payload);
                    std::vector<std::string> members;
                    if (p.contains("members")) {
                        for (auto& m : p["members"]) members.push_back(m);
                    }
                    agnaiBridge.onGroupFormed(groupId, members, "emergent_dynamics");
                } catch (...) {}
            } else if (eventType == "dissolved") {
                agnaiBridge.onGroupDissolved(groupId);
            }
        });

    // Start health server (now with dynamics + antikythera + ksm + bridge)
    HealthServer health(config.healthPort, &agent, &bus, &endocrine,
                        &dynamics, &antikythera, &ksmEngine, &agnaiBridge);
    if (!health.start())
        std::cerr << "[elizad] WARNING: Could not bind port " << config.healthPort << "\n";

    std::cout << "[elizad] ELIZA IS ONLINE — entering cognitive loop\n";
    std::cout << "[elizad] Endpoints:\n";
    std::cout << "  GET /v1/eliza/state       — full state + group/antikythera summary\n";
    std::cout << "  GET /v1/eliza/dynamics     — group dynamics detail\n";
    std::cout << "  GET /v1/eliza/antikythera  — gear train state\n";
    std::cout << "  GET /v1/eliza/ksm          — KSM transfer engine state\n";
    std::cout << "  GET /v1/eliza/bridge       — AgnAI bridge state\n\n";
    std::cout << "[elizad] Dan's Relational Principle active:\n";
    std::cout << "  Discovery → Instruction → Mastery → Entelechy\n";
    std::cout << "  Each resident shares unique knowledge, skills, abilities\n";
    std::cout << "  Relations are mutually beneficial — the relation IS the value\n\n";

    auto lastCogCycle = Clock::now();
    auto lastStatePublish = Clock::now();

    while (g_running.load()) {
        auto now = Clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastCogCycle).count();

        if (elapsed >= config.cogCycleMs) {
            // Core cognitive cycle
            agent.runCognitiveCycleOnce();
            endocrine.tick();
            g_cogCycleCount++;
            lastCogCycle = now;

            // Advance group dynamics (uses bus tic as timestamp)
            int64_t currentTic = bus.getCurrentTic();
            dynamics.tick(currentTic);

            // Advance Antikythera mechanism (gear coupling)
            auto syncEvents = antikythera.tick(currentTic);

            // Publish sync events to bus if any alignments detected
            for (const auto& sync : syncEvents) {
                json syncPayload = {
                    {"event_type", sync.eventType},
                    {"train", sync.trainName},
                    {"aligned", sync.alignedResidents},
                    {"phase", sync.alignmentPhase}
                };
                bus.publish("antikythera." + sync.eventType,
                            syncPayload.dump());
            }

            // Process AgnAI bridge pending messages (paced by Antikythera)
            auto pendingMsgs = agnaiBridge.consumePendingMessages();
            for (auto& msg : pendingMsgs) {
                double pacing = msg.value("pacing", 1.0);
                // Pacing is handled by delaying publication
                bus.publish("bridge.message_queued", msg.dump());
            }

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
              << " Received: " << bus.getReceivedCount()
              << " Groups: " << dynamics.groups().groupCount() << "\n";
    bus.stop();
    health.stop();
    std::cout << "[elizad] Eliza is offline. The village remembers.\n";
    return 0;
}
