// oc::ure — Header-only C++11 reimplementation of opencog/ure
// Unified Rule Engine: Rule, RuleSet, Unifier, ForwardChainer, BackwardChainer
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_URE_HPP
#define OC_URE_HPP

#include "../atomspace/atomspace.hpp"

namespace oc {
namespace ure {

// ─────────────────────────────────────────────────────────────────────────────
// Unifier — Unify two atomese expressions, producing variable bindings
// ─────────────────────────────────────────────────────────────────────────────
struct Substitution {
    std::map<Handle, Handle> var_map;  // VariableNode → concrete atom

    bool has(Handle var) const { return var_map.count(var) > 0; }
    Handle get(Handle var) const {
        auto it = var_map.find(var);
        return (it != var_map.end()) ? it->second : UNDEFINED_HANDLE;
    }

    // Compose two substitutions
    Substitution compose(const Substitution& other) const {
        Substitution result = *this;
        for (auto& kv : other.var_map) {
            if (!result.has(kv.first)) {
                result.var_map[kv.first] = kv.second;
            }
        }
        return result;
    }

    bool empty() const { return var_map.empty(); }
    size_t size() const { return var_map.size(); }
};

class Unifier {
public:
    explicit Unifier(const AtomSpace& as) : as_(as) {}

    // Attempt to unify two atoms, returning substitution if successful
    bool unify(Handle a, Handle b, Substitution& sub) const {
        const Atom* aa = as_.get_atom(a);
        const Atom* ab = as_.get_atom(b);
        if (!aa || !ab) return false;

        // Variable matches anything
        if (aa->type == types::VARIABLE_NODE) {
            if (sub.has(a)) {
                return sub.get(a) == b;
            }
            sub.var_map[a] = b;
            return true;
        }
        if (ab->type == types::VARIABLE_NODE) {
            if (sub.has(b)) {
                return sub.get(b) == a;
            }
            sub.var_map[b] = a;
            return true;
        }

        // Types must match
        if (aa->type != ab->type) return false;

        // Nodes: names must match
        if (aa->is_node()) return aa->name == ab->name;

        // Links: arity must match, recursively unify outgoing
        if (aa->outgoing.size() != ab->outgoing.size()) return false;
        for (size_t i = 0; i < aa->outgoing.size(); ++i) {
            if (!unify(aa->outgoing[i], ab->outgoing[i], sub)) return false;
        }
        return true;
    }

    // Apply substitution to an atom, returning a new handle
    Handle apply(Handle h, const Substitution& sub, AtomSpace& target) const {
        const Atom* a = as_.get_atom(h);
        if (!a) return UNDEFINED_HANDLE;

        // Variable: substitute if bound
        if (a->type == types::VARIABLE_NODE) {
            if (sub.has(h)) return sub.get(h);
            return h;
        }

        // Node: copy as-is
        if (a->is_node()) {
            return target.add_node(a->type, a->name, a->tv);
        }

        // Link: recursively apply to outgoing
        HandleSeq new_outgoing;
        for (auto oh : a->outgoing) {
            Handle nh = apply(oh, sub, target);
            if (nh == UNDEFINED_HANDLE) return UNDEFINED_HANDLE;
            new_outgoing.push_back(nh);
        }
        return target.add_link(a->type, new_outgoing, a->tv);
    }

private:
    const AtomSpace& as_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Rule — An inference rule represented as a pattern → rewrite mapping
// ─────────────────────────────────────────────────────────────────────────────
class Rule {
public:
    Rule() : weight_(1.0) {}
    Rule(const std::string& name, Handle pattern, Handle rewrite,
         const HandleSeq& variables, double weight = 1.0)
        : name_(name), pattern_(pattern), rewrite_(rewrite),
          variables_(variables), weight_(weight) {}

    const std::string& name() const { return name_; }
    Handle pattern() const { return pattern_; }
    Handle rewrite() const { return rewrite_; }
    const HandleSeq& variables() const { return variables_; }
    double weight() const { return weight_; }
    void set_weight(double w) { weight_ = w; }

    // Check if a rule is applicable to a given atom
    bool is_applicable(Handle target, const AtomSpace& as) const {
        Unifier unifier(as);
        Substitution sub;
        return unifier.unify(pattern_, target, sub);
    }

    // Apply rule to a target atom, producing new atoms
    HandleSeq apply(Handle target, AtomSpace& as) const {
        Unifier unifier(as);
        Substitution sub;
        if (!unifier.unify(pattern_, target, sub)) return {};

        Handle result = unifier.apply(rewrite_, sub, as);
        if (result == UNDEFINED_HANDLE) return {};
        return {result};
    }

private:
    std::string name_;
    Handle pattern_;
    Handle rewrite_;
    HandleSeq variables_;
    double weight_;
};

using RulePtr = std::shared_ptr<Rule>;

// ─────────────────────────────────────────────────────────────────────────────
// RuleSet — Collection of rules with weighted selection
// ─────────────────────────────────────────────────────────────────────────────
class RuleSet {
public:
    void add_rule(RulePtr rule) { rules_.push_back(rule); }

    void remove_rule(const std::string& name) {
        rules_.erase(
            std::remove_if(rules_.begin(), rules_.end(),
                [&](const RulePtr& r) { return r->name() == name; }),
            rules_.end());
    }

    RulePtr get_rule(const std::string& name) const {
        for (auto& r : rules_) {
            if (r->name() == name) return r;
        }
        return nullptr;
    }

    const std::vector<RulePtr>& rules() const { return rules_; }
    size_t size() const { return rules_.size(); }
    bool empty() const { return rules_.empty(); }

    // Weighted random selection
    RulePtr select(util::RandGen& rng) const {
        if (rules_.empty()) return nullptr;
        double total = 0.0;
        for (auto& r : rules_) total += r->weight();
        double pick = rng.randdouble() * total;
        double acc = 0.0;
        for (auto& r : rules_) {
            acc += r->weight();
            if (pick <= acc) return r;
        }
        return rules_.back();
    }

    // Get all applicable rules for a target
    std::vector<RulePtr> get_applicable(Handle target, const AtomSpace& as) const {
        std::vector<RulePtr> result;
        for (auto& r : rules_) {
            if (r->is_applicable(target, as)) result.push_back(r);
        }
        return result;
    }

private:
    std::vector<RulePtr> rules_;
};

// ─────────────────────────────────────────────────────────────────────────────
// UREConfig — Configuration for the rule engine
// ─────────────────────────────────────────────────────────────────────────────
struct UREConfig {
    int max_iterations = 100;
    int max_results = 10;
    bool fc_full_rule_application = true;
    enum class SourceSelection { TV_FITNESS, STI, UNIFORM };
    SourceSelection source_selection = SourceSelection::UNIFORM;
    double complexity_penalty = 0.01;
};

// ─────────────────────────────────────────────────────────────────────────────
// ForwardChainer — Data-driven forward chaining inference
// ─────────────────────────────────────────────────────────────────────────────
class ForwardChainer {
public:
    ForwardChainer(AtomSpace& as, const RuleSet& rules,
                    const UREConfig& config = UREConfig())
        : as_(as), rules_(rules), config_(config) {}

    // Run forward chaining from a source atom
    HandleSeq run(Handle source) {
        HandleSeq results;
        HandleSet visited;
        HandleSeq frontier = {source};

        for (int iter = 0; iter < config_.max_iterations; ++iter) {
            if (frontier.empty()) break;
            if (static_cast<int>(results.size()) >= config_.max_results) break;

            // Select source from frontier
            Handle src = select_source(frontier);
            if (visited.count(src)) {
                // Remove from frontier and continue
                frontier.erase(std::remove(frontier.begin(), frontier.end(), src),
                               frontier.end());
                continue;
            }
            visited.insert(src);

            // Find applicable rules
            auto applicable = rules_.get_applicable(src, as_);
            if (applicable.empty()) {
                frontier.erase(std::remove(frontier.begin(), frontier.end(), src),
                               frontier.end());
                continue;
            }

            // Apply each applicable rule
            for (auto& rule : applicable) {
                auto new_atoms = rule->apply(src, as_);
                for (auto h : new_atoms) {
                    if (!visited.count(h)) {
                        frontier.push_back(h);
                        results.push_back(h);
                    }
                }
            }
        }

        return results;
    }

    const UREConfig& config() const { return config_; }
    UREConfig& config() { return config_; }

private:
    AtomSpace& as_;
    const RuleSet& rules_;
    UREConfig config_;
    util::RandGen rng_;

    Handle select_source(const HandleSeq& frontier) {
        switch (config_.source_selection) {
            case UREConfig::SourceSelection::TV_FITNESS: {
                // Select by truth value fitness (mean * confidence)
                Handle best = frontier[0];
                double best_fitness = -1.0;
                for (auto h : frontier) {
                    const Atom* a = as_.get_atom(h);
                    if (!a) continue;
                    double fitness = a->tv.mean * a->tv.confidence;
                    if (fitness > best_fitness) {
                        best_fitness = fitness;
                        best = h;
                    }
                }
                return best;
            }
            case UREConfig::SourceSelection::STI: {
                Handle best = frontier[0];
                int16_t best_sti = -32768;
                for (auto h : frontier) {
                    const Atom* a = as_.get_atom(h);
                    if (!a) continue;
                    if (a->av.sti > best_sti) {
                        best_sti = a->av.sti;
                        best = h;
                    }
                }
                return best;
            }
            case UREConfig::SourceSelection::UNIFORM:
            default:
                return rng_.pick(frontier);
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// BackwardChainer — Goal-driven backward chaining inference
// ─────────────────────────────────────────────────────────────────────────────
class BackwardChainer {
public:
    BackwardChainer(AtomSpace& as, const RuleSet& rules,
                     const UREConfig& config = UREConfig())
        : as_(as), rules_(rules), config_(config) {}

    // Run backward chaining to prove a target
    HandleSeq run(Handle target) {
        HandleSeq results;
        int depth = 0;
        if (prove(target, results, depth)) {
            return results;
        }
        return results;  // Partial results
    }

private:
    AtomSpace& as_;
    const RuleSet& rules_;
    UREConfig config_;

    bool prove(Handle target, HandleSeq& results, int depth) {
        if (depth > config_.max_iterations) return false;
        if (static_cast<int>(results.size()) >= config_.max_results) return false;

        // Check if target already exists in AtomSpace with high TV
        const Atom* a = as_.get_atom(target);
        if (a && a->tv.confidence > 0.5 && a->tv.mean > 0.5) {
            results.push_back(target);
            return true;
        }

        // Find rules whose rewrite pattern could produce the target
        for (auto& rule : rules_.rules()) {
            Unifier unifier(as_);
            Substitution sub;
            if (unifier.unify(rule->rewrite(), target, sub)) {
                // The rule's rewrite matches the target.
                // Now we need to prove the rule's pattern (premises)
                Handle premise = unifier.apply(rule->pattern(), sub, as_);
                if (premise != UNDEFINED_HANDLE) {
                    // Try to prove the premise
                    if (prove(premise, results, depth + 1)) {
                        // Apply the rule to produce the target
                        auto new_atoms = rule->apply(premise, as_);
                        for (auto h : new_atoms) results.push_back(h);
                        return true;
                    }
                }
            }
        }

        return false;
    }
};

} // namespace ure
} // namespace oc

#endif // OC_URE_HPP
