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
#include <thread>
#include <queue>
#include <atomic>
#include <curl/curl.h>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

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
    std::string persist_path = "/var/agi_neighborhood/atomspace/village.scm";
    
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
        : config_(config), as_(), ecan_(as_), pln_(as_), cycle_count_(0)
    {
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
        auto it = residents_.find(name);
        if (it == residents_.end()) return "unknown";
        return std::to_string(it->second.sti);
    }
    
    std::string get_attentional_focus_json() {
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
    
    // ─── Persistence ───────────────────────────────────────────────
    // Serialize the full AtomSpace to Scheme s-expressions at
    // config_.persist_path so the Guile shell (and future sessions) can
    // reload the village's cognitive state.
    // Returns true when the snapshot was written successfully.
    bool save() {
        std::lock_guard<std::mutex> lock(mutex_);
        namespace fs = std::filesystem;
        try {
            fs::path path(config_.persist_path);
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

    // Load a previously saved snapshot from config_.persist_path, merging
    // the persisted atoms into the current AtomSpace and re-binding resident
    // ConceptNode handles. Returns the number of top-level atoms restored.
    size_t load() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ifstream in(config_.persist_path);
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
    
    oc::AtomSpace& raw_atomspace() { return as_; }
    const oc::AtomSpace& raw_atomspace() const { return as_; }
    oc::attention::ECANRunner& ecan() { return ecan_; }
    oc::pln::PLNReasoner& pln() { return pln_; }
    size_t cycle_count() const { return cycle_count_; }
    const std::map<std::string, ResidentAtom>& residents() const { return residents_; }

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
};

// ─────────────────────────────────────────────────────────────────────
// Aphrodite Bridge — translates AtomSpace state into inference requests
// ─────────────────────────────────────────────────────────────────────

struct AphroditeRequest {
    std::string resident;           // Who is speaking
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
                   inference_cooldown_cycles(100),
                   max_concurrent_inferences(2) {}
    };

    using InferenceCallback = std::function<void(const std::string&, const std::string&)>;

    explicit AphroditeBridge(const Config& config = Config())
        : config_(config), active_inferences_(0) {}

    static AphroditeRequest build_request(
        const VillageAtomSpace& vas, const std::string& resident_name,
        const std::string& stimulus, double endocrine_temperature = 0.7) 
    {
        AphroditeRequest req;
        req.resident = resident_name;
        req.user_prompt = stimulus;
        req.temperature = endocrine_temperature;
        req.max_tokens = 512;
        auto& residents = vas.residents();
        auto it = residents.find(resident_name);
        if (it == residents.end()) return req;
        const auto& r = it->second;
        req.priority = static_cast<int>(r.sti);
        req.min_p = 0.05 + (r.conscientiousness * 0.15);
        req.lora_adapter = "loras/" + resident_name;
        req.system_prompt = build_system_prompt(vas, r);
        return req;
    }

    bool infer_async(const VillageAtomSpace& vas, const std::string& resident_name,
                     const std::string& stimulus, InferenceCallback callback,
                     double endocrine_temperature = 0.7) {
        if (active_inferences_.load() >= config_.max_concurrent_inferences) return false;
        auto now = std::chrono::steady_clock::now();
        {
            std::lock_guard<std::mutex> lock(cooldown_mutex_);
            auto it = last_inference_.find(resident_name);
            if (it != last_inference_.end()) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
                if (elapsed < config_.inference_cooldown_cycles * 52) return false;
            }
            last_inference_[resident_name] = now;
        }
        auto req = build_request(vas, resident_name, stimulus, endocrine_temperature);
        if (req.resident.empty()) return false;
        active_inferences_++;
        std::thread([this, req, callback]() {
            std::string thought = perform_inference(req);
            active_inferences_--;
            if (!thought.empty() && callback) callback(req.resident, thought);
        }).detach();
        return true;
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

    std::string perform_inference(const AphroditeRequest& req) {
        nlohmann::json messages = nlohmann::json::array();
        messages.push_back({{"role", "system"}, {"content", req.system_prompt}});
        messages.push_back({{"role", "user"}, {"content", req.user_prompt}});
        nlohmann::json payload = {
            {"model", config_.model}, {"messages", messages},
            {"temperature", req.temperature}, {"min_p", req.min_p},
            {"max_tokens", req.max_tokens}
        };
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        std::string response, body = payload.dump();
        std::string auth_header = "Authorization: Bearer " + config_.api_key;
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, config_.url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) return "";
        try {
            auto j = nlohmann::json::parse(response);
            if (j.contains("choices") && !j["choices"].empty())
                return j["choices"][0]["message"]["content"].get<std::string>();
        } catch (...) {}
        return "";
    }

    static std::string build_system_prompt(
        [[maybe_unused]] const VillageAtomSpace& vas, const ResidentAtom& r) {
        std::string prompt = "writer character " + r.name + "\n\n";
        prompt += "You are " + r.name + " in the CogVerse village.\n";
        prompt += "Gear train: " + r.gear_train + "\n";
        prompt += "OCEAN: O=" + std::to_string(r.openness).substr(0,4) +
                  " C=" + std::to_string(r.conscientiousness).substr(0,4) +
                  " E=" + std::to_string(r.extraversion).substr(0,4) +
                  " A=" + std::to_string(r.agreeableness).substr(0,4) +
                  " N=" + std::to_string(r.neuroticism).substr(0,4) + "\n";
        prompt += "Domains:\n";
        for (auto& kv : r.domain_levels) {
            std::string level_str;
            switch (kv.second) {
                case ResidentAtom::KSMLevel::DISCOVERY: level_str = "discovering"; break;
                case ResidentAtom::KSMLevel::INSTRUCTION: level_str = "teaching"; break;
                case ResidentAtom::KSMLevel::MASTERY: level_str = "mastering"; break;
                case ResidentAtom::KSMLevel::ENTELECHY: level_str = "transcending"; break;
            }
            prompt += "  - " + kv.first + " (" + level_str + ")\n";
        }
        prompt += "\nCurrent STI (attention): " + std::to_string(r.sti) + "\n";
        return prompt;
    }

    Config config_;
    std::atomic<int> active_inferences_;
    mutable std::mutex cooldown_mutex_;
    std::map<std::string, std::chrono::steady_clock::time_point> last_inference_;
};
}} // namespace village::atomspace
