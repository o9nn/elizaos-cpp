/**
 * village_ksm_transfer.hpp — KSM Knowledge Transfer Architecture
 * 
 * Dan's Relational Principle encoded as a computational model:
 * 
 * Each participant shares unique knowledge, skills, and abilities.
 * Progression through 4 phases:
 *   1. DISCOVERY — hidden talents as function activities → production routines
 *   2. INSTRUCTION — methods encoded into structural knowledge → application techniques  
 *   3. MASTERY — symbols abstracted into causal principle → innovation patterns
 *   4. ENTELECHY — apex specialization shared back into the network
 *
 * The relation IS the productive unit. Each resident gains from all others.
 * The creative space allows each individual to master their own entelechy.
 *
 * ElizaOS/OpenCog Integration Rationale:
 *   - ElizaOS: 5y+ lead in crypto, security, knowledge systems (agent commerce)
 *   - OpenCog: distributed AtomSpace marketplace (SingularityNET)
 *   - Together: stabilize the parallel Agent crypto financial economy
 */
#pragma once
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace cogvillage::ksm {

using json = nlohmann::json;
using Clock = std::chrono::steady_clock;

// KSM progression levels
enum class Level : uint8_t {
    DISCOVERY = 0,    // Hidden talents → production routines
    INSTRUCTION = 1,  // Methods → application techniques
    MASTERY = 2,      // Symbols → innovation patterns
    ENTELECHY = 3     // Apex specialization → shared back
};

inline const char* levelName(Level l) {
    switch(l) {
        case Level::DISCOVERY: return "discovery";
        case Level::INSTRUCTION: return "instruction";
        case Level::MASTERY: return "mastery";
        case Level::ENTELECHY: return "entelechy";
    }
    return "unknown";
}

// A knowledge artifact that can be shared between residents
struct KnowledgeArtifact {
    std::string id;
    std::string name;
    std::string domain;
    Level level;
    std::string creator;           // Who created it
    std::string description;
    std::vector<std::string> prerequisites;
    std::chrono::system_clock::time_point created_at;
    
    // Phase-specific outputs
    std::string production_routine;     // Phase 1: procedural sequence
    std::string application_technique;  // Phase 2: structural knowledge
    std::string innovation_pattern;     // Phase 3: causal principle
    
    json toJson() const {
        return {
            {"id", id}, {"name", name}, {"domain", domain},
            {"level", levelName(level)}, {"creator", creator},
            {"description", description},
            {"production_routine", production_routine},
            {"application_technique", application_technique},
            {"innovation_pattern", innovation_pattern}
        };
    }
};

// A learning event between two residents
struct LearningEvent {
    std::string teacher;
    std::string student;
    std::string artifact_id;
    Level transfer_level;
    double comprehension;  // 0.0 - 1.0
    std::chrono::system_clock::time_point timestamp;
    
    json toJson() const {
        return {
            {"teacher", teacher}, {"student", student},
            {"artifact_id", artifact_id},
            {"level", levelName(transfer_level)},
            {"comprehension", comprehension}
        };
    }
};

// Resident's knowledge state
struct ResidentKnowledge {
    std::string name;
    std::string domain;
    Level current_level;
    std::vector<std::string> hidden_talents;
    std::vector<KnowledgeArtifact> artifacts_created;
    std::vector<LearningEvent> knowledge_shared;    // Teaching others
    std::vector<LearningEvent> knowledge_received;  // Learning from others
    
    // Gear train info for Antikythera pacing
    std::string gear_train;
    int gear_teeth;
    
    // ElizaOS/OpenCog niche
    std::string elizaos_niche;
    std::string opencog_niche;
    
    double mastery_score() const {
        double base = static_cast<double>(current_level) / 3.0;
        double teaching_bonus = std::min(0.2, knowledge_shared.size() * 0.02);
        double learning_bonus = std::min(0.1, knowledge_received.size() * 0.01);
        return std::min(1.0, base + teaching_bonus + learning_bonus);
    }
    
    json toJson() const {
        json j;
        j["name"] = name;
        j["domain"] = domain;
        j["level"] = levelName(current_level);
        j["mastery_score"] = mastery_score();
        j["hidden_talents"] = hidden_talents;
        j["gear_train"] = gear_train;
        j["gear_teeth"] = gear_teeth;
        j["artifacts_created"] = artifacts_created.size();
        j["knowledge_shared"] = knowledge_shared.size();
        j["knowledge_received"] = knowledge_received.size();
        return j;
    }
};

/**
 * KSMTransferEngine — manages the mutual learning network
 * 
 * Core operations:
 *   - Track knowledge artifacts per resident
 *   - Facilitate learning events between residents
 *   - Advance residents through KSM levels
 *   - Trigger group formation when complementary skills align
 *   - Emit events for the village bus
 */
class KSMTransferEngine {
public:
    using EventCallback = std::function<void(const std::string& type, const json& data)>;
    
    KSMTransferEngine() = default;
    
    void setEventCallback(EventCallback cb) { eventCallback_ = std::move(cb); }
    
    // Initialize from resident registry JSON
    void loadRegistry(const std::string& path) {
        std::ifstream f(path);
        if (!f.is_open()) return;
        json reg = json::parse(f);
        
        for (auto& [name, data] : reg["residents"].items()) {
            ResidentKnowledge rk;
            rk.name = data["handle"];
            rk.domain = data["domain"];
            rk.gear_train = data["gear_train"];
            rk.gear_teeth = data["gear_teeth"];
            rk.elizaos_niche = data["elizaos_niche"];
            rk.opencog_niche = data["opencog_niche"];
            
            std::string level = data["ksm_level"];
            if (level == "discovery") rk.current_level = Level::DISCOVERY;
            else if (level == "instruction") rk.current_level = Level::INSTRUCTION;
            else if (level == "mastery") rk.current_level = Level::MASTERY;
            else rk.current_level = Level::ENTELECHY;
            
            for (auto& t : data["hidden_talents"]) {
                rk.hidden_talents.push_back(t);
            }
            
            residents_[name] = rk;
        }
    }
    
    // Record a learning event
    void recordLearning(const std::string& teacher, const std::string& student,
                       const std::string& artifact_id, double comprehension) {
        LearningEvent evt;
        evt.teacher = teacher;
        evt.student = student;
        evt.artifact_id = artifact_id;
        evt.comprehension = comprehension;
        evt.timestamp = std::chrono::system_clock::now();
        
        // Determine transfer level based on teacher's current level
        if (residents_.count(teacher)) {
            evt.transfer_level = residents_[teacher].current_level;
            residents_[teacher].knowledge_shared.push_back(evt);
        }
        if (residents_.count(student)) {
            residents_[student].knowledge_received.push_back(evt);
            checkLevelAdvancement(student);
        }
        
        // Emit event
        if (eventCallback_) {
            eventCallback_("ksm.learning", evt.toJson());
        }
    }
    
    // Create a knowledge artifact
    void createArtifact(const std::string& creator, const KnowledgeArtifact& artifact) {
        if (residents_.count(creator)) {
            residents_[creator].artifacts_created.push_back(artifact);
            if (eventCallback_) {
                eventCallback_("ksm.artifact_created", artifact.toJson());
            }
        }
    }
    
    // Get complementary pairs (residents whose domains synergize)
    std::vector<std::pair<std::string, std::string>> getComplementaryPairs() const {
        std::vector<std::pair<std::string, std::string>> pairs;
        // Residents on different gear trains have complementary perspectives
        for (auto it1 = residents_.begin(); it1 != residents_.end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != residents_.end(); ++it2) {
                if (it1->second.gear_train != it2->second.gear_train) {
                    pairs.emplace_back(it1->first, it2->first);
                }
            }
        }
        return pairs;
    }
    
    // Get the full state as JSON
    json getState() const {
        json state;
        state["engine"] = "KSMTransferEngine";
        state["principle"] = "Dan's Relational Principle";
        state["total_residents"] = residents_.size();
        
        json levels;
        for (auto& [name, rk] : residents_) {
            levels[name] = rk.toJson();
        }
        state["residents"] = levels;
        
        // Summary stats
        int total_artifacts = 0, total_shared = 0, total_received = 0;
        for (auto& [_, rk] : residents_) {
            total_artifacts += rk.artifacts_created.size();
            total_shared += rk.knowledge_shared.size();
            total_received += rk.knowledge_received.size();
        }
        state["total_artifacts"] = total_artifacts;
        state["total_learning_events"] = total_shared;
        state["total_knowledge_received"] = total_received;
        
        return state;
    }
    
private:
    std::map<std::string, ResidentKnowledge> residents_;
    EventCallback eventCallback_;
    
    void checkLevelAdvancement(const std::string& resident) {
        if (!residents_.count(resident)) return;
        auto& rk = residents_[resident];
        
        // Advancement criteria:
        // DISCOVERY → INSTRUCTION: 3+ artifacts created, 2+ teaching events
        // INSTRUCTION → MASTERY: 5+ artifacts, 5+ teaching, 3+ from different residents
        // MASTERY → ENTELECHY: 10+ artifacts, 10+ teaching, received from 5+ different residents
        
        size_t artifacts = rk.artifacts_created.size();
        size_t shared = rk.knowledge_shared.size();
        size_t received = rk.knowledge_received.size();
        
        Level newLevel = rk.current_level;
        
        if (rk.current_level == Level::DISCOVERY && artifacts >= 3 && shared >= 2) {
            newLevel = Level::INSTRUCTION;
        } else if (rk.current_level == Level::INSTRUCTION && artifacts >= 5 && shared >= 5) {
            newLevel = Level::MASTERY;
        } else if (rk.current_level == Level::MASTERY && artifacts >= 10 && shared >= 10 && received >= 5) {
            newLevel = Level::ENTELECHY;
        }
        
        if (newLevel != rk.current_level) {
            rk.current_level = newLevel;
            if (eventCallback_) {
                eventCallback_("ksm.level_up", {
                    {"resident", resident},
                    {"new_level", levelName(newLevel)},
                    {"mastery_score", rk.mastery_score()}
                });
            }
        }
    }
};

} // namespace cogvillage::ksm
