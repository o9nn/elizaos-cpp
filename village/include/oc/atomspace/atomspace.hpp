// oc::atomspace — Header-only C++11 reimplementation of opencog/atomspace
// Type system, Atom, Node, Link, Handle, TruthValue, Value, AtomTable, AtomSpace
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_ATOMSPACE_HPP
#define OC_ATOMSPACE_HPP

#include "../util/util.hpp"

namespace oc {

// ─────────────────────────────────────────────────────────────────────────────
// Type — Runtime atom type identifier (mirrors opencog::Type)
// ─────────────────────────────────────────────────────────────────────────────
using Type = uint16_t;

namespace types {
    // Sentinel
    static const Type NOTYPE            = 0;
    // Base
    static const Type ATOM              = 1;
    static const Type NODE              = 2;
    static const Type LINK              = 3;
    // Node subtypes
    static const Type CONCEPT_NODE      = 10;
    static const Type PREDICATE_NODE    = 11;
    static const Type SCHEMA_NODE       = 12;
    static const Type GROUNDED_SCHEMA_NODE = 13;
    static const Type NUMBER_NODE       = 14;
    static const Type VARIABLE_NODE     = 15;
    static const Type ANCHOR_NODE       = 16;
    static const Type TYPE_NODE         = 17;
    static const Type DEFINED_SCHEMA_NODE = 18;
    static const Type GLOB_NODE         = 19;
    // Link subtypes
    static const Type ORDERED_LINK      = 30;
    static const Type UNORDERED_LINK    = 31;
    static const Type LIST_LINK         = 32;
    static const Type SET_LINK          = 33;
    static const Type MEMBER_LINK       = 34;
    static const Type INHERITANCE_LINK  = 35;
    static const Type SIMILARITY_LINK   = 36;
    static const Type EVALUATION_LINK   = 37;
    static const Type EXECUTION_LINK    = 38;
    static const Type EXECUTION_OUTPUT_LINK = 39;
    static const Type BIND_LINK         = 40;
    static const Type AND_LINK          = 41;
    static const Type OR_LINK           = 42;
    static const Type NOT_LINK          = 43;
    static const Type IMPLICATION_LINK  = 44;
    static const Type EQUIVALENCE_LINK  = 45;
    static const Type CONTEXT_LINK      = 46;
    static const Type STATE_LINK        = 47;
    static const Type DEFINE_LINK       = 48;
    static const Type LAMBDA_LINK       = 49;
    static const Type PUT_LINK          = 50;
    static const Type GET_LINK          = 51;
    static const Type SATISFACTION_LINK = 52;
    static const Type SCOPE_LINK        = 53;
    static const Type PRESENT_LINK      = 54;
    static const Type ABSENT_LINK       = 55;
    static const Type CHOICE_LINK       = 56;
    static const Type QUOTE_LINK        = 57;
    static const Type UNQUOTE_LINK      = 58;
    // PLN types
    static const Type INTENSIONAL_IMPLICATION_LINK = 70;
    static const Type EXTENSIONAL_IMPLICATION_LINK = 71;
    static const Type INTENSIONAL_SIMILARITY_LINK  = 72;
    static const Type EXTENSIONAL_SIMILARITY_LINK  = 73;
    static const Type PREDICTIVE_IMPLICATION_LINK  = 74;
    // Attention types
    static const Type HEBBIAN_LINK      = 80;
    static const Type ASYMMETRIC_HEBBIAN_LINK = 81;
    static const Type INVERSE_HEBBIAN_LINK    = 82;
    // Sentinel
    static const Type TYPE_COUNT        = 128;
}

// Type classification
inline bool is_node_type(Type t) { return (t >= 10 && t < 30) || t == types::NODE; }
inline bool is_link_type(Type t) { return t >= 30 || t == types::LINK; }

// Type name registry
class TypeRegistry {
public:
    TypeRegistry() {
        register_defaults();
    }

    void register_type(Type t, const std::string& name, Type parent = types::ATOM) {
        names_[t] = name;
        parents_[t] = parent;
        name_to_type_[name] = t;
    }

    std::string get_name(Type t) const {
        auto it = names_.find(t);
        return (it != names_.end()) ? it->second : "Unknown";
    }

    Type get_type(const std::string& name) const {
        auto it = name_to_type_.find(name);
        return (it != name_to_type_.end()) ? it->second : types::NOTYPE;
    }

    Type get_parent(Type t) const {
        auto it = parents_.find(t);
        return (it != parents_.end()) ? it->second : types::NOTYPE;
    }

    bool is_subtype(Type child, Type parent) const {
        if (child == parent) return true;
        Type cur = child;
        for (int depth = 0; depth < 20; ++depth) {
            auto it = parents_.find(cur);
            if (it == parents_.end()) return false;
            if (it->second == parent) return true;
            cur = it->second;
        }
        return false;
    }

    static TypeRegistry& instance() {
        static TypeRegistry reg;
        return reg;
    }

private:
    std::unordered_map<Type, std::string> names_;
    std::unordered_map<Type, Type> parents_;
    std::unordered_map<std::string, Type> name_to_type_;

    void register_defaults() {
        register_type(types::ATOM, "Atom");
        register_type(types::NODE, "Node", types::ATOM);
        register_type(types::LINK, "Link", types::ATOM);
        register_type(types::CONCEPT_NODE, "ConceptNode", types::NODE);
        register_type(types::PREDICATE_NODE, "PredicateNode", types::NODE);
        register_type(types::SCHEMA_NODE, "SchemaNode", types::NODE);
        register_type(types::GROUNDED_SCHEMA_NODE, "GroundedSchemaNode", types::NODE);
        register_type(types::NUMBER_NODE, "NumberNode", types::NODE);
        register_type(types::VARIABLE_NODE, "VariableNode", types::NODE);
        register_type(types::ANCHOR_NODE, "AnchorNode", types::NODE);
        register_type(types::TYPE_NODE, "TypeNode", types::NODE);
        register_type(types::DEFINED_SCHEMA_NODE, "DefinedSchemaNode", types::NODE);
        register_type(types::GLOB_NODE, "GlobNode", types::NODE);
        register_type(types::ORDERED_LINK, "OrderedLink", types::LINK);
        register_type(types::UNORDERED_LINK, "UnorderedLink", types::LINK);
        register_type(types::LIST_LINK, "ListLink", types::ORDERED_LINK);
        register_type(types::SET_LINK, "SetLink", types::UNORDERED_LINK);
        register_type(types::MEMBER_LINK, "MemberLink", types::ORDERED_LINK);
        register_type(types::INHERITANCE_LINK, "InheritanceLink", types::ORDERED_LINK);
        register_type(types::SIMILARITY_LINK, "SimilarityLink", types::UNORDERED_LINK);
        register_type(types::EVALUATION_LINK, "EvaluationLink", types::ORDERED_LINK);
        register_type(types::EXECUTION_LINK, "ExecutionLink", types::ORDERED_LINK);
        register_type(types::EXECUTION_OUTPUT_LINK, "ExecutionOutputLink", types::ORDERED_LINK);
        register_type(types::BIND_LINK, "BindLink", types::SCOPE_LINK);
        register_type(types::AND_LINK, "AndLink", types::UNORDERED_LINK);
        register_type(types::OR_LINK, "OrLink", types::UNORDERED_LINK);
        register_type(types::NOT_LINK, "NotLink", types::LINK);
        register_type(types::IMPLICATION_LINK, "ImplicationLink", types::ORDERED_LINK);
        register_type(types::EQUIVALENCE_LINK, "EquivalenceLink", types::UNORDERED_LINK);
        register_type(types::CONTEXT_LINK, "ContextLink", types::ORDERED_LINK);
        register_type(types::STATE_LINK, "StateLink", types::ORDERED_LINK);
        register_type(types::DEFINE_LINK, "DefineLink", types::ORDERED_LINK);
        register_type(types::LAMBDA_LINK, "LambdaLink", types::SCOPE_LINK);
        register_type(types::PUT_LINK, "PutLink", types::ORDERED_LINK);
        register_type(types::GET_LINK, "GetLink", types::SCOPE_LINK);
        register_type(types::SATISFACTION_LINK, "SatisfactionLink", types::SCOPE_LINK);
        register_type(types::SCOPE_LINK, "ScopeLink", types::ORDERED_LINK);
        register_type(types::PRESENT_LINK, "PresentLink", types::LINK);
        register_type(types::ABSENT_LINK, "AbsentLink", types::LINK);
        register_type(types::CHOICE_LINK, "ChoiceLink", types::LINK);
        register_type(types::QUOTE_LINK, "QuoteLink", types::LINK);
        register_type(types::UNQUOTE_LINK, "UnquoteLink", types::LINK);
        register_type(types::INTENSIONAL_IMPLICATION_LINK, "IntensionalImplicationLink", types::ORDERED_LINK);
        register_type(types::EXTENSIONAL_IMPLICATION_LINK, "ExtensionalImplicationLink", types::ORDERED_LINK);
        register_type(types::INTENSIONAL_SIMILARITY_LINK, "IntensionalSimilarityLink", types::UNORDERED_LINK);
        register_type(types::EXTENSIONAL_SIMILARITY_LINK, "ExtensionalSimilarityLink", types::UNORDERED_LINK);
        register_type(types::PREDICTIVE_IMPLICATION_LINK, "PredictiveImplicationLink", types::ORDERED_LINK);
        register_type(types::HEBBIAN_LINK, "HebbianLink", types::UNORDERED_LINK);
        register_type(types::ASYMMETRIC_HEBBIAN_LINK, "AsymmetricHebbianLink", types::ORDERED_LINK);
        register_type(types::INVERSE_HEBBIAN_LINK, "InverseHebbianLink", types::ORDERED_LINK);
    }
};

inline const char* type_name(Type t) {
    static std::string buf;
    buf = TypeRegistry::instance().get_name(t);
    return buf.c_str();
}

// ─────────────────────────────────────────────────────────────────────────────
// TruthValue — Probabilistic truth value (mirrors opencog::TruthValue)
// ─────────────────────────────────────────────────────────────────────────────
struct TruthValue {
    double mean;       // strength / probability
    double confidence; // confidence [0,1]
    double count;      // evidence count

    TruthValue() : mean(0.0), confidence(0.0), count(0.0) {}
    TruthValue(double m, double c) : mean(m), confidence(c), count(confidence_to_count(c)) {}
    TruthValue(double m, double c, double n) : mean(m), confidence(c), count(n) {}

    // OpenCog default K parameter
    static constexpr double DEFAULT_K = 800.0;

    static double count_to_confidence(double n, double k = DEFAULT_K) {
        return n / (n + k);
    }
    static double confidence_to_count(double c, double k = DEFAULT_K) {
        if (c >= 1.0) return 1e12;
        return k * c / (1.0 - c);
    }

    // Merge: revision (evidence accumulation)
    TruthValue merge(const TruthValue& other) const {
        double n_total = count + other.count;
        if (n_total < 1e-12) return TruthValue(0.5, 0.0, 0.0);
        double m = (mean * count + other.mean * other.count) / n_total;
        double c = count_to_confidence(n_total);
        return TruthValue(m, c, n_total);
    }

    bool is_default() const {
        return std::fabs(mean) < 1e-12 && std::fabs(confidence) < 1e-12;
    }

    bool operator==(const TruthValue& o) const {
        return std::fabs(mean - o.mean) < 1e-9 &&
               std::fabs(confidence - o.confidence) < 1e-9;
    }
    bool operator!=(const TruthValue& o) const { return !(*this == o); }

    std::string to_string() const {
        std::ostringstream os;
        os << "(stv " << mean << " " << confidence << ")";
        return os.str();
    }
};

static const TruthValue DEFAULT_TV;
static const TruthValue TRUE_TV(1.0, 1.0);
static const TruthValue FALSE_TV(0.0, 1.0);

// ─────────────────────────────────────────────────────────────────────────────
// AttentionValue — ECAN attention economics (mirrors opencog::AttentionValue)
// ─────────────────────────────────────────────────────────────────────────────
struct AttentionValue {
    using sti_t = int16_t;
    using lti_t = int16_t;
    using vlti_t = int16_t;

    sti_t sti;
    lti_t lti;
    vlti_t vlti;

    AttentionValue() : sti(0), lti(0), vlti(0) {}
    AttentionValue(sti_t s, lti_t l, vlti_t v = 0) : sti(s), lti(l), vlti(v) {}

    static const AttentionValue& DEFAULT_AV() {
        static AttentionValue av;
        return av;
    }

    bool is_default() const { return sti == 0 && lti == 0 && vlti == 0; }

    std::string to_string() const {
        std::ostringstream os;
        os << "(av " << sti << " " << lti << " " << vlti << ")";
        return os.str();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Value — Key-value store for arbitrary data on atoms
// ─────────────────────────────────────────────────────────────────────────────
class Value {
public:
    enum class Kind { FLOAT, STRING, LINK };

    Value() : kind_(Kind::FLOAT) {}
    explicit Value(const std::vector<double>& fv)
        : kind_(Kind::FLOAT), floats_(fv) {}
    explicit Value(const std::vector<std::string>& sv)
        : kind_(Kind::STRING), strings_(sv) {}

    Kind kind() const { return kind_; }
    const std::vector<double>& floats() const { return floats_; }
    const std::vector<std::string>& strings() const { return strings_; }

    void set_floats(const std::vector<double>& fv) {
        kind_ = Kind::FLOAT; floats_ = fv;
    }
    void set_strings(const std::vector<std::string>& sv) {
        kind_ = Kind::STRING; strings_ = sv;
    }

private:
    Kind kind_;
    std::vector<double> floats_;
    std::vector<std::string> strings_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Handle — Smart pointer to Atom (uint64_t ID in this implementation)
// ─────────────────────────────────────────────────────────────────────────────
using Handle = uint64_t;
static const Handle UNDEFINED_HANDLE = 0;

using HandleSeq = std::vector<Handle>;
using HandleSet = std::set<Handle>;

// ─────────────────────────────────────────────────────────────────────────────
// Atom — Base atom in the hypergraph
// ─────────────────────────────────────────────────────────────────────────────
struct Atom {
    Handle handle;
    Type type;
    std::string name;           // For nodes
    HandleSeq outgoing;         // For links
    TruthValue tv;
    AttentionValue av;
    std::map<Handle, Value> values;  // Key-value store (key = PredicateNode handle)

    // Incoming set: links pointing to this atom
    HandleSet incoming;

    Atom() : handle(UNDEFINED_HANDLE), type(types::NOTYPE) {}

    bool is_node() const { return is_node_type(type) || (!name.empty() && outgoing.empty()); }
    bool is_link() const { return is_link_type(type) || !outgoing.empty(); }

    size_t arity() const { return outgoing.size(); }

    void set_value(Handle key, const Value& val) { values[key] = val; }
    const Value* get_value(Handle key) const {
        auto it = values.find(key);
        return (it != values.end()) ? &it->second : nullptr;
    }

    std::string to_short_string() const {
        std::ostringstream os;
        os << "(" << type_name(type);
        if (is_node()) os << " \"" << name << "\"";
        if (!tv.is_default()) os << " " << tv.to_string();
        os << ")";
        return os.str();
    }

    // Full recursive string (requires AtomSpace for outgoing resolution)
    std::string to_string() const { return to_short_string(); }
};

// ─────────────────────────────────────────────────────────────────────────────
// AtomSpace — In-memory hypergraph container (mirrors opencog::AtomSpace)
// ─────────────────────────────────────────────────────────────────────────────
class AtomSpace {
public:
    AtomSpace() : next_handle_(1) {}

    // ── Node operations ──
    Handle add_node(Type type, const std::string& name,
                    const TruthValue& tv = DEFAULT_TV) {
        // Dedup: return existing if same type+name
        auto key = make_node_key(type, name);
        auto it = node_index_.find(key);
        if (it != node_index_.end()) {
            if (!tv.is_default()) {
                atoms_[it->second].tv = atoms_[it->second].tv.merge(tv);
            }
            return it->second;
        }
        Handle h = next_handle_++;
        Atom a;
        a.handle = h;
        a.type = type;
        a.name = name;
        a.tv = tv;
        atoms_[h] = a;
        node_index_[key] = h;
        type_index_[type].push_back(h);
        return h;
    }

    // ── Link operations ──
    Handle add_link(Type type, const HandleSeq& outgoing,
                    const TruthValue& tv = DEFAULT_TV) {
        // Dedup: return existing if same type+outgoing
        auto key = make_link_key(type, outgoing);
        auto it = link_index_.find(key);
        if (it != link_index_.end()) {
            if (!tv.is_default()) {
                atoms_[it->second].tv = atoms_[it->second].tv.merge(tv);
            }
            return it->second;
        }
        Handle h = next_handle_++;
        Atom a;
        a.handle = h;
        a.type = type;
        a.outgoing = outgoing;
        a.tv = tv;
        atoms_[h] = a;
        link_index_[key] = h;
        type_index_[type].push_back(h);
        // Update incoming sets
        for (auto oh : outgoing) {
            auto ait = atoms_.find(oh);
            if (ait != atoms_.end()) {
                ait->second.incoming.insert(h);
            }
        }
        return h;
    }

    // ── Retrieval ──
    const Atom* get_atom(Handle h) const {
        auto it = atoms_.find(h);
        return (it != atoms_.end()) ? &it->second : nullptr;
    }

    Atom* get_atom_mut(Handle h) {
        auto it = atoms_.find(h);
        return (it != atoms_.end()) ? &it->second : nullptr;
    }

    Handle get_node(Type type, const std::string& name) const {
        auto key = make_node_key(type, name);
        auto it = node_index_.find(key);
        return (it != node_index_.end()) ? it->second : UNDEFINED_HANDLE;
    }

    // Get all atoms of a given type (optionally including subtypes)
    HandleSeq get_by_type(Type type, bool subtype = false) const {
        if (!subtype) {
            auto it = type_index_.find(type);
            if (it != type_index_.end()) return it->second;
            return {};
        }
        HandleSeq result;
        for (auto& kv : type_index_) {
            if (TypeRegistry::instance().is_subtype(kv.first, type)) {
                result.insert(result.end(), kv.second.begin(), kv.second.end());
            }
        }
        return result;
    }

    // Get incoming set
    HandleSeq get_incoming(Handle h) const {
        auto it = atoms_.find(h);
        if (it == atoms_.end()) return {};
        return HandleSeq(it->second.incoming.begin(), it->second.incoming.end());
    }

    // Get incoming by type
    HandleSeq get_incoming_by_type(Handle h, Type type) const {
        HandleSeq result;
        auto inc = get_incoming(h);
        for (auto lh : inc) {
            auto a = get_atom(lh);
            if (a && a->type == type) result.push_back(lh);
        }
        return result;
    }

    // ── Removal ──
    bool remove_atom(Handle h, bool recursive = false) {
        auto it = atoms_.find(h);
        if (it == atoms_.end()) return false;
        const Atom& a = it->second;

        // If recursive, remove all links in incoming set first
        if (recursive) {
            HandleSeq inc(a.incoming.begin(), a.incoming.end());
            for (auto lh : inc) remove_atom(lh, true);
        } else {
            // Cannot remove if still referenced
            if (!a.incoming.empty()) return false;
        }

        // Remove from type index
        auto& tv = type_index_[a.type];
        tv.erase(std::remove(tv.begin(), tv.end(), h), tv.end());

        // Remove from node/link index
        if (a.is_node()) {
            node_index_.erase(make_node_key(a.type, a.name));
        } else {
            link_index_.erase(make_link_key(a.type, a.outgoing));
        }

        // Remove from incoming sets of outgoing atoms
        for (auto oh : a.outgoing) {
            auto ait = atoms_.find(oh);
            if (ait != atoms_.end()) {
                ait->second.incoming.erase(h);
            }
        }

        atoms_.erase(it);
        return true;
    }

    // ── Pattern matching ──
    // Simple pattern match: find all links of given type where outgoing matches
    // UNDEFINED_HANDLE in pattern acts as wildcard
    HandleSeq pattern_match(Type link_type, const HandleSeq& pattern) const {
        HandleSeq results;
        auto handles = get_by_type(link_type);
        for (auto h : handles) {
            const Atom* a = get_atom(h);
            if (!a || a->outgoing.size() != pattern.size()) continue;
            bool match = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (pattern[i] != UNDEFINED_HANDLE && pattern[i] != a->outgoing[i]) {
                    match = false;
                    break;
                }
            }
            if (match) results.push_back(h);
        }
        return results;
    }

    // Variable-aware pattern matching with bindings
    struct Binding {
        std::map<Handle, Handle> var_map;  // VariableNode handle → matched handle
    };

    std::vector<Binding> match_pattern(Handle pattern_h) const {
        std::vector<Binding> results;
        const Atom* pat = get_atom(pattern_h);
        if (!pat) return results;

        if (pat->is_link()) {
            // Try to match against all links of the same type
            auto candidates = get_by_type(pat->type);
            for (auto ch : candidates) {
                if (ch == pattern_h) continue;
                Binding b;
                if (try_match(pattern_h, ch, b)) {
                    results.push_back(b);
                }
            }
        }
        return results;
    }

    // ── Utilities ──
    size_t size() const { return atoms_.size(); }

    template<typename Fn>
    void foreach_atom(Fn fn) const {
        for (auto& kv : atoms_) fn(kv.second);
    }

    void clear() {
        atoms_.clear();
        node_index_.clear();
        link_index_.clear();
        type_index_.clear();
        next_handle_ = 1;
    }

    // Set truth value
    void set_tv(Handle h, const TruthValue& tv) {
        auto it = atoms_.find(h);
        if (it != atoms_.end()) it->second.tv = tv;
    }

    // Set attention value
    void set_av(Handle h, const AttentionValue& av) {
        auto it = atoms_.find(h);
        if (it != atoms_.end()) it->second.av = av;
    }

    // Set value on atom
    void set_value(Handle atom, Handle key, const Value& val) {
        auto it = atoms_.find(atom);
        if (it != atoms_.end()) it->second.set_value(key, val);
    }

    // Recursive to_string
    std::string atom_to_string(Handle h, int depth = 0) const {
        const Atom* a = get_atom(h);
        if (!a) return "(null)";
        std::string indent(depth * 2, ' ');
        std::ostringstream os;
        os << indent << "(" << type_name(a->type);
        if (a->is_node()) {
            os << " \"" << a->name << "\"";
        }
        if (!a->tv.is_default()) os << " " << a->tv.to_string();
        if (a->is_link()) {
            os << "\n";
            for (auto oh : a->outgoing) {
                os << atom_to_string(oh, depth + 1) << "\n";
            }
            os << indent;
        }
        os << ")";
        return os.str();
    }

private:
    Handle next_handle_;
    std::unordered_map<Handle, Atom> atoms_;
    std::unordered_map<std::string, Handle> node_index_;
    std::unordered_map<std::string, Handle> link_index_;
    std::unordered_map<Type, HandleSeq> type_index_;

    static std::string make_node_key(Type t, const std::string& name) {
        return std::to_string(t) + ":" + name;
    }

    static std::string make_link_key(Type t, const HandleSeq& out) {
        std::string key = std::to_string(t) + ":";
        for (auto h : out) { key += std::to_string(h) + ","; }
        return key;
    }

    // Recursive pattern matching with variable binding
    bool try_match(Handle pat_h, Handle target_h, Binding& b) const {
        const Atom* pat = get_atom(pat_h);
        const Atom* tgt = get_atom(target_h);
        if (!pat || !tgt) return false;

        // If pattern is a VariableNode, bind it
        if (pat->type == types::VARIABLE_NODE) {
            auto it = b.var_map.find(pat_h);
            if (it != b.var_map.end()) {
                return it->second == target_h;  // Must be consistent
            }
            b.var_map[pat_h] = target_h;
            return true;
        }

        // Types must match
        if (pat->type != tgt->type) return false;

        // Nodes: names must match
        if (pat->is_node()) return pat->name == tgt->name;

        // Links: arity must match, recursively match outgoing
        if (pat->outgoing.size() != tgt->outgoing.size()) return false;
        for (size_t i = 0; i < pat->outgoing.size(); ++i) {
            if (!try_match(pat->outgoing[i], tgt->outgoing[i], b)) return false;
        }
        return true;
    }
};

} // namespace oc

#endif // OC_ATOMSPACE_HPP
