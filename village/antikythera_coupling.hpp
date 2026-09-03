#pragma once
/**
 * Antikythera Temporal Coupling — Multi-resident orchestration via gear-ratio
 *
 * Maps the Antikythera mechanism's physical gear architecture to village
 * resident cognitive cycles. Each resident runs at its own cadence (gear),
 * and the coupling module synchronizes them via gear ratios, revolute joints
 * (synchronous locks), and epicyclic modulation (non-linear attention).
 *
 * Architecture Mapping (from cogpy/antikythera-engine):
 *   Gear Tooth    → Cognitive tick (smallest unit)
 *   Gear Mesh     → Sequential dependency (→)
 *   Gear Ratio    → Temporal scaling between residents
 *   Revolute Joint → Synchronous lock (⊗) — forced alignment
 *   Epicyclic Pin → Non-linear modulation (~⊗~) — attention head
 *   Coaxial Shaft → Concurrent state (⊕) — shared embedding
 *
 * Time Crystal Hierarchy (from village_event_bus.hpp):
 *   L0: Planck (1μs)  L1: Quantum (10μs)  L2: Neural (100μs)
 *   L3: Synaptic (1ms)  L4: Perceptual (10ms)  L5: Motor (100ms)
 *   L6: Attention (330ms)  L7: Heartbeat (500ms)  L8: Cognitive (1000ms)
 *   L9: Deliberation (3s)  L10: Planning (10s)  L11: Session (60s)
 *   L12: Circadian (86400s)
 */

// MSVC's <cmath> only declares M_PI when _USE_MATH_DEFINES is defined before
// the header is pulled in. The build system defines it globally for MSVC, but
// this header is also consumed standalone (tests, tooling), so keep a local
// guard plus a portable fallback constant.
#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif

#include <chrono>
#include <cmath>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace elizaos {
namespace village {

// ============================================================================
// Gear — A single rotating element representing a resident's cognitive cadence
// ============================================================================

struct Gear {
    std::string residentId;
    int teeth;             // Number of teeth (determines ratio)
    double rpm = 1.0;      // Current rotations per minute
    double phase = 0.0;    // Current angular position [0, 2π)
    int timeCrystalLevel;  // Which level in the hierarchy this gear lives at

    // Derived
    double angularVelocity() const { return rpm * 2.0 * M_PI / 60.0; }
    double periodMs() const { return (teeth > 0) ? (60000.0 / rpm) : 1000.0; }
};

// ============================================================================
// Joint — Coupling between two gears
// ============================================================================

enum class JointType {
    GearMesh,       // Sequential: output of A feeds input of B (ratio = teeth_A/teeth_B)
    RevoluteJoint,  // Synchronous: A and B lock at specific phase
    EpicyclicPin,   // Non-linear: sinusoidal modulation of B by A
    CoaxialShaft    // Concurrent: A and B share state (same axis)
};

struct Joint {
    std::string id;
    std::string gearA;     // Driving gear (resident)
    std::string gearB;     // Driven gear (resident)
    JointType type;
    double ratio = 1.0;    // Gear ratio (for GearMesh)
    double lockPhase = 0.0; // Phase alignment (for RevoluteJoint)
    double modulationAmplitude = 0.11; // Epicyclic anomaly (for EpicyclicPin)

    double computeDrivenRpm(double driverRpm) const {
        switch (type) {
            case JointType::GearMesh: return driverRpm * ratio;
            case JointType::RevoluteJoint: return driverRpm; // Same speed, locked phase
            case JointType::CoaxialShaft: return driverRpm;
            case JointType::EpicyclicPin: return driverRpm; // Modulated elsewhere
        }
        return driverRpm;
    }
};

// ============================================================================
// GearTrain — A complete mechanism coupling multiple residents
// ============================================================================

struct GearTrain {
    std::string name;
    std::string description;
    std::vector<std::string> gearIds;  // Ordered from driver to final driven
    std::vector<std::string> jointIds;
    double overallRatio = 1.0;         // Product of all joint ratios
};

// ============================================================================
// Synchronization Event — Emitted when gears align
// ============================================================================

struct SyncEvent {
    std::string trainName;
    std::vector<std::string> alignedResidents;
    double alignmentPhase;
    int64_t tic;
    std::string eventType;  // "phase_lock", "ratio_tick", "epicyclic_peak"
};

// ============================================================================
// Antikythera Engine — The orchestrator
// ============================================================================

class AntikytheraEngine {
public:
    struct Config {
        double primaryRpm = 60.0;  // 1 revolution per second (L8 cognitive)
        double phaseAlignmentTolerance = 0.1;  // radians
        int ticksPerRevolution = 60;
        bool enableEpicyclicModulation = true;
    };

    AntikytheraEngine();
    explicit AntikytheraEngine(const Config& config);

    // Gear management
    void addGear(const Gear& gear);
    void removeGear(const std::string& residentId);
    const Gear* getGear(const std::string& residentId) const;

    // Joint management
    void addJoint(const Joint& joint);
    void removeJoint(const std::string& jointId);

    // Gear train management
    void addGearTrain(const GearTrain& train);
    const GearTrain* getGearTrain(const std::string& name) const;

    // Tick — advance all gears by one time step
    std::vector<SyncEvent> tick(int64_t villageTic);

    // Query
    double getPhase(const std::string& residentId) const;
    double getRpm(const std::string& residentId) const;
    bool areAligned(const std::string& a, const std::string& b) const;
    double getGearRatio(const std::string& a, const std::string& b) const;

    // Epicyclic modulation — returns temperature multiplier for a resident
    double getEpicyclicModulation(const std::string& residentId, int64_t tic) const;

    // Initialize the village mechanism with default gear trains
    void initializeVillageMechanism();

    // Serialize state
    std::string toJson() const;

    // Callback for sync events
    using SyncCallback = std::function<void(const SyncEvent&)>;
    void setSyncCallback(SyncCallback cb) { syncCallback_ = std::move(cb); }

private:
    Config config_;
    std::map<std::string, Gear> gears_;
    std::map<std::string, Joint> joints_;
    std::map<std::string, GearTrain> trains_;
    SyncCallback syncCallback_;
    mutable std::mutex mutex_;
    int64_t lastTic_ = 0;

    void propagateRatios();
    std::vector<SyncEvent> detectAlignments(int64_t tic);
    void applyEpicyclicModulation(int64_t tic);
};

// ============================================================================
// Default Village Mechanism — The CogHood gear train configuration
// ============================================================================

/**
 * The village mechanism maps the 10 residents to gears with ratios
 * derived from their cognitive cadences:
 *
 * Primary Gear: dan (human) — 1 RPM (deliberate, L10 planning scale)
 * Train 1 (Cognitive Core):
 *   dan(60T) → manus(30T) → echo(20T) → eliza(15T)
 *   Ratios: 2:1, 1.5:1, 1.33:1 (each faster than the previous)
 *
 * Train 2 (Symbolic Reasoning):
 *   manus(30T) → opencog(40T) → aion(50T)
 *   Ratios: 0.75:1, 0.8:1 (slower, more deliberate)
 *
 * Train 3 (Creative):
 *   echo(20T) → ember(12T) → vega(16T)
 *   Ratios: 1.67:1, 0.75:1
 *
 * Train 4 (Integration):
 *   manus(30T) ⊕ ma9us(30T) — coaxial (same speed, shared state)
 *   marduk(25T) via revolute joint to manus (phase-locked)
 *
 * Epicyclic Pin: echo ↔ eliza (sinusoidal attention modulation)
 */

} // namespace village
} // namespace elizaos
