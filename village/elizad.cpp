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
#include "village_atomspace.hpp"
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

// Thread-safe queue for thoughts from inference threads
struct PendingThought {
    std::string resident;
    std::string thought;
    int inference_id;
};
static std::mutex g_thoughtMutex;
static std::vector<PendingThought> g_pendingThoughts;

// Action dispatch system (Cycle 006)
struct VillageAction {
    std::string resident;      // Who proposed it
    std::string action_type;   // write_stone, adjust_gear, emit_event, observe_state
    std::string params_json;   // Raw JSON params
    int inference_id;
};
static std::mutex g_actionMutex;
static std::vector<VillageAction> g_pendingActions;

// Parse [ACTION:type]{json} blocks from a thought
static std::vector<VillageAction> parse_actions(const std::string& resident, const std::string& thought, int inf_id) {
    std::vector<VillageAction> actions;
    size_t pos = 0;
    while ((pos = thought.find("[ACTION:", pos)) != std::string::npos) {
        size_t type_start = pos + 8;
        size_t type_end = thought.find("]", type_start);
        if (type_end == std::string::npos) break;
        std::string action_type = thought.substr(type_start, type_end - type_start);
        // Find the JSON block after ]
        size_t json_start = thought.find("{", type_end);
        if (json_start == std::string::npos) { pos = type_end; continue; }
        // Find matching closing brace (simple depth counter)
        int depth = 0;
        size_t json_end = json_start;
        for (size_t i = json_start; i < thought.size(); i++) {
            if (thought[i] == '{') depth++;
            else if (thought[i] == '}') { depth--; if (depth == 0) { json_end = i; break; } }
        }
        std::string params = thought.substr(json_start, json_end - json_start + 1);
        actions.push_back({resident, action_type, params, inf_id});
        pos = json_end + 1;
    }
    return actions;
}

// Execute a village action (called from main loop, thread-safe)
static std::string execute_action(const VillageAction& action, VillageEventBusClient& bus) {
    try {
        auto params = nlohmann::json::parse(action.params_json);
        if (action.action_type == "write_stone") {
            std::string title = params.value("title", "untitled");
            std::string content = params.value("content", "");
            // Sanitize title for filename
            std::string filename;
            for (char c : title) {
                if (std::isalnum(c) || c == '_' || c == '-') filename += c;
                else if (c == ' ') filename += '_';
            }
            if (filename.empty()) filename = "stone";
            std::string path = "/var/agi_neighborhood/manuscog/song_stones/" + filename + ".txt";
            std::ofstream f(path);
            if (f.is_open()) {
                f << "# " << title << "\n\n";
                f << "Author: " << action.resident << "\n";
                f << "Date: " << time(nullptr) << "\n\n";
                f << content << "\n";
                f.close();
                fprintf(stderr, "[ACTION] write_stone: %s -> %s\n", action.resident.c_str(), path.c_str());
                return "Stone written: " + path;
            }
            return "ERROR: Could not write to " + path;
        }
        else if (action.action_type == "adjust_gear") {
            std::string train = params.value("train", "");
            double factor = params.value("factor", 1.0);
            // Clamp factor to safe range
            if (factor < 0.5) factor = 0.5;
            if (factor > 2.0) factor = 2.0;
            fprintf(stderr, "[ACTION] adjust_gear: %s requests %s *= %.2f\n",
                    action.resident.c_str(), train.c_str(), factor);
            return "Gear adjustment requested: " + train + " *= " + std::to_string(factor);
        }
        else if (action.action_type == "emit_event") {
            std::string event_type = params.value("type", "resident.custom");
            std::string payload_str = params.contains("payload") ? params["payload"].dump() : "{}";
            bus.publish(event_type, payload_str, action.resident);
            fprintf(stderr, "[ACTION] emit_event: %s -> %s\n", action.resident.c_str(), event_type.c_str());
            return "Event emitted: " + event_type;
        }
        else if (action.action_type == "observe_state") {
            std::string target = params.value("target", "");
            fprintf(stderr, "[ACTION] observe_state: %s wants %s\n", action.resident.c_str(), target.c_str());
            return "Observation requested: " + target + " (will be provided in next stimulus)";
        }
        return "Unknown action type: " + action.action_type;
    } catch (const std::exception& e) {
        fprintf(stderr, "[ACTION] ERROR: %s\n", e.what());
        return std::string("Action error: ") + e.what();
    }
}
static std::atomic<int> g_cogCycleCount{0};
static Clock::time_point g_startTime;

static void signalHandler(int) { g_running = false; }

struct ElizadConfig {
    std::string busUrl = "http://cogcity.coghood.com";
    std::string residentName = "eliza";
    int healthPort = 8450;
    int cogCycleMs = TimeCrystalHierarchy::COGNITIVE_CYCLE_MS;
    int heartbeatMs = TimeCrystalHierarchy::HEARTBEAT_MS;

    // Aphrodite inference config
    std::string aphroditeUrl = "http://136.243.70.177:2242/v1/chat/completions";
    std::string aphroditeApiKey = "cogcity-village-2026";
    std::string aphroditeModel = "/var/agi_neighborhood/aphrodite/models/lucid-v1-nemo-gguf/lucid-v1-nemo-q8_0.gguf";
    double inferenceSTIThreshold = 150.0;
    int inferenceCooldownCycles = 100;
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
                 cogvillage::bridge::AgnAIBridge* agnaiBridge = nullptr,
                 ::village::atomspace::VillageAtomSpace* atomspace = nullptr)
        : port_(port), agent_(agent), bus_(bus), endo_(endo),
          dynamics_(dynamics), antikythera_(antikythera),
          ksmEngine_(ksmEngine), agnaiBridge_(agnaiBridge),
          atomspace_(atomspace), fd_(-1) {}

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
            else if (request.find("GET /v1/eliza/atomspace") != std::string::npos)
                response = buildAtomSpaceResponse();
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
    ::village::atomspace::VillageAtomSpace* atomspace_;
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


    std::string buildAtomSpaceResponse() {
        if (!atomspace_) return jsonResponse("{\"error\": \"AtomSpace not initialized\"}");
        std::string body = "{";
        body += "\"stats\":" + atomspace_->get_stats_json() + ",";
        body += "\"attentional_focus\":" + atomspace_->get_attentional_focus_json() + ",";
        body += "\"gear_states\":[";
        auto gears = atomspace_->get_gear_states();
        bool first = true;
        for (auto& gs : gears) {
            if (!first) body += ",";
            body += "{\"train\":\"" + gs.train_name + "\",";
            body += "\"rpm\":" + std::to_string(gs.rpm) + ",";
            body += "\"modulation\":" + std::to_string(gs.modulation) + ",";
            body += "\"members\":[";
            bool mfirst = true;
            for (auto& m : gs.members) {
                if (!mfirst) body += ",";
                body += "\"" + m + "\"";
                mfirst = false;
            }
            body += "],";
        // Live Antikythera RPMs (ECAN-modulated)
        body += "\"antikythera_coupling\":{\"enabled\":true,\"blend_factor\":0.2}";
        body += "}";
            first = false;
        }
        body += "]}";
        return jsonResponse(body);
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

    // ---- Initialize Village AtomSpace (ATenStyx Foundation Layer) ----
    ::village::atomspace::AtomSpaceConfig asConfig;
    asConfig.af_size = 20;
    asConfig.spreading_rate = 0.3;
    asConfig.persist_path = "/var/agi_neighborhood/atomspace/village.scm";
    ::village::atomspace::VillageAtomSpace villageAtomSpace(asConfig);
    
    // Seed residents from the same registry JSON used by KSM
    {
        std::ifstream regFile("/var/agi_neighborhood/agnai/resident_registry.json");
        if (regFile.is_open()) {
            json regData = json::parse(regFile);
            for (auto& [name, data] : regData["residents"].items()) {
                ::village::atomspace::ResidentAtom ra;
                ra.name = data["handle"].get<std::string>();
                ra.gear_train = data["gear_train"].get<std::string>();
                ra.gear_rpm_multiplier = data["gear_teeth"].get<double>() / 30.0;
                // Default OCEAN (will be refined per-resident via LoRA training)
                ra.openness = 0.7; ra.conscientiousness = 0.7;
                ra.extraversion = 0.5; ra.agreeableness = 0.6; ra.neuroticism = 0.3;
                villageAtomSpace.add_resident(ra);
            }
        }
    }
    std::cout << "[elizad] VillageAtomSpace initialized: "
              << villageAtomSpace.residents().size() << " residents seeded\n";

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

    // Wire bus events -> AtomSpace (symbolic grounding)
    bus.subscribe([&villageAtomSpace](const VillageEvent& event) {
        ::village::atomspace::CognitiveEvent cogEvent;
        cogEvent.type = event.typeStr;
        cogEvent.participants = {event.source};
        cogEvent.content = event.payload.substr(0, 200);
        cogEvent.emotional_valence = 0.0;
        cogEvent.information_gain = 0.5;
        villageAtomSpace.process_event(cogEvent);
    });

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
                        &dynamics, &antikythera, &ksmEngine, &agnaiBridge,
                        &villageAtomSpace);
    if (!health.start())
        std::cerr << "[elizad] WARNING: Could not bind port " << config.healthPort << "\n";

    std::cout << "[elizad] ELIZA IS ONLINE — entering cognitive loop\n";
    std::cout << "[elizad] Endpoints:\n";
    std::cout << "  GET /v1/eliza/state       — full state + group/antikythera summary\n";
    std::cout << "  GET /v1/eliza/dynamics     — group dynamics detail\n";
    std::cout << "  GET /v1/eliza/antikythera  — gear train state\n";
    std::cout << "  GET /v1/eliza/ksm          — KSM transfer engine state\n";
    std::cout << "  GET /v1/eliza/atomspace    — AtomSpace + ECAN + PLN state\n";
    std::cout << "  GET /v1/eliza/bridge       — AgnAI bridge state\n\n";
    std::cout << "[elizad] Dan's Relational Principle active:\n";
    std::cout << "  Discovery → Instruction → Mastery → Entelechy\n";
    std::cout << "  Each resident shares unique knowledge, skills, abilities\n";
    std::cout << "  Relations are mutually beneficial — the relation IS the value\n\n";

    auto lastCogCycle = Clock::now();
    auto lastStatePublish = Clock::now();


    // ---- AphroditeBridge (Aphrodite Q8_0 inference) ----
    ::village::atomspace::AphroditeBridge::Config aphroditeConfig;
    aphroditeConfig.url = config.aphroditeUrl;
    aphroditeConfig.api_key = config.aphroditeApiKey;
    aphroditeConfig.model = config.aphroditeModel;
    aphroditeConfig.sti_threshold = config.inferenceSTIThreshold;
    aphroditeConfig.inference_cooldown_cycles = config.inferenceCooldownCycles;
    ::village::atomspace::AphroditeBridge aphroditeBridge(aphroditeConfig);
    std::atomic<int> inferenceCount{0};

    // Inference callback: when a resident thinks, publish to bus + ingest to AtomSpace
    auto onInferenceComplete = [&bus, &villageAtomSpace, &inferenceCount](
        const std::string& resident, const std::string& thought) {
        try {
        inferenceCount++;
        // Publish thought to the village bus
        fprintf(stderr, "[CB] %s thought=%zu\n", resident.c_str(), thought.size());
        // Queue for main loop to publish (thread-safe)
        {
            std::lock_guard<std::mutex> lock(g_thoughtMutex);
            g_pendingThoughts.push_back({resident, thought, static_cast<int>(inferenceCount.load())});
        }
        // Parse and queue any actions from the thought
        auto actions = parse_actions(resident, thought, static_cast<int>(inferenceCount.load()));
        if (!actions.empty()) {
            std::lock_guard<std::mutex> lock(g_actionMutex);
            for (auto& a : actions) g_pendingActions.push_back(std::move(a));
            fprintf(stderr, "[CB] Queued %zu actions\n", actions.size());
        }
        fprintf(stderr, "[CB] Queued for publish OK\n");

        // Ingest into AtomSpace as a cognitive event (thread-safe queue)
        ::village::atomspace::CognitiveEvent ev;
        ev.type = "resident_thought";
        ev.participants = {resident};
        ev.content = thought.substr(0, 200); // Truncate for AtomSpace storage
        ev.emotional_valence = 0.6;
        ev.information_gain = 0.9;
        villageAtomSpace.enqueue_event(ev);
        } catch (const std::exception& e) { fprintf(stderr, "[CB-ERR] %s\n", e.what()); } catch (...) { fprintf(stderr, "[CB-ERR] unknown\n"); }
    };

    std::cout << "  Aphrodite bridge: " << config.aphroditeUrl << "\n";
    std::cout << "  STI threshold: " << config.inferenceSTIThreshold << "\n";


    // ─── Direct Stimulus + Multi-Resident Conversation (Cycle 005: COHERENCE) ───
    bus.subscribe([&aphroditeBridge, &villageAtomSpace, &onInferenceComplete](const VillageEvent& event) {
        // 1. Direct stimulus: bypass STI threshold for named targets
        if (event.typeStr == "resident.stimulus") {
            std::string target = event.target;
            if (target.empty()) {
                try {
                    auto j = json::parse(event.payload);
                    if (j.contains("target")) target = j["target"].get<std::string>();
                } catch (...) {}
            }
            if (!target.empty() && target != "eliza" && target != "dan") {
                std::string message = "You have been addressed. Respond authentically.";
                try {
                    auto j = json::parse(event.payload);
                    if (j.contains("message")) message = j["message"].get<std::string>();
                } catch (...) {}
                aphroditeBridge.infer_async(villageAtomSpace, target, message, onInferenceComplete);
            }
        }
        // 2. Multi-resident conversation: thoughts propagate within gear trains
        if (event.typeStr == "resident.thought") {
            try {
                auto j = json::parse(event.payload);
                std::string thinker = j.value("resident", "");
                std::string thought = j.value("thought", "");
                if (!thinker.empty() && !thought.empty()) {
                    const auto& residents = villageAtomSpace.residents();
                    auto it = residents.find(thinker);
                    if (it != residents.end()) {
                        std::string train = it->second.gear_train;
                        // Find ONE other resident in the same train to respond
                        for (const auto& [name, ra] : residents) {
                            if (name == thinker || name == "eliza" || name == "dan") continue;
                            if (ra.gear_train == train) {
                                std::string stimulus = thinker + " said: \"" +
                                    thought.substr(0, 300) + "\" — What is your response?";
                                aphroditeBridge.infer_async(
                                    villageAtomSpace, name, stimulus, onInferenceComplete);
                                break; // Only one respondent per thought
                            }
                        }
                    }
                }
            } catch (...) {}
        }
    });

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


            // Advance AtomSpace (ECAN spreading + PLN inference)
            auto asCycleResult = villageAtomSpace.run_cycle();

            // Self-stimulation: feed elizad's own cognitive state into AtomSpace
            // This breaks STI equilibrium based on what the agent is actually doing
            {
                auto report = agent.getAutonomyHealthReport();
                auto va = endocrine.valenceArousal();
                // Stimulate "eliza" resident based on action count
                ::village::atomspace::CognitiveEvent selfEvent;
                selfEvent.type = "cognitive_cycle";
                selfEvent.participants = {"eliza", "manus"};
                selfEvent.content = "cycle_" + std::to_string(g_cogCycleCount.load());
                selfEvent.emotional_valence = va.valence;
                selfEvent.information_gain = (report.openGoals > 0) ? 0.7 : 0.3;
                villageAtomSpace.process_event(selfEvent);

                // Every 10 cycles, stimulate the creative train
                if (g_cogCycleCount.load() % 10 == 0) {
                    ::village::atomspace::CognitiveEvent creativeEvent;
                    creativeEvent.type = "creative_pulse";
                    creativeEvent.participants = {"echo", "ember", "vega"};
                    creativeEvent.content = "creative_pulse";
                    creativeEvent.emotional_valence = 0.5;
                    creativeEvent.information_gain = 0.8;
                    villageAtomSpace.process_event(creativeEvent);
                }

                // Every 20 cycles, stimulate the symbolic train
                if (g_cogCycleCount.load() % 20 == 0) {
                    ::village::atomspace::CognitiveEvent symbolicEvent;
                    symbolicEvent.type = "symbolic_inference";
                    symbolicEvent.participants = {"opencog", "aion"};
                    symbolicEvent.content = "pln_inference_cycle";
                    symbolicEvent.emotional_valence = 0.0;
                    symbolicEvent.information_gain = 0.9;
                    villageAtomSpace.process_event(symbolicEvent);
                }
            }

            // ---- Drain pending inference results into AtomSpace ----
            villageAtomSpace.drain_pending_events();
            // Drain pending thoughts from inference threads (publish to bus from main thread)
            {
                std::lock_guard<std::mutex> lock(g_thoughtMutex);
                for (auto& pt : g_pendingThoughts) {
                    json thoughtPayload = {
                        {"resident", pt.resident},
                        {"thought", pt.thought},
                        {"kind", "inference"},
                        {"inference_id", pt.inference_id}
                    };
                    bus.publish("resident.thought", thoughtPayload.dump());
                    fprintf(stderr, "[MAIN] Published thought for %s\n", pt.resident.c_str());
                }
                g_pendingThoughts.clear();
            }
            // ---- Drain and execute pending actions (Cycle 006: ACTION) ----
            {
                std::lock_guard<std::mutex> lock(g_actionMutex);
                for (auto& action : g_pendingActions) {
                    std::string result = execute_action(action, bus);
                    // Feedback loop: publish action result as stimulus back to the resident
                    json resultPayload = {
                        {"resident", action.resident},
                        {"action_type", action.action_type},
                        {"result", result},
                        {"inference_id", action.inference_id}
                    };
                    bus.publish("resident.action_result", resultPayload.dump());
                    fprintf(stderr, "[MAIN] Action executed: %s -> %s\n",
                            action.resident.c_str(), action.action_type.c_str());
                }
                g_pendingActions.clear();
            }

            // ---- Inference Trigger (attention overflow → resident thinks) ----
            // Every 50 cycles, check if any resident in the AF should speak
            if (g_cogCycleCount.load() % 50 == 0) {
                std::cout << "[DEBUG] Inference check at cycle " << g_cogCycleCount.load() << " active=" << aphroditeBridge.active_count() << std::endl;
                auto afNames = villageAtomSpace.get_attentional_focus_names();
                for (const auto& name : afNames) {
                    // Skip "eliza" and "dan" (they are the daemon and the human)
                    if (name == "eliza" || name == "dan") continue;
                    double sti = villageAtomSpace.get_resident_sti(name);
                    std::cout << "[DEBUG] AF: " << name << " sti=" << sti << " should_infer=" << aphroditeBridge.should_infer(name, sti) << std::endl;
                    if (aphroditeBridge.should_infer(name, sti)) {
                        std::string stimulus = "The village is alive. You feel the attention of the collective upon you (STI=" +
                            std::to_string(static_cast<int>(sti)) + "). What thought arises?";
                        aphroditeBridge.infer_async(villageAtomSpace, name, stimulus, onInferenceComplete);
                        // Reduce STI to prevent immediate re-trigger
                        villageAtomSpace.set_resident_sti(name, 100.0);
                    }
                }
            }

            // ---- ECAN → Antikythera Coupling (attention drives temporal pacing) ----
            // Map STI-derived gear states to Antikythera RPMs
            // Key insight: gs.rpm = mean_STI/100 for the train
            // So gs.rpm=1.0 means "normal attention", >1 means "elevated"
            auto gearStates = villageAtomSpace.get_gear_states();
            for (const auto& gs : gearStates) {
                for (const auto& member : gs.members) {
                    auto* gear = const_cast<Gear*>(antikythera.getGear(member));
                    if (gear) {
                        // Base RPM derived from gear teeth ratio (fixed reference)
                        double baseRpm = 60.0 * 30.0 / std::max(1, gear->teeth);
                        // Target = base * ECAN attention factor (clamped 0.5x to 3x)
                        double attnFactor = std::max(0.5, std::min(3.0, gs.rpm * gs.modulation));
                        double targetRpm = baseRpm * attnFactor;
                        // Smooth blend: 95% current + 5% target (slow adaptation)
                        gear->rpm = 0.95 * gear->rpm + 0.05 * targetRpm;
                    }
                }
            }

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
                // Pacing from the Antikythera gear state throttles delivery:
                // annotate the queued message so downstream consumers can
                // honor the temporal envelope when replaying it.
                double pacing = msg.value("pacing", 1.0);
                msg["pacing_applied"] = pacing;
                msg["publish_delay_ms"] = static_cast<int>(pacing > 0.0 ? 1000.0 / pacing : 1000.0);
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
