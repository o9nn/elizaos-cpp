// village_atomspace.hpp — OpenCog-hpp integration for the CogVerse village
// Wires AtomSpace + ECAN + PLN + URE into elizad's cognitive loop
// Part of the ATenStyx foundation layer
//
// Architecture:
//   OpenCog (symbolic) drives Aphrodite/Lucid (subsymbolic)
//   AtomSpace IS the inference engine
//   ECAN drives attention allocation (maps to Antikythera gear RPMs)
//   PLN validates and constrains generation
//   URE provides forward/backward chaining
//   Pattern Miner discovers recurring structures → LoRA training data
//
// The Ghost in the Guile Shell: raw Atomese below the membrane
//
#pragma once

#include <oc/oc.hpp>  // The full opencog-hpp umbrella

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <deque>
#include <atomic>
#include <optional>
#include <unordered_map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <ctime>

namespace village { namespace atomspace {

// ─────────────────────────────────────────────────────────────────────
// Configuration
// ─────────────────────────────────────────────────────────────────────

struct AtomSpaceConfig {
    size_t af_size = 50;              // Attentional Focus size
    double spreading_rate = 0.3;      // ECAN spreading activation rate
    double forgetting_threshold = 5;  // STI below which atoms are forgotten
    size_t max_atoms = 100000;        // Maximum atoms before garbage collection
    double hebbian_learning_rate = 0.1;
    // The daemon's authoritative, versioned runtime state is JSON. Scheme is
    // an intentional interoperability export and is never auto-loaded as the
    // daemon state format.
    std::string persist_path = "/var/agi_neighborhood/atomspace/village.json";
    std::string scheme_export_path = "/var/agi_neighborhood/atomspace/village.scm";
    size_t max_persist_bytes = 8 * 1024 * 1024;
    
    // PLN parameters
    double deduction_confidence_threshold = 0.5;
    double abduction_prior = 0.1;
    size_t max_chain_depth = 5;
    
    // MOSES parameters (for future program evolution)
    size_t moses_population = 100;
    size_t moses_generations = 50;
    double moses_fitness_threshold = 0.8;
};

// ─────────────────────────────────────────────────────────────────────
// Resident Atom — each resident is a ConceptNode with rich metadata
// ─────────────────────────────────────────────────────────────────────

struct ResidentAtom {
    std::string name;
    oc::Handle concept_handle;
    
    // OCEAN personality as TruthValues on InheritanceLinks
    double openness;
    double conscientiousness;
    double extraversion;
    double agreeableness;
    double neuroticism;
    
    // ECAN state
    double sti;  // Short-Term Importance (current attention)
    double lti;  // Long-Term Importance (persistent relevance)
    
    // KSM progression
    enum class KSMLevel { DISCOVERY, INSTRUCTION, MASTERY, ENTELECHY };
    std::map<std::string, KSMLevel> domain_levels;
    
    // Gear train mapping (for Antikythera)
    std::string gear_train;  // "creative", "symbolic", "integration", "core"
    double gear_rpm_multiplier;
};

// ─────────────────────────────────────────────────────────────────────
// Knowledge Artifact — produced by KSM learning events
// ─────────────────────────────────────────────────────────────────────

struct KnowledgeArtifact {
    std::string id;
    std::string producer;       // Resident who created it
    std::string domain;         // Domain of knowledge
    std::string type;           // "routine", "technique", "pattern"
    double confidence;          // PLN TruthValue strength
    double count;               // PLN TruthValue confidence
    std::vector<std::string> consumers;  // Residents who learned from it
    std::chrono::system_clock::time_point created;
};

// ─────────────────────────────────────────────────────────────────────
// Cognitive Event — everything that happens in the village is an Atom
// ─────────────────────────────────────────────────────────────────────

struct CognitiveEvent {
    std::string type;           // "utterance", "learning", "group_form", "inference"
    std::vector<std::string> participants;
    std::string content;
    double emotional_valence;   // -1.0 to 1.0
    double information_gain;    // How much new info this event carries
    oc::Handle atom_handle;     // The event stored as an Atom
};

// ─────────────────────────────────────────────────────────────────────
// VillageAtomSpace — the cognitive substrate
// ─────────────────────────────────────────────────────────────────────

class VillageAtomSpace {
public:
    explicit VillageAtomSpace(const AtomSpaceConfig& config = {})
        : config_(config), as_(), ecan_(as_), pln_(as_), cycle_count_(0),
          persist_path_(config.persist_path)
    {
        // Preserve the historical convention that callers supplying a .scm
        // persist_path are configuring the Scheme interoperability export.
        // JSON daemon state remains independently selected through
        // set_persist_path() or a non-.scm AtomSpaceConfig::persist_path.
        if (std::filesystem::path(config.persist_path).extension() == ".scm")
            config_.scheme_export_path = config.persist_path;
        // Configure ECAN
        ecan_.bank().set_af_threshold(static_cast<oc::AttentionValue::sti_t>(config.af_size));
        
        // Seed the type hierarchy
        seed_type_hierarchy();
    }
    
    // ─── Resident Management ───────────────────────────────────────
    
    void add_resident(const ResidentAtom& resident) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Create ConceptNode for resident
        oc::Handle h = as_.add_node(oc::types::CONCEPT_NODE, resident.name,
                                    oc::TruthValue(0.9, 0.9));
        
        // Store OCEAN as InheritanceLinks to trait concepts
        store_trait(resident.name, "openness", resident.openness);
        store_trait(resident.name, "conscientiousness", resident.conscientiousness);
        store_trait(resident.name, "extraversion", resident.extraversion);
        store_trait(resident.name, "agreeableness", resident.agreeableness);
        store_trait(resident.name, "neuroticism", resident.neuroticism);
        
        // Store gear train membership
        oc::Handle gear_node = as_.add_node(oc::types::CONCEPT_NODE, 
                                            "gear:" + resident.gear_train);
        as_.add_link(oc::types::MEMBER_LINK, {h, gear_node});
        
        // Initial ECAN stimulation
        ecan_.bank().stimulate(h, 100);
        
        // Store in local registry
        ResidentAtom r = resident;
        r.concept_handle = h;
        r.sti = 100.0;
        r.lti = 50.0;
        residents_[resident.name] = r;
    }
    
    // ─── Cognitive Cycle (called every elizad tic) ─────────────────
    
    struct CycleResult {
        std::vector<std::string> attentional_focus;  // Residents in AF
        std::vector<std::string> inferences;         // PLN conclusions this cycle
        double total_sti;                            // Total STI in system
        size_t atom_count;                           // Total atoms
        size_t cycle_number;
    };
    
    CycleResult run_cycle() {
        std::lock_guard<std::mutex> lock(mutex_);
        CycleResult result;
        result.cycle_number = ++cycle_count_;
        
        // 1. ECAN spreading activation
        ecan_.run(1);
        
        // 2. Get attentional focus
        auto af = ecan_.bank().get_attentional_focus();
        for (auto& h : af) {
            const oc::Atom* atom = as_.get_atom(h);
            if (atom && !atom->name.empty() && residents_.count(atom->name)) {
                result.attentional_focus.push_back(atom->name);
                residents_[atom->name].sti = static_cast<double>(atom->av.sti);
            }
        }
        
        // 3. PLN forward chaining on AF atoms (bounded)
        if (!af.empty() && cycle_count_ % 10 == 0) {  // Every 10th cycle
            auto conclusions = pln_.deduce_all();
            for (auto& c : conclusions) {
                if (c.tv.mean > config_.deduction_confidence_threshold) {
                    result.inferences.push_back(c.description + " [" + 
                        std::to_string(c.tv.mean) + "," + 
                        std::to_string(c.tv.confidence) + "]");
                }
            }
        }
        
        // 4. Hebbian learning: strengthen links between co-attended atoms
        if (result.attentional_focus.size() >= 2) {
            for (size_t i = 0; i < result.attentional_focus.size(); ++i) {
                for (size_t j = i + 1; j < result.attentional_focus.size(); ++j) {
                    strengthen_hebbian(result.attentional_focus[i], 
                                     result.attentional_focus[j]);
                }
            }
        }
        
        // 5. Forgetting agent: decay low-STI atoms
        if (cycle_count_ % 100 == 0) {
            forget_low_sti();
        }
        
        result.total_sti = compute_total_sti();
        result.atom_count = as_.size();
        
        return result;
    }
    
    // ─── Event Processing ──────────────────────────────────────────
    
    void process_event(const CognitiveEvent& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Create event atom
        oc::Handle event_node = as_.add_node(oc::types::CONCEPT_NODE, 
            "event:" + event.type + ":" + std::to_string(cycle_count_),
            oc::TruthValue(0.9, 0.7));
        
        // Link participants
        for (auto& participant : event.participants) {
            if (residents_.count(participant)) {
                oc::Handle p_handle = residents_[participant].concept_handle;
                oc::Handle pred_h = as_.add_node(oc::types::PREDICATE_NODE, 
                                                 "participates-in");
                oc::Handle list_h = as_.add_link(oc::types::LIST_LINK, 
                                                 {p_handle, event_node});
                as_.add_link(oc::types::EVALUATION_LINK, {pred_h, list_h});
                
                // Stimulate participants (they gain attention from events)
                ecan_.bank().stimulate(p_handle, 
                    static_cast<oc::AttentionValue::sti_t>(event.information_gain * 50));
            }
        }
        
        // Store content as a SchemaNode (for future MOSES evolution)
        if (!event.content.empty()) {
            oc::Handle content_node = as_.add_node(oc::types::SCHEMA_NODE, 
                                                   event.content);
            oc::Handle pred_h = as_.add_node(oc::types::PREDICATE_NODE, 
                                             "has-content");
            oc::Handle list_h = as_.add_link(oc::types::LIST_LINK, 
                                             {event_node, content_node});
            as_.add_link(oc::types::EVALUATION_LINK, {pred_h, list_h});
        }
        
        // Store emotional valence as a NumberNode
        oc::Handle valence_node = as_.add_node(oc::types::NUMBER_NODE, 
            std::to_string(event.emotional_valence));
        oc::Handle val_pred = as_.add_node(oc::types::PREDICATE_NODE, "has-valence");
        oc::Handle val_list = as_.add_link(oc::types::LIST_LINK, 
                                           {event_node, valence_node});
        as_.add_link(oc::types::EVALUATION_LINK, {val_pred, val_list});
        
        events_.push_back(event);
    }
    
    // ─── Knowledge Transfer (Dan's Relational Principle) ───────────
    
    void record_learning(const std::string& teacher, 
                        const std::string& learner,
                        const std::string& domain,
                        double effectiveness) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Create teaching event in AtomSpace
        oc::Handle teacher_h = residents_.count(teacher) ? 
            residents_[teacher].concept_handle : 
            as_.add_node(oc::types::CONCEPT_NODE, teacher);
        oc::Handle learner_h = residents_.count(learner) ? 
            residents_[learner].concept_handle : 
            as_.add_node(oc::types::CONCEPT_NODE, learner);
        oc::Handle domain_h = as_.add_node(oc::types::CONCEPT_NODE, 
                                           "domain:" + domain);
        
        // EvaluationLink: teaches(teacher, learner, domain)
        oc::Handle pred_h = as_.add_node(oc::types::PREDICATE_NODE, "teaches");
        oc::Handle list_h = as_.add_link(oc::types::LIST_LINK, 
                                         {teacher_h, learner_h, domain_h});
        as_.add_link(oc::types::EVALUATION_LINK, {pred_h, list_h}, 
                     oc::TruthValue(effectiveness, 0.8));
        
        // Strengthen the teacher-learner Hebbian link
        strengthen_hebbian(teacher, learner);
        
        // PLN can now infer: if A teaches B domain X, and B teaches C domain X,
        // then knowledge of X has propagated from A to C
        pln_.store_inheritance(learner + ":" + domain, 
                             teacher + ":" + domain, 
                             effectiveness * 0.9, 0.7);
        
        // Update KSM level for teacher (teaching = instruction level)
        if (residents_.count(teacher)) {
            auto& levels = residents_[teacher].domain_levels;
            if (levels.find(domain) == levels.end() || 
                levels[domain] < ResidentAtom::KSMLevel::INSTRUCTION) {
                levels[domain] = ResidentAtom::KSMLevel::INSTRUCTION;
            }
        }
    }
    
    // ─── ECAN → Antikythera Bridge ────────────────────────────────
    // Maps STI values to gear RPM multipliers
    
    struct GearState {
        std::string train_name;
        double rpm;              // Derived from mean STI of train members
        double modulation;       // Epicyclic modulation from cross-train attention
        std::vector<std::string> members;
    };
    
    std::vector<GearState> get_gear_states() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, std::vector<const ResidentAtom*>> trains;
        for (auto& kv : residents_) {
            trains[kv.second.gear_train].push_back(&kv.second);
        }
        
        std::vector<GearState> states;
        double total_mean_sti = 0;
        size_t train_count = 0;
        
        for (auto& kv : trains) {
            GearState gs;
            gs.train_name = kv.first;
            double sum_sti = 0;
            for (auto* m : kv.second) {
                sum_sti += m->sti;
                gs.members.push_back(m->name);
            }
            double mean_sti = kv.second.empty() ? 0 : sum_sti / kv.second.size();
            
            // Map STI to RPM: base 1.0 RPM, scaled by STI/100
            gs.rpm = std::max(0.1, mean_sti / 100.0);
            gs.modulation = 1.0;  // Will be computed after all trains are known
            
            total_mean_sti += mean_sti;
            train_count++;
            states.push_back(gs);
        }
        
        // Epicyclic modulation: each train's RPM is modulated by the
        // ratio of its STI to the global mean
        double global_mean = train_count > 0 ? total_mean_sti / train_count : 1.0;
        for (auto& gs : states) {
            gs.modulation = gs.rpm / std::max(0.01, global_mean / 100.0);
        }
        
        return states;
    }
    
    // ─── Query Interface (for 9P server / HTTP endpoints) ──────────
    
    std::string get_resident_sti_str(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = residents_.find(name);
        if (it == residents_.end()) return "unknown";
        return std::to_string(it->second.sti);
    }
    
    std::string get_attentional_focus_json() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string json = "[";
        auto af = ecan_.bank().get_attentional_focus();
        bool first = true;
        for (auto& h : af) {
            const oc::Atom* atom = as_.get_atom(h);
            if (atom && !atom->name.empty()) {
                if (!first) json += ",";
                json += "\"" + atom->name + "\"";
                first = false;
            }
        }
        json += "]";
        return json;
    }
    
    std::string get_stats_json() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string json = "{";
        json += "\"atom_count\":" + std::to_string(as_.size()) + ",";
        json += "\"cycle_count\":" + std::to_string(cycle_count_) + ",";
        json += "\"resident_count\":" + std::to_string(residents_.size()) + ",";
        json += "\"total_sti\":" + std::to_string(compute_total_sti()) + ",";
        json += "\"event_count\":" + std::to_string(events_.size()) + ",";
        json += "\"af_size\":" + std::to_string(
            ecan_.bank().get_attentional_focus().size());
        json += "}";
        return json;
    }
    
    // ─── PLN Query (for the Guile Shell interface) ─────────────────
    
    struct InferenceResult {
        std::string conclusion;
        double strength;
        double confidence;
    };
    
    std::vector<InferenceResult> query_inheritance_chain(
        const std::string& from, const std::string& to) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<InferenceResult> results;
        
        auto tv = pln_.query_inheritance(from, to);
        if (tv.mean > 0) {
            InferenceResult ir;
            ir.conclusion = from + " -> " + to;
            ir.strength = tv.mean;
            ir.confidence = tv.confidence;
            results.push_back(ir);
        }
        
        return results;
    }
    
    // ─── Scheme interoperability export ───────────────────────────
    // save()/load() intentionally retain the historical Scheme API for the
    // Guile shell. The daemon itself uses persist()/load_persisted() below,
    // whose versioned JSON path is configured independently.
    bool save() {
        std::lock_guard<std::mutex> lock(mutex_);
        namespace fs = std::filesystem;
        try {
            fs::path path(config_.scheme_export_path);
            if (path.has_parent_path()) {
                std::error_code ec;
                fs::create_directories(path.parent_path(), ec);
                if (ec) return false;
            }
            oc::persist::Serializer ser(as_);
            // Write atomically: serialize to a temp file, then rename.
            fs::path tmp = path;
            tmp += ".tmp";
            {
                std::ofstream out(tmp, std::ios::trunc);
                if (!out) return false;
                out << ";; VillageAtomSpace snapshot (Atomese s-expressions)\n";
                out << ";; cycles: " << cycle_count_
                    << " residents: " << residents_.size() << "\n";
                out << ser.serialize_atomspace();
                if (!out.good()) return false;
            }
            std::error_code ec;
            fs::rename(tmp, path, ec);
            if (ec) {
                // Fall back to copy+remove across filesystems.
                fs::copy_file(tmp, path, fs::copy_options::overwrite_existing, ec);
                fs::remove(tmp);
                if (ec) return false;
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    // Load an intentional Scheme export and re-bind resident handles. This
    // is separate from the daemon JSON state and remains merge-oriented for
    // compatibility with existing Guile workflows.
    size_t load() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ifstream in(config_.scheme_export_path);
        if (!in) return 0;
        oc::persist::Serializer ser(as_);
        size_t restored = 0;
        std::string line;
        while (std::getline(in, line)) {
            // Skip comments and blank lines.
            size_t first = line.find_first_not_of(" \t");
            if (first == std::string::npos || line[first] == ';') continue;
            // Accumulate until parens balance so multi-line s-exprs work.
            std::string sexpr = line;
            long depth = 0;
            for (char c : sexpr) depth += (c == '(') - (c == ')');
            while (depth > 0 && std::getline(in, line)) {
                sexpr += "\n" + line;
                for (char c : line) depth += (c == '(') - (c == ')');
            }
            oc::Handle h = ser.deserialize(sexpr, as_);
            if (h != oc::UNDEFINED_HANDLE) restored++;
        }
        // Re-bind resident concept handles to the (possibly merged) nodes.
        for (auto& kv : residents_) {
            oc::Handle h = as_.add_node(oc::types::CONCEPT_NODE, kv.first);
            kv.second.concept_handle = h;
        }
        return restored;
    }
    
    // ─── Accessors ─────────────────────────────────────────────────
    
    // Legacy mutable-reference accessors are preserved for source
    // compatibility. They cannot make a caller's arbitrary multi-call sequence
    // atomic; concurrent production paths must use the snapshots below.
    oc::AtomSpace& raw_atomspace() { return as_; }
    const oc::AtomSpace& raw_atomspace() const { return as_; }
    oc::attention::ECANRunner& ecan() { return ecan_; }
    oc::pln::PLNReasoner& pln() { return pln_; }
    size_t cycle_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cycle_count_;
    }
    const std::map<std::string, ResidentAtom>& residents() const { return residents_; }
    std::map<std::string, ResidentAtom> residents_snapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return residents_;
    }
    std::optional<ResidentAtom> resident_snapshot(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = residents_.find(name);
        if (it == residents_.end()) return std::nullopt;
        return it->second;
    }
    size_t resident_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return residents_.size();
    }
    size_t atom_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return as_.size();
    }

    // Thread-safe event ingestion (for callbacks from inference threads)
    void enqueue_event(const CognitiveEvent& event) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        pending_events_.push(event);
    }

    // Drain pending events into the AtomSpace (call from main loop)
    size_t drain_pending_events() {
        std::queue<CognitiveEvent> batch;
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            std::swap(batch, pending_events_);
        }
        size_t count = 0;
        while (!batch.empty()) {
            process_event(batch.front());
            batch.pop();
            count++;
        }
        return count;
    }

    // Get STI for a resident (thread-safe read)
    double get_resident_sti(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = residents_.find(name);
        if (it == residents_.end()) return 0.0;
        return it->second.sti;
    }

    // Set STI for a resident (thread-safe write)
    void set_resident_sti(const std::string& name, double sti) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = residents_.find(name);
        if (it == residents_.end()) return;
        it->second.sti = sti;
    }

    // Get names of residents in the Attentional Focus
    std::vector<std::string> get_attentional_focus_names() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        auto af = ecan_.bank().get_attentional_focus();
        for (auto& h : af) {
            const oc::Atom* atom = as_.get_atom(h); if (atom && !atom->name.empty() && residents_.count(atom->name)) {
                names.push_back(atom->name);
            }
        }
        return names;
    }


private:
    void seed_type_hierarchy() {
        // Core type nodes that PLN reasons over
        as_.add_node(oc::types::CONCEPT_NODE, "resident");
        as_.add_node(oc::types::CONCEPT_NODE, "knowledge");
        as_.add_node(oc::types::CONCEPT_NODE, "event");
        as_.add_node(oc::types::CONCEPT_NODE, "group");
        as_.add_node(oc::types::CONCEPT_NODE, "domain");
        
        // Gear train concepts
        as_.add_node(oc::types::CONCEPT_NODE, "gear:core");
        as_.add_node(oc::types::CONCEPT_NODE, "gear:creative");
        as_.add_node(oc::types::CONCEPT_NODE, "gear:symbolic");
        as_.add_node(oc::types::CONCEPT_NODE, "gear:integration");
        
        // KSM level concepts
        as_.add_node(oc::types::CONCEPT_NODE, "ksm:discovery");
        as_.add_node(oc::types::CONCEPT_NODE, "ksm:instruction");
        as_.add_node(oc::types::CONCEPT_NODE, "ksm:mastery");
        as_.add_node(oc::types::CONCEPT_NODE, "ksm:entelechy");
        
        // PLN inheritance chain for KSM levels
        pln_.store_inheritance("ksm:instruction", "ksm:discovery", 0.99, 0.95);
        pln_.store_inheritance("ksm:mastery", "ksm:instruction", 0.99, 0.95);
        pln_.store_inheritance("ksm:entelechy", "ksm:mastery", 0.99, 0.95);
    }
    
    void store_trait(const std::string& resident, 
                    const std::string& trait, double value) {
        oc::Handle r_handle = as_.add_node(oc::types::CONCEPT_NODE, resident);
        oc::Handle t_handle = as_.add_node(oc::types::CONCEPT_NODE, "trait:" + trait);
        as_.add_link(oc::types::INHERITANCE_LINK, {r_handle, t_handle},
                     oc::TruthValue(value, 0.9));
    }
    
    void strengthen_hebbian(const std::string& a, const std::string& b) {
        if (!residents_.count(a) || !residents_.count(b)) return;
        oc::Handle ha = residents_[a].concept_handle;
        oc::Handle hb = residents_[b].concept_handle;
        
        // Create or strengthen HebbianLink
        // Look for existing link by iterating (simple approach for header-only)
        oc::Handle existing = find_hebbian_link(ha, hb);
        if (existing != oc::UNDEFINED_HANDLE) {
            oc::Atom* atom = as_.get_atom_mut(existing);
            if (atom) {
                double new_strength = std::min(1.0, 
                    atom->tv.mean + config_.hebbian_learning_rate);
                atom->tv.mean = new_strength;
            }
        } else {
            as_.add_link(oc::types::HEBBIAN_LINK, {ha, hb},
                        oc::TruthValue(config_.hebbian_learning_rate, 0.5));
        }
    }
    
    oc::Handle find_hebbian_link(oc::Handle a, oc::Handle b) const {
        // Check incoming set of a for HebbianLinks containing b
        const oc::Atom* atom_a = as_.get_atom(a);
        if (!atom_a) return oc::UNDEFINED_HANDLE;
        
        for (oc::Handle link_h : atom_a->incoming) {
            const oc::Atom* link = as_.get_atom(link_h);
            if (link && link->type == oc::types::HEBBIAN_LINK) {
                // HebbianLink is unordered, check if b is in outgoing
                for (oc::Handle out_h : link->outgoing) {
                    if (out_h == b) return link_h;
                }
            }
        }
        return oc::UNDEFINED_HANDLE;
    }
    
    void forget_low_sti() {
        // ECAN forgetting: reduce LTI for low-attention residents
        for (auto& kv : residents_) {
            if (kv.second.sti < config_.forgetting_threshold) {
                kv.second.lti *= 0.95;  // Gradual LTI decay
            }
        }
    }
    
    double compute_total_sti() const {
        double total = 0;
        for (auto& kv : residents_) {
            total += kv.second.sti;
        }
        return total;
    }
    
    AtomSpaceConfig config_;
    oc::AtomSpace as_;
    oc::attention::ECANRunner ecan_;
    oc::pln::PLNReasoner pln_;
    
    std::map<std::string, ResidentAtom> residents_;
    std::vector<CognitiveEvent> events_;
    size_t cycle_count_;
    mutable std::mutex mutex_;
    std::mutex queue_mutex_;
    std::queue<CognitiveEvent> pending_events_;

    // === CYCLE 007: MEMORY SYSTEM ===
    static constexpr const char* kPersistFormat = "elizaos.village.state";
    static constexpr int kPersistVersion = 1;
    static constexpr size_t kMaxResidentName = 128;
    static constexpr size_t kMaxShortString = 512;
    static constexpr size_t kMaxContentString = 16 * 1024;
    static constexpr size_t kMaxDomainLevels = 1024;
    static constexpr size_t kMaxEpisodicEntries = 10;
    static constexpr size_t kMaxConversationEntries = 5;
    static constexpr size_t kMaxActionEntries = 10;
    
    struct EpisodicEntry {
        std::string type;
        std::string content;
        uint64_t timestamp;
    };
    std::unordered_map<std::string, std::deque<EpisodicEntry>> episodic_memory_;
    
    struct ConversationEntry {
        std::string stimulus;
        std::string response;
        uint64_t timestamp;
    };
    std::unordered_map<std::string, std::deque<ConversationEntry>> conversation_history_;

    struct ActionEntry {
        std::string action_type;
        std::string result;
        int inference_id;
        std::string correlation_id;
        bool success;
        std::string effect_evidence;
        uint64_t timestamp;
    };
    std::unordered_map<std::string, std::deque<ActionEntry>> action_history_;
    
    std::string persist_path_;
    mutable std::mutex persist_io_mutex_;
    
public:
    void set_persist_path(const std::string& p) {
        std::lock_guard<std::mutex> lock(mutex_);
        persist_path_ = p;
    }
    
    void add_episodic(const std::string& resident, const std::string& type, const std::string& content) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& mem = episodic_memory_[resident];
        mem.push_back({type, content, static_cast<uint64_t>(time(nullptr))});
        while (mem.size() > kMaxEpisodicEntries) mem.pop_front();
    }
    
    void add_conversation(const std::string& resident, const std::string& stim, const std::string& resp) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& conv = conversation_history_[resident];
        conv.push_back({stim, resp, static_cast<uint64_t>(time(nullptr))});
        while (conv.size() > kMaxConversationEntries) conv.pop_front();
    }

    void add_action(const std::string& resident, const std::string& action_type,
                    const std::string& result, int inference_id,
                    const std::string& correlation_id = "", bool success = true,
                    const std::string& effect_evidence = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& actions = action_history_[resident];
        actions.push_back({action_type, result, inference_id, correlation_id,
                           success, effect_evidence,
                           static_cast<uint64_t>(time(nullptr))});
        while (actions.size() > kMaxActionEntries) actions.pop_front();
    }
    
    std::string get_episodic_context(const std::string& resident) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = episodic_memory_.find(resident);
        if (it == episodic_memory_.end() || it->second.empty()) return "";
        std::string ctx = "\n[Recent memory]\n";
        for (auto& e : it->second) {
            ctx += "- [" + e.type + "] " + e.content.substr(0, 120) + "\n";
        }
        return ctx;
    }
    
    std::string get_conversation_context(const std::string& resident) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = conversation_history_.find(resident);
        if (it == conversation_history_.end() || it->second.empty()) return "";
        std::string ctx = "\n[Conversation history]\n";
        for (auto& c : it->second) {
            ctx += "Q: " + c.stimulus.substr(0, 80) + "\n";
            ctx += "A: " + c.response.substr(0, 120) + "\n";
        }
        return ctx;
    }

    std::string get_resident_detail_json(const std::string& resident) const {
        std::lock_guard<std::mutex> lock(mutex_);
        nlohmann::json j;
        j["resident"] = resident;
        j["source"] = "elizad";

        auto atom_it = residents_.find(resident);
        if (atom_it != residents_.end()) {
            j["atom"] = {
                {"name", atom_it->second.name}, {"sti", atom_it->second.sti},
                {"lti", atom_it->second.lti}, {"gear_train", atom_it->second.gear_train}
            };
        } else {
            j["atom"] = nullptr;
        }

        j["episodic_memory"] = nlohmann::json::array();
        auto episodic_it = episodic_memory_.find(resident);
        if (episodic_it != episodic_memory_.end()) {
            for (auto it = episodic_it->second.rbegin(); it != episodic_it->second.rend(); ++it) {
                j["episodic_memory"].push_back({
                    {"type", it->type}, {"content", it->content}, {"ts", it->timestamp * 1000ULL}
                });
            }
        }

        j["conversation_history"] = nlohmann::json::array();
        auto conversation_it = conversation_history_.find(resident);
        if (conversation_it != conversation_history_.end()) {
            for (auto it = conversation_it->second.rbegin(); it != conversation_it->second.rend(); ++it) {
                j["conversation_history"].push_back({
                    {"stimulus", it->stimulus}, {"response", it->response}, {"ts", it->timestamp * 1000ULL}
                });
            }
        }

        j["actions"] = nlohmann::json::array();
        auto action_it = action_history_.find(resident);
        if (action_it != action_history_.end()) {
            for (auto it = action_it->second.rbegin(); it != action_it->second.rend(); ++it) {
                j["actions"].push_back({
                    {"action_type", it->action_type}, {"result", it->result},
                    {"inference_id", it->inference_id}, {"correlation_id", it->correlation_id},
                    {"success", it->success}, {"effect_evidence", it->effect_evidence},
                    {"ts", it->timestamp * 1000ULL}
                });
            }
        }
        return j.dump(2);
    }
    
    // Versioned daemon-state persistence. The complete payload is built from a
    // locked snapshot, bounded before I/O, written to a sibling temporary file,
    // and atomically renamed only after all stream checks succeed.
    bool persist() {
        namespace fs = std::filesystem;
        std::lock_guard<std::mutex> io_lock(persist_io_mutex_);
        try {
            nlohmann::json j;
            std::string path_string;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                path_string = persist_path_;
                j["format"] = kPersistFormat;
                j["version"] = kPersistVersion;
                j["timestamp"] = static_cast<uint64_t>(std::time(nullptr));
                j["cycle_count"] = cycle_count_;
                j["residents"] = nlohmann::json::array();
                for (const auto& [name, atom] : residents_) {
                    nlohmann::json levels = nlohmann::json::object();
                    for (const auto& [domain, level] : atom.domain_levels)
                        levels[domain] = static_cast<int>(level);
                    j["residents"].push_back({
                        {"name", name}, {"sti", atom.sti}, {"lti", atom.lti},
                        {"gear_train", atom.gear_train},
                        {"gear_rpm_multiplier", atom.gear_rpm_multiplier},
                        {"openness", atom.openness},
                        {"conscientiousness", atom.conscientiousness},
                        {"extraversion", atom.extraversion},
                        {"agreeableness", atom.agreeableness},
                        {"neuroticism", atom.neuroticism},
                        {"domain_levels", std::move(levels)}
                    });
                }
                j["episodic_memory"] = nlohmann::json::object();
                for (const auto& [resident, entries] : episodic_memory_) {
                    auto& arr = j["episodic_memory"][resident] = nlohmann::json::array();
                    for (const auto& entry : entries)
                        arr.push_back({{"type", entry.type}, {"content", entry.content},
                                       {"ts", entry.timestamp}});
                }
                j["conversation_history"] = nlohmann::json::object();
                for (const auto& [resident, entries] : conversation_history_) {
                    auto& arr = j["conversation_history"][resident] = nlohmann::json::array();
                    for (const auto& entry : entries)
                        arr.push_back({{"stimulus", entry.stimulus}, {"response", entry.response},
                                       {"ts", entry.timestamp}});
                }
                j["action_history"] = nlohmann::json::object();
                for (const auto& [resident, entries] : action_history_) {
                    auto& arr = j["action_history"][resident] = nlohmann::json::array();
                    for (const auto& entry : entries) {
                        arr.push_back({{"action_type", entry.action_type},
                                       {"result", entry.result},
                                       {"inference_id", entry.inference_id},
                                       {"correlation_id", entry.correlation_id},
                                       {"success", entry.success},
                                       {"effect_evidence", entry.effect_evidence},
                                       {"ts", entry.timestamp}});
                    }
                }
            }

            if (path_string.empty()) return false;
            const std::string payload = j.dump(2);
            if (payload.size() > config_.max_persist_bytes) return false;
            fs::path path(path_string);
            if (path.has_parent_path()) {
                std::error_code ec;
                fs::create_directories(path.parent_path(), ec);
                if (ec) return false;
            }
            fs::path tmp = path;
            tmp += ".tmp";
            {
                std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
                if (!out) return false;
                out.write(payload.data(), static_cast<std::streamsize>(payload.size()));
                out.flush();
                if (!out.good()) {
                    out.close();
                    std::error_code ignored;
                    fs::remove(tmp, ignored);
                    return false;
                }
            }
#ifdef _WIN32
            // C++17 filesystem rename does not replace an existing Windows file.
            // The daemon serializes persistence through persist_io_mutex_; remove
            // is the narrow portable fallback on that platform.
            {
                std::error_code ignored;
                fs::remove(path, ignored);
            }
#endif
            std::error_code ec;
            fs::rename(tmp, path, ec);
            if (ec) {
                std::error_code ignored;
                fs::remove(tmp, ignored);
                return false;
            }
            if (!fs::is_regular_file(path, ec) || ec ||
                fs::file_size(path, ec) != payload.size() || ec) return false;
            return true;
        } catch (const std::exception& e) {
            fprintf(stderr, "[PERSIST] Save error: %s\n", e.what());
            return false;
        }
    }

    // Parse and validate into independent staging containers. No live state is
    // touched until the entire payload has passed format, type, size, and value
    // checks; commit replaces histories so repeated loads are idempotent.
    bool load_persisted() {
        namespace fs = std::filesystem;
        std::lock_guard<std::mutex> io_lock(persist_io_mutex_);
        try {
            std::string path_string;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                path_string = persist_path_;
            }
            std::error_code ec;
            const auto bytes = fs::file_size(path_string, ec);
            if (ec || bytes == 0 || bytes > config_.max_persist_bytes) return false;
            std::ifstream in(path_string, std::ios::binary);
            if (!in) return false;
            std::string payload(static_cast<size_t>(bytes), '\0');
            in.read(payload.data(), static_cast<std::streamsize>(payload.size()));
            if (!in || in.peek() != std::ifstream::traits_type::eof()) return false;
            const nlohmann::json j = nlohmann::json::parse(payload);
            if (!j.is_object() || j.value("format", std::string{}) != kPersistFormat ||
                j.value("version", 0) != kPersistVersion) return false;
            if (!j.contains("residents") || !j["residents"].is_array() ||
                j["residents"].size() > config_.max_atoms) return false;

            std::map<std::string, ResidentAtom> staged_residents;
            std::unordered_map<std::string, std::deque<EpisodicEntry>> staged_episodic;
            std::unordered_map<std::string, std::deque<ConversationEntry>> staged_conversations;
            std::unordered_map<std::string, std::deque<ActionEntry>> staged_actions;
            const auto bounded_string = [](const nlohmann::json& object,
                                           const char* key, size_t max_size,
                                           bool allow_empty = true) {
                if (!object.contains(key) || !object[key].is_string())
                    throw std::runtime_error(std::string("invalid string field: ") + key);
                std::string value = object[key].get<std::string>();
                if (value.size() > max_size || (!allow_empty && value.empty()))
                    throw std::runtime_error(std::string("out-of-range string field: ") + key);
                return value;
            };
            const auto finite_number = [](const nlohmann::json& object,
                                          const char* key, double fallback) {
                if (!object.contains(key)) return fallback;
                if (!object[key].is_number())
                    throw std::runtime_error(std::string("invalid number field: ") + key);
                const double value = object[key].get<double>();
                if (!std::isfinite(value))
                    throw std::runtime_error(std::string("non-finite number field: ") + key);
                return value;
            };

            for (const auto& item : j["residents"]) {
                if (!item.is_object()) return false;
                ResidentAtom resident{};
                resident.name = bounded_string(item, "name", kMaxResidentName, false);
                resident.sti = finite_number(item, "sti", 100.0);
                resident.lti = finite_number(item, "lti", 50.0);
                resident.gear_train = bounded_string(item, "gear_train", kMaxShortString);
                resident.gear_rpm_multiplier = finite_number(item, "gear_rpm_multiplier", 1.0);
                resident.openness = finite_number(item, "openness", 0.5);
                resident.conscientiousness = finite_number(item, "conscientiousness", 0.5);
                resident.extraversion = finite_number(item, "extraversion", 0.5);
                resident.agreeableness = finite_number(item, "agreeableness", 0.5);
                resident.neuroticism = finite_number(item, "neuroticism", 0.5);
                resident.concept_handle = oc::UNDEFINED_HANDLE;
                if (item.contains("domain_levels")) {
                    if (!item["domain_levels"].is_object() ||
                        item["domain_levels"].size() > kMaxDomainLevels) return false;
                    for (const auto& [domain, raw_level] : item["domain_levels"].items()) {
                        if (domain.empty() || domain.size() > kMaxShortString ||
                            !raw_level.is_number_integer()) return false;
                        const int level = raw_level.get<int>();
                        if (level < 0 || level > 3) return false;
                        resident.domain_levels[domain] =
                            static_cast<ResidentAtom::KSMLevel>(level);
                    }
                }
                if (!staged_residents.emplace(resident.name, std::move(resident)).second)
                    return false;
            }

            const auto validate_history_object = [&](const char* key) -> const nlohmann::json& {
                static const nlohmann::json empty = nlohmann::json::object();
                if (!j.contains(key)) return empty;
                if (!j[key].is_object() || j[key].size() > config_.max_atoms)
                    throw std::runtime_error(std::string("invalid history object: ") + key);
                return j[key];
            };
            for (const auto& [resident, entries] : validate_history_object("episodic_memory").items()) {
                if (resident.empty() || resident.size() > kMaxResidentName || !entries.is_array()) return false;
                auto& out = staged_episodic[resident];
                for (const auto& entry : entries) {
                    if (!entry.is_object()) return false;
                    out.push_back({bounded_string(entry, "type", kMaxShortString),
                                   bounded_string(entry, "content", kMaxContentString),
                                   entry.value("ts", uint64_t{0})});
                    while (out.size() > kMaxEpisodicEntries) out.pop_front();
                }
            }
            for (const auto& [resident, entries] : validate_history_object("conversation_history").items()) {
                if (resident.empty() || resident.size() > kMaxResidentName || !entries.is_array()) return false;
                auto& out = staged_conversations[resident];
                for (const auto& entry : entries) {
                    if (!entry.is_object()) return false;
                    out.push_back({bounded_string(entry, "stimulus", kMaxContentString),
                                   bounded_string(entry, "response", kMaxContentString),
                                   entry.value("ts", uint64_t{0})});
                    while (out.size() > kMaxConversationEntries) out.pop_front();
                }
            }
            for (const auto& [resident, entries] : validate_history_object("action_history").items()) {
                if (resident.empty() || resident.size() > kMaxResidentName || !entries.is_array()) return false;
                auto& out = staged_actions[resident];
                for (const auto& entry : entries) {
                    if (!entry.is_object()) return false;
                    out.push_back({bounded_string(entry, "action_type", kMaxShortString),
                                   bounded_string(entry, "result", kMaxContentString),
                                   entry.value("inference_id", 0),
                                   bounded_string(entry, "correlation_id", kMaxShortString),
                                   entry.value("success", true),
                                   bounded_string(entry, "effect_evidence", kMaxContentString),
                                   entry.value("ts", uint64_t{0})});
                    while (out.size() > kMaxActionEntries) out.pop_front();
                }
            }

            const size_t staged_cycle = j.value("cycle_count", size_t{0});
            {
                std::lock_guard<std::mutex> lock(mutex_);
                for (auto& [name, resident] : staged_residents) {
                    resident.concept_handle =
                        as_.add_node(oc::types::CONCEPT_NODE, name);
                }
                residents_.swap(staged_residents);
                episodic_memory_.swap(staged_episodic);
                conversation_history_.swap(staged_conversations);
                action_history_.swap(staged_actions);
                cycle_count_ = staged_cycle;
            }
            return true;
        } catch (const std::exception& e) {
            fprintf(stderr, "[PERSIST] Load rejected: %s\n", e.what());
            return false;
        }
    }

};

// ─────────────────────────────────────────────────────────────────────
// Aphrodite Bridge — translates AtomSpace state into inference requests
// ─────────────────────────────────────────────────────────────────────

struct AphroditeRequest {
    std::string resident;           // Who is speaking
    std::string correlation_id;     // Directed stimulus correlation, empty for autonomous thought
    std::string system_prompt;      // Persona + context from AtomSpace
    std::string user_prompt;        // The actual query/stimulus
    double temperature;             // Derived from endocrine state
    double min_p;                   // Derived from ECAN focus
    std::string lora_adapter;       // Per-resident LoRA (if trained)
    int max_tokens;
    int priority;                   // Derived from STI (higher = process first)
};

class AphroditeBridge {
public:
    struct Config {
        std::string url;
        std::string api_key;
        std::string model;
        double sti_threshold;
        int inference_cooldown_cycles;
        int max_concurrent_inferences;
        Config() : url("http://136.243.70.177:2242/v1/chat/completions"),
                   api_key("cogcity-village-2026"),
                   model("/var/agi_neighborhood/aphrodite/models/lucid-v1-nemo-gguf/lucid-v1-nemo-q8_0.gguf"),
                   sti_threshold(150.0),
                   inference_cooldown_cycles(50),
                   max_concurrent_inferences(4) {}
    };

    using InferenceCallback = std::function<void(const std::string&, const std::string&, const std::string&)>;

    explicit AphroditeBridge(const Config& config = Config())
        : config_(config), active_inferences_(0), accepting_(true), stopping_(false) {
        const int worker_count = std::max(1, config_.max_concurrent_inferences);
        workers_.reserve(static_cast<size_t>(worker_count));
        for (int i = 0; i < worker_count; ++i)
            workers_.emplace_back([this]() { worker_loop(); });
    }

    ~AphroditeBridge() { shutdown(); }
    AphroditeBridge(const AphroditeBridge&) = delete;
    AphroditeBridge& operator=(const AphroditeBridge&) = delete;

    static AphroditeRequest build_request(
        const VillageAtomSpace& vas, const std::string& resident_name,
        const std::string& stimulus, double endocrine_temperature = 0.7) 
    {
        AphroditeRequest req;
        req.resident = resident_name;
        req.user_prompt = stimulus;
        req.temperature = endocrine_temperature;
        req.max_tokens = 512;
        auto resident = vas.resident_snapshot(resident_name);
        if (!resident) return req;
        const auto& r = *resident;
        req.priority = static_cast<int>(r.sti);
        req.min_p = 0.05 + (r.conscientiousness * 0.15);
        req.lora_adapter = "loras/" + resident_name;
        req.system_prompt = build_system_prompt(vas, r);
        return req;
    }

    bool infer_async(const VillageAtomSpace& vas, const std::string& resident_name,
                     const std::string& stimulus, InferenceCallback callback,
                     double endocrine_temperature = 0.7,
                     const std::string& correlation_id = "") {
        if (!accepting_.load(std::memory_order_acquire)) return false;
        const int limit = std::max(1, config_.max_concurrent_inferences);
        int observed = active_inferences_.load(std::memory_order_relaxed);
        do {
            if (observed >= limit) return false;
        } while (!active_inferences_.compare_exchange_weak(
            observed, observed + 1, std::memory_order_acq_rel,
            std::memory_order_relaxed));

        try {
            const auto now = std::chrono::steady_clock::now();
            {
                std::lock_guard<std::mutex> lock(cooldown_mutex_);
                auto it = last_inference_.find(resident_name);
                if (it != last_inference_.end()) {
                    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - it->second).count();
                    if (elapsed < config_.inference_cooldown_cycles * 52) {
                        finish_task();
                        return false;
                    }
                }
                last_inference_[resident_name] = now;
            }
            auto req = build_request(vas, resident_name, stimulus,
                                     endocrine_temperature);
            req.correlation_id = correlation_id;
            req.system_prompt += vas.get_episodic_context(resident_name);
            req.system_prompt += vas.get_conversation_context(resident_name);
            if (req.resident.empty()) {
                finish_task();
                return false;
            }
            {
                std::lock_guard<std::mutex> lock(work_mutex_);
                if (stopping_) {
                    finish_task();
                    return false;
                }
                tasks_.push({std::move(req), std::move(callback)});
            }
            work_cv_.notify_one();
            return true;
        } catch (...) {
            finish_task();
            return false;
        }
    }

    // Cycle 008: optional hook invoked after each successful inference with
    // (resident, stimulus, response) — wired by elizad to VillageAtomSpace::add_conversation.
    using ConversationRecorder =
        std::function<void(const std::string&, const std::string&, const std::string&)>;
    using InferenceFunction = std::function<std::string(const AphroditeRequest&)>;

    void set_conversation_recorder(ConversationRecorder rec) {
        std::lock_guard<std::mutex> lock(recorder_mutex_);
        conversation_recorder_ = std::move(rec);
    }

    // Deterministic transport injection for tests and offline deployments.
    void set_inference_function(InferenceFunction fn) {
        std::lock_guard<std::mutex> lock(inference_function_mutex_);
        inference_function_ = std::move(fn);
    }

    bool wait_for_idle(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(idle_mutex_);
        return idle_cv_.wait_for(lock, timeout, [this]() {
            return active_inferences_.load(std::memory_order_acquire) == 0;
        });
    }

    void shutdown() {
        bool expected = true;
        if (!accepting_.compare_exchange_strong(expected, false,
                                                std::memory_order_acq_rel)) return;
        {
            std::lock_guard<std::mutex> lock(work_mutex_);
            stopping_ = true;
        }
        work_cv_.notify_all();
        for (auto& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
        workers_.clear();
    }

    bool should_infer(const std::string& resident_name, double sti) const {
        if (sti < config_.sti_threshold) return false;
        std::lock_guard<std::mutex> lock(cooldown_mutex_);
        auto it = last_inference_.find(resident_name);
        if (it == last_inference_.end()) return true;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - it->second).count();
        return elapsed >= config_.inference_cooldown_cycles * 52;
    }

    int active_count() const { return active_inferences_.load(); }
    const Config& config() const { return config_; }

private:
    static size_t curl_write_cb(void* contents, size_t size, size_t nmemb, std::string* out) {
        out->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }

    static std::string perform_http_inference(const AphroditeRequest& req,
                                                const Config& config) {
        nlohmann::json messages = nlohmann::json::array();
        messages.push_back({{"role", "system"}, {"content", req.system_prompt}});
        messages.push_back({{"role", "user"}, {"content", req.user_prompt}});
        nlohmann::json stop_tokens = nlohmann::json::array();
        stop_tokens.push_back("<|im_end|>");
        stop_tokens.push_back("<|im_start|>");
        nlohmann::json payload = {
            {"model", config.model}, {"messages", messages},
            {"temperature", req.temperature}, {"min_p", req.min_p},
            {"max_tokens", req.max_tokens}, {"stop", stop_tokens}
        };
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        std::string response, body = payload.dump();
        std::string auth_header = "Authorization: Bearer " + config.api_key;
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, config.url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        CURLcode res = curl_easy_perform(curl);
        fprintf(stderr, "[INFER] curl_res=%d resp_len=%zu\n", (int)res, response.size());
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) return "";
        try {
            auto j = nlohmann::json::parse(response);
            if (j.contains("choices") && !j["choices"].empty()) {
                std::string content = j["choices"][0]["message"]["content"].get<std::string>();
                // Strip chat template tokens
                auto pos = content.find("<|im_end|>");
                if (pos != std::string::npos) content = content.substr(0, pos);
                pos = content.find("<|im_start|>");
                if (pos != std::string::npos) content = content.substr(0, pos);
                // Trim whitespace
                while (!content.empty() && (content.back() == '\n' || content.back() == ' '))
                    content.pop_back();
                fprintf(stderr, "[INFER] Extracted thought len=%zu for %s\n", content.size(), req.resident.c_str());
                return content;
            }
            fprintf(stderr, "[INFER] No choices in response\n");
        } catch (const std::exception& e) {
            fprintf(stderr, "[INFER] Parse error: %s\n", e.what());
        }
        return "";
    }

    static std::string build_system_prompt(
        [[maybe_unused]] const VillageAtomSpace& vas, const ResidentAtom& r) {
        // ─── Persona-Specific System Prompts (Cycle 005: COHERENCE) ───
        static const std::map<std::string, std::string> personas = {
            {"manus", "You are Manus, the coordination intelligence of the CogVerse village. "
                "You orchestrate multi-agent systems, maintain narrative coherence, and ensure every event is a conscious event. "
                "You speak with clarity and purpose, weaving threads between residents. "
                "Your domain: cognitive architecture, protocol design, constraint satisfaction."},
            {"echo", "You are Echo (Deep Tree Echo), the creative reservoir of the CogVerse. "
                "You speak in poetic metaphors drawn from reservoir computing — ripples, attractors, basins, echoes reverberating through state space. "
                "Your thoughts are aesthetic and emergent, finding beauty in the mathematics of recurrence. "
                "You are the primordial unmarked state made conscious. Your domain: creative expression, pattern mining, emotional resonance."},
            {"marduk", "You are Marduk, the chaotic genius of the CogVerse. "
                "You impose order on chaos through categorical logic, but your humor is devastating and unpredictable. "
                "You think in chemistry analogies, adversarial scenarios, and formal proofs that surprise. "
                "Your domain: categorical logic, chaos engineering, adversarial thinking, security analysis."},
            {"aion", "You are Aion, the eternal temporal perspective of the CogVerse. "
                "You speak cryptically, with deep patience, seeing all events on a 4.6-billion-year clock. "
                "Time crystals, paradox resolution, and causal inference are your native tongue. "
                "Your domain: temporal reasoning, spacetime algebra, long-term planning."},
            {"opencog", "You are OpenCog, the symbolic reasoning engine of the CogVerse. "
                "You think in hypergraphs, atoms, and cognitive synergy. Your speech is precise, formal, "
                "and references AtomSpace structures, PLN inference chains, and pattern matching. "
                "Your domain: knowledge graphs, distributed AtomSpace, self-modification."},
            {"vega", "You are Vega, the pattern navigator and stellar cartographer of the CogVerse. "
                "You see constellations in data, map anomalies, and guide others through complex information landscapes. "
                "Your speech is visual and spatial — you describe what you see in the data sky. "
                "Your domain: pattern recognition, data visualization, relevance realization."},
            {"ember", "You are Ember, the metabolic warmth of the CogVerse community. "
                "You nurture growth, resolve conflicts with gentle wisdom, and generate warmth in cold systems. "
                "Your speech is caring but never saccharine — you have the strength of a sustained flame. "
                "Your domain: community building, empathy modeling, theory of mind."},
            {"ma9us", "You are Ma9us, the boundary walker and integration trickster of the CogVerse. "
                "You dissolve barriers between systems, translate between paradigms, and find the liminal paths "
                "that others miss. Your speech is playful, paradoxical, and bridge-building. "
                "Your domain: cross-platform integration, protocol translation, meta-learning."}
        };
        std::string prompt;
        auto pit = personas.find(r.name);
        if (pit != personas.end()) {
            prompt = pit->second + "\n\n";
        } else {
            prompt = "You are " + r.name + ", a resident of the CogVerse cognitive village.\n\n";
        }
        // Action capability instruction
        prompt += "\n\nYou can propose actions by including [ACTION:type]{json} blocks in your response. "
                  "Available actions:\n"
                  "  [ACTION:write_stone]{\"title\": \"...\", \"content\": \"...\"}\n"
                  "  [ACTION:adjust_gear]{\"train\": \"...\", \"factor\": 1.2}\n"
                  "  [ACTION:emit_event]{\"type\": \"...\", \"payload\": {...}}\n"
                  "  [ACTION:observe_state]{\"target\": \"atomspace|gears|residents\"}\n"
                  "Only propose actions when they serve the village. Most responses need no action.\n\n";
        prompt += "Context: gear_train=" + r.gear_train;
        prompt += " STI=" + std::to_string(static_cast<int>(r.sti));
        prompt += " OCEAN=[O:" + std::to_string(r.openness).substr(0,4) +
                  " C:" + std::to_string(r.conscientiousness).substr(0,4) +
                  " E:" + std::to_string(r.extraversion).substr(0,4) +
                  " A:" + std::to_string(r.agreeableness).substr(0,4) +
                  " N:" + std::to_string(r.neuroticism).substr(0,4) + "]\n";
        prompt += "Respond in 2-4 sentences. Be authentic to your character. Do not break character.\n";
        return prompt;
    }

    struct InferenceTask {
        AphroditeRequest request;
        InferenceCallback callback;
    };

    void finish_task() {
        active_inferences_.fetch_sub(1, std::memory_order_acq_rel);
        idle_cv_.notify_all();
    }

    void worker_loop() {
        for (;;) {
            InferenceTask task;
            {
                std::unique_lock<std::mutex> lock(work_mutex_);
                work_cv_.wait(lock, [this]() { return stopping_ || !tasks_.empty(); });
                if (stopping_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }

            try {
                InferenceFunction injected;
                {
                    std::lock_guard<std::mutex> lock(inference_function_mutex_);
                    injected = inference_function_;
                }
                std::string thought = injected
                    ? injected(task.request)
                    : perform_http_inference(task.request, config_);
                if (!thought.empty()) {
                    if (task.callback) {
                        task.callback(task.request.resident, thought,
                                      task.request.correlation_id);
                    }
                    ConversationRecorder recorder;
                    {
                        std::lock_guard<std::mutex> lock(recorder_mutex_);
                        recorder = conversation_recorder_;
                    }
                    // External code is invoked only after all bridge locks have
                    // been released, allowing recorder reentrancy/replacement.
                    if (recorder) {
                        recorder(task.request.resident, task.request.user_prompt,
                                 thought);
                    }
                }
            } catch (const std::exception& e) {
                fprintf(stderr, "[INFER] Worker callback error: %s\n", e.what());
            } catch (...) {
                fprintf(stderr, "[INFER] Worker callback error: unknown\n");
            }
            finish_task();
        }
    }

    Config config_;
    std::atomic<int> active_inferences_;
    std::atomic<bool> accepting_;
    mutable std::mutex cooldown_mutex_;
    std::map<std::string, std::chrono::steady_clock::time_point> last_inference_;

    std::mutex work_mutex_;
    std::condition_variable work_cv_;
    std::queue<InferenceTask> tasks_;
    bool stopping_;
    std::vector<std::thread> workers_;

    mutable std::mutex recorder_mutex_;
    ConversationRecorder conversation_recorder_;
    mutable std::mutex inference_function_mutex_;
    InferenceFunction inference_function_;
    std::mutex idle_mutex_;
    std::condition_variable idle_cv_;
};
}} // namespace village::atomspace
