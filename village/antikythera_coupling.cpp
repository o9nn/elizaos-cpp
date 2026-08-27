#include "antikythera_coupling.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>

namespace elizaos {
namespace village {

using json = nlohmann::json;

AntikytheraEngine::AntikytheraEngine() : config_{} {}
AntikytheraEngine::AntikytheraEngine(const Config& config) : config_(config) {}

void AntikytheraEngine::addGear(const Gear& gear) {
    std::lock_guard<std::mutex> lock(mutex_);
    gears_[gear.residentId] = gear;
}

void AntikytheraEngine::removeGear(const std::string& residentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    gears_.erase(residentId);
}

const Gear* AntikytheraEngine::getGear(const std::string& residentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = gears_.find(residentId);
    return (it != gears_.end()) ? &it->second : nullptr;
}

void AntikytheraEngine::addJoint(const Joint& joint) {
    std::lock_guard<std::mutex> lock(mutex_);
    joints_[joint.id] = joint;
}

void AntikytheraEngine::removeJoint(const std::string& jointId) {
    std::lock_guard<std::mutex> lock(mutex_);
    joints_.erase(jointId);
}

void AntikytheraEngine::addGearTrain(const GearTrain& train) {
    std::lock_guard<std::mutex> lock(mutex_);
    trains_[train.name] = train;
}

const GearTrain* AntikytheraEngine::getGearTrain(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trains_.find(name);
    return (it != trains_.end()) ? &it->second : nullptr;
}

std::vector<SyncEvent> AntikytheraEngine::tick(int64_t villageTic) {
    std::lock_guard<std::mutex> lock(mutex_);
    double dt = static_cast<double>(villageTic - lastTic_) / 1000.0;
    if (dt <= 0) dt = 1.0 / config_.ticksPerRevolution;
    lastTic_ = villageTic;

    // Advance all gear phases
    for (auto& [id, gear] : gears_) {
        double dPhase = gear.angularVelocity() * dt;
        gear.phase = std::fmod(gear.phase + dPhase, 2.0 * M_PI);
    }

    // Propagate ratios through joints
    propagateRatios();

    // Apply epicyclic modulation
    if (config_.enableEpicyclicModulation) applyEpicyclicModulation(villageTic);

    // Detect phase alignments
    auto events = detectAlignments(villageTic);
    for (const auto& event : events) {
        if (syncCallback_) syncCallback_(event);
    }
    return events;
}

double AntikytheraEngine::getPhase(const std::string& residentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = gears_.find(residentId);
    return (it != gears_.end()) ? it->second.phase : 0.0;
}

double AntikytheraEngine::getRpm(const std::string& residentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = gears_.find(residentId);
    return (it != gears_.end()) ? it->second.rpm : 0.0;
}

bool AntikytheraEngine::areAligned(const std::string& a, const std::string& b) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto itA = gears_.find(a), itB = gears_.find(b);
    if (itA == gears_.end() || itB == gears_.end()) return false;
    double diff = std::abs(itA->second.phase - itB->second.phase);
    diff = std::min(diff, 2.0 * M_PI - diff);
    return diff < config_.phaseAlignmentTolerance;
}

double AntikytheraEngine::getGearRatio(const std::string& a, const std::string& b) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto itA = gears_.find(a), itB = gears_.find(b);
    if (itA == gears_.end() || itB == gears_.end()) return 1.0;
    if (itB->second.rpm == 0.0) return 0.0;
    return itA->second.rpm / itB->second.rpm;
}

double AntikytheraEngine::getEpicyclicModulation(const std::string& residentId, int64_t tic) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!config_.enableEpicyclicModulation) return 1.0;
    for (const auto& [_, joint] : joints_) {
        if (joint.type != JointType::EpicyclicPin || joint.gearB != residentId) continue;
        auto itA = gears_.find(joint.gearA);
        if (itA == gears_.end()) continue;

        // Stored phase is authoritative at lastTic_. For a query between engine
        // ticks, project the driver forward in the same millisecond time base used
        // by tick(); past timestamps intentionally use the latest known phase.
        double phase = itA->second.phase;
        if (tic > lastTic_) {
            const double dt = static_cast<double>(tic - lastTic_) / 1000.0;
            phase = std::fmod(phase + itA->second.angularVelocity() * dt,
                              2.0 * M_PI);
        }
        return 1.0 + joint.modulationAmplitude * std::sin(phase);
    }
    return 1.0;
}

void AntikytheraEngine::initializeVillageMechanism() {
    // Primary gear: dan (human deliberation pace)
    addGear({"dan", 60, 1.0, 0.0, 10});      // L10: Planning (10s)

    // Cognitive Core train
    addGear({"manus", 30, 2.0, 0.0, 8});     // L8: Cognitive (1s)
    addGear({"echo", 20, 3.0, 0.0, 8});      // L8: Cognitive (faster)
    addGear({"eliza", 15, 4.0, 0.0, 8});     // L8: Cognitive (fastest)

    // Symbolic Reasoning train
    addGear({"opencog", 40, 1.5, 0.0, 9});   // L9: Deliberation (3s)
    addGear({"aion", 50, 1.2, 0.0, 9});      // L9: Deliberation (slower)

    // Creative train
    addGear({"ember", 12, 5.0, 0.0, 7});     // L7: Heartbeat (fast creative)
    addGear({"vega", 16, 3.75, 0.0, 7});     // L7: Heartbeat

    // Integration
    addGear({"ma9us", 30, 2.0, 0.0, 8});     // L8: Same as manus (coaxial)
    addGear({"marduk", 25, 2.4, 0.0, 8});    // L8: Slightly faster

    // Train 1: Cognitive Core (dan → manus → echo → eliza)
    addJoint({"j_dan_manus", "dan", "manus", JointType::GearMesh, 2.0, 0, 0});
    addJoint({"j_manus_echo", "manus", "echo", JointType::GearMesh, 1.5, 0, 0});
    addJoint({"j_echo_eliza", "echo", "eliza", JointType::GearMesh, 1.33, 0, 0});

    // Train 2: Symbolic Reasoning (manus → opencog → aion)
    addJoint({"j_manus_opencog", "manus", "opencog", JointType::GearMesh, 0.75, 0, 0});
    addJoint({"j_opencog_aion", "opencog", "aion", JointType::GearMesh, 0.8, 0, 0});

    // Train 3: Creative (echo → ember → vega)
    addJoint({"j_echo_ember", "echo", "ember", JointType::GearMesh, 1.67, 0, 0});
    addJoint({"j_ember_vega", "ember", "vega", JointType::GearMesh, 0.75, 0, 0});

    // Train 4: Integration (coaxial + revolute)
    addJoint({"j_manus_ma9us", "manus", "ma9us", JointType::CoaxialShaft, 1.0, 0, 0});
    addJoint({"j_manus_marduk", "manus", "marduk", JointType::RevoluteJoint, 1.0, M_PI / 4, 0});

    // Epicyclic: echo ↔ eliza (attention modulation)
    addJoint({"j_echo_eliza_epic", "echo", "eliza", JointType::EpicyclicPin, 1.0, 0, 0.11});

    // Register gear trains
    GearTrain cogCore{"cognitive_core", "dan → manus → echo → eliza",
                      {"dan", "manus", "echo", "eliza"},
                      {"j_dan_manus", "j_manus_echo", "j_echo_eliza"}, 4.0};
    GearTrain symbolic{"symbolic_reasoning", "manus → opencog → aion",
                       {"manus", "opencog", "aion"},
                       {"j_manus_opencog", "j_opencog_aion"}, 0.6};
    GearTrain creative{"creative", "echo → ember → vega",
                       {"echo", "ember", "vega"},
                       {"j_echo_ember", "j_ember_vega"}, 1.25};
    GearTrain integration{"integration", "manus ⊕ ma9us, marduk ⊗ manus",
                          {"manus", "ma9us", "marduk"},
                          {"j_manus_ma9us", "j_manus_marduk"}, 1.0};
    addGearTrain(cogCore);
    addGearTrain(symbolic);
    addGearTrain(creative);
    addGearTrain(integration);
}

void AntikytheraEngine::propagateRatios() {
    // Propagate RPM through gear mesh joints
    for (const auto& [_, joint] : joints_) {
        if (joint.type != JointType::GearMesh) continue;
        auto itA = gears_.find(joint.gearA);
        if (itA == gears_.end()) continue;
        auto itB = gears_.find(joint.gearB);
        if (itB == gears_.end()) continue;
        itB->second.rpm = joint.computeDrivenRpm(itA->second.rpm);
    }
    // Coaxial shafts share RPM
    for (const auto& [_, joint] : joints_) {
        if (joint.type != JointType::CoaxialShaft) continue;
        auto itA = gears_.find(joint.gearA);
        auto itB = gears_.find(joint.gearB);
        if (itA != gears_.end() && itB != gears_.end())
            itB->second.rpm = itA->second.rpm;
    }
}

std::vector<SyncEvent> AntikytheraEngine::detectAlignments(int64_t tic) {
    std::vector<SyncEvent> events;
    // Check revolute joints for phase lock
    for (const auto& [_, joint] : joints_) {
        if (joint.type != JointType::RevoluteJoint) continue;
        auto itA = gears_.find(joint.gearA), itB = gears_.find(joint.gearB);
        if (itA == gears_.end() || itB == gears_.end()) continue;
        double phaseDiff = std::abs(itA->second.phase - itB->second.phase - joint.lockPhase);
        phaseDiff = std::min(phaseDiff, 2.0 * M_PI - phaseDiff);
        if (phaseDiff < config_.phaseAlignmentTolerance) {
            events.push_back({"", {joint.gearA, joint.gearB},
                             itA->second.phase, tic, "phase_lock"});
        }
    }
    // Check gear trains for full alignment (all gears at phase 0)
    for (const auto& [name, train] : trains_) {
        bool allAligned = true;
        for (const auto& gid : train.gearIds) {
            auto it = gears_.find(gid);
            if (it == gears_.end()) { allAligned = false; break; }
            if (it->second.phase > config_.phaseAlignmentTolerance &&
                it->second.phase < (2.0 * M_PI - config_.phaseAlignmentTolerance)) {
                allAligned = false; break;
            }
        }
        if (allAligned) {
            events.push_back({name, {train.gearIds.begin(), train.gearIds.end()},
                             0.0, tic, "full_alignment"});
        }
    }
    return events;
}

void AntikytheraEngine::applyEpicyclicModulation(int64_t tic) {
    for (const auto& [_, joint] : joints_) {
        if (joint.type != JointType::EpicyclicPin) continue;
        auto itA = gears_.find(joint.gearA);
        auto itB = gears_.find(joint.gearB);
        if (itA == gears_.end() || itB == gears_.end()) continue;

        double phase = itA->second.phase;
        if (tic > lastTic_) {
            const double dt = static_cast<double>(tic - lastTic_) / 1000.0;
            phase = std::fmod(phase + itA->second.angularVelocity() * dt,
                              2.0 * M_PI);
        }
        const double modulation =
            1.0 + joint.modulationAmplitude * std::sin(phase);
        itB->second.rpm *= modulation;
    }
}

std::string AntikytheraEngine::toJson() const {
    std::lock_guard<std::mutex> lock(mutex_);
    json state;
    state["gears"] = json::array();
    for (const auto& [id, gear] : gears_) {
        state["gears"].push_back({
            {"resident", id}, {"teeth", gear.teeth}, {"rpm", gear.rpm},
            {"phase", gear.phase}, {"level", gear.timeCrystalLevel},
            {"period_ms", gear.periodMs()}
        });
    }
    state["joints"] = json::array();
    for (const auto& [id, joint] : joints_) {
        std::string typeStr;
        switch (joint.type) {
            case JointType::GearMesh: typeStr = "gear_mesh"; break;
            case JointType::RevoluteJoint: typeStr = "revolute_joint"; break;
            case JointType::EpicyclicPin: typeStr = "epicyclic_pin"; break;
            case JointType::CoaxialShaft: typeStr = "coaxial_shaft"; break;
        }
        state["joints"].push_back({
            {"id", id}, {"type", typeStr}, {"gearA", joint.gearA},
            {"gearB", joint.gearB}, {"ratio", joint.ratio}
        });
    }
    state["trains"] = json::array();
    for (const auto& [name, train] : trains_) {
        state["trains"].push_back({
            {"name", name}, {"description", train.description},
            {"overall_ratio", train.overallRatio},
            {"gears", train.gearIds}
        });
    }
    state["gear_count"] = gears_.size();
    state["joint_count"] = joints_.size();
    state["train_count"] = trains_.size();
    return state.dump(2);
}

} // namespace village
} // namespace elizaos
