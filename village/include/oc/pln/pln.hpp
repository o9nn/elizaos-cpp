// oc::pln — Header-only C++11 reimplementation of opencog/pln
// Probabilistic Logic Networks: deduction, induction, abduction, revision,
// fuzzy conjunction, modus ponens, inheritance reasoning
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_PLN_HPP
#define OC_PLN_HPP

#include "../atomspace/atomspace.hpp"
#include "../ure/ure.hpp"

namespace oc {
namespace pln {

// ─────────────────────────────────────────────────────────────────────────────
// PLN Truth Value Formulas — The actual probability math
// ─────────────────────────────────────────────────────────────────────────────
namespace formulas {

// Deduction: A→B, B→C  ⊢  A→C
// sAC = sAB * sBC + (1 - sAB) * (sC - sB * sBC) / (1 - sB)
inline TruthValue deduction(const TruthValue& AB, const TruthValue& BC,
                             double sA, double sB, double sC) {
    double sAB = AB.mean;
    double sBC = BC.mean;

    double sAC;
    if (std::fabs(1.0 - sB) < 1e-12) {
        sAC = sAB * sBC;
    } else {
        sAC = sAB * sBC + (1.0 - sAB) * (sC - sB * sBC) / (1.0 - sB);
    }
    sAC = std::max(0.0, std::min(1.0, sAC));

    // Confidence: min of premises, penalized
    double conf = AB.confidence * BC.confidence * sB;
    conf = std::max(0.0, std::min(1.0, conf));

    return TruthValue(sAC, conf);
}

// Induction: A→B, A→C  ⊢  B→C
// sBC = sAB * sAC + (1 - sAB) * (sC - sA * sAC) / (1 - sA)
inline TruthValue induction(const TruthValue& AB, const TruthValue& AC,
                              double sA, double sB, double sC) {
    double sAB = AB.mean;
    double sAC = AC.mean;

    double sBC;
    if (std::fabs(1.0 - sA) < 1e-12) {
        sBC = sAB * sAC;
    } else {
        sBC = sAB * sAC + (1.0 - sAB) * (sC - sA * sAC) / (1.0 - sA);
    }
    sBC = std::max(0.0, std::min(1.0, sBC));

    double conf = AB.confidence * AC.confidence * sA;
    conf = std::max(0.0, std::min(1.0, conf));

    return TruthValue(sBC, conf);
}

// Abduction: B→A, B→C  ⊢  A→C
// sAC = sBA * sBC + (1 - sBA) * (sC - sB * sBC) / (1 - sB)
inline TruthValue abduction(const TruthValue& BA, const TruthValue& BC,
                              double sA, double sB, double sC) {
    double sBA = BA.mean;
    double sBC = BC.mean;

    double sAC;
    if (std::fabs(1.0 - sB) < 1e-12) {
        sAC = sBA * sBC;
    } else {
        sAC = sBA * sBC + (1.0 - sBA) * (sC - sB * sBC) / (1.0 - sB);
    }
    sAC = std::max(0.0, std::min(1.0, sAC));

    double conf = BA.confidence * BC.confidence * sB;
    conf = std::max(0.0, std::min(1.0, conf));

    return TruthValue(sAC, conf);
}

// Revision: merge two independent estimates of the same proposition
inline TruthValue revision(const TruthValue& a, const TruthValue& b) {
    return a.merge(b);
}

// Modus Ponens: A, A→B  ⊢  B
// sB = sA * sAB + (1 - sA) * sAnotB
// where sAnotB ≈ (sB - sA * sAB) / (1 - sA)
inline TruthValue modus_ponens(const TruthValue& A, const TruthValue& AB,
                                double sB_prior = 0.5) {
    double sA = A.mean;
    double sAB = AB.mean;

    double sB;
    if (std::fabs(1.0 - sA) < 1e-12) {
        sB = sAB;
    } else {
        double sAnotB = (sB_prior - sA * sAB) / (1.0 - sA);
        sAnotB = std::max(0.0, std::min(1.0, sAnotB));
        sB = sA * sAB + (1.0 - sA) * sAnotB;
    }
    sB = std::max(0.0, std::min(1.0, sB));

    double conf = A.confidence * AB.confidence;
    return TruthValue(sB, conf);
}

// Fuzzy conjunction: AND(A, B) = min(sA, sB)
inline TruthValue fuzzy_conjunction(const TruthValue& A, const TruthValue& B) {
    double s = std::min(A.mean, B.mean);
    double c = std::min(A.confidence, B.confidence);
    return TruthValue(s, c);
}

// Fuzzy disjunction: OR(A, B) = max(sA, sB)
inline TruthValue fuzzy_disjunction(const TruthValue& A, const TruthValue& B) {
    double s = std::max(A.mean, B.mean);
    double c = std::min(A.confidence, B.confidence);
    return TruthValue(s, c);
}

// Negation: NOT(A) = 1 - sA
inline TruthValue negation(const TruthValue& A) {
    return TruthValue(1.0 - A.mean, A.confidence);
}

// Similarity from inheritance: sim(A,B) = sAB * sBA
inline TruthValue similarity_from_inheritance(const TruthValue& AB,
                                                const TruthValue& BA) {
    double s = AB.mean * BA.mean;
    double c = std::min(AB.confidence, BA.confidence);
    return TruthValue(s, c);
}

// Inheritance from member: P(A→B) = |A∩B| / |A|
inline TruthValue inheritance_from_member(double n_AB, double n_A) {
    if (n_A < 1e-12) return TruthValue(0.0, 0.0);
    double s = n_AB / n_A;
    double c = TruthValue::count_to_confidence(n_A);
    return TruthValue(s, c);
}

} // namespace formulas

// ─────────────────────────────────────────────────────────────────────────────
// PLNReasoner — High-level PLN reasoning engine over AtomSpace
// ─────────────────────────────────────────────────────────────────────────────
class PLNReasoner {
public:
    explicit PLNReasoner(AtomSpace& as) : as_(as) {}

    // Store an inheritance relationship
    Handle store_inheritance(const std::string& sub, const std::string& super,
                              double strength, double confidence) {
        Handle h_sub = as_.add_node(types::CONCEPT_NODE, sub);
        Handle h_super = as_.add_node(types::CONCEPT_NODE, super);
        return as_.add_link(types::INHERITANCE_LINK, {h_sub, h_super},
                            TruthValue(strength, confidence));
    }

    // Store a similarity relationship
    Handle store_similarity(const std::string& a, const std::string& b,
                             double strength, double confidence) {
        Handle ha = as_.add_node(types::CONCEPT_NODE, a);
        Handle hb = as_.add_node(types::CONCEPT_NODE, b);
        return as_.add_link(types::SIMILARITY_LINK, {ha, hb},
                            TruthValue(strength, confidence));
    }

    // Store an evaluation
    Handle store_evaluation(const std::string& predicate,
                             const std::vector<std::string>& args,
                             double strength, double confidence) {
        Handle pred = as_.add_node(types::PREDICATE_NODE, predicate);
        HandleSeq arg_handles;
        for (auto& a : args) {
            arg_handles.push_back(as_.add_node(types::CONCEPT_NODE, a));
        }
        Handle list = as_.add_link(types::LIST_LINK, arg_handles);
        return as_.add_link(types::EVALUATION_LINK, {pred, list},
                            TruthValue(strength, confidence));
    }

    // Run deductive inference on all inheritance chains
    struct Conclusion {
        Handle handle;
        std::string description;
        TruthValue tv;
        std::string rule_name;
    };

    std::vector<Conclusion> deduce_all() {
        std::vector<Conclusion> results;

        // Get all inheritance links
        auto inh_links = as_.get_by_type(types::INHERITANCE_LINK);

        // For each pair (A→B, B→C), deduce A→C
        for (size_t i = 0; i < inh_links.size(); ++i) {
            const Atom* ab = as_.get_atom(inh_links[i]);
            if (!ab || ab->outgoing.size() != 2) continue;
            Handle hA = ab->outgoing[0];
            Handle hB = ab->outgoing[1];

            for (size_t j = 0; j < inh_links.size(); ++j) {
                if (i == j) continue;
                const Atom* bc = as_.get_atom(inh_links[j]);
                if (!bc || bc->outgoing.size() != 2) continue;
                if (bc->outgoing[0] != hB) continue;
                Handle hC = bc->outgoing[1];
                if (hC == hA) continue;  // Skip cycles

                // Check if A→C already exists
                auto existing = as_.pattern_match(types::INHERITANCE_LINK, {hA, hC});
                if (!existing.empty()) continue;

                // Get concept strengths (use TV mean as proxy)
                const Atom* aA = as_.get_atom(hA);
                const Atom* aB = as_.get_atom(hB);
                const Atom* aC = as_.get_atom(hC);
                double sA = aA ? aA->tv.mean : 0.5;
                double sB = aB ? aB->tv.mean : 0.5;
                double sC = aC ? aC->tv.mean : 0.5;
                if (sA < 1e-12) sA = 0.5;
                if (sB < 1e-12) sB = 0.5;
                if (sC < 1e-12) sC = 0.5;

                TruthValue tv = formulas::deduction(ab->tv, bc->tv, sA, sB, sC);
                Handle h = as_.add_link(types::INHERITANCE_LINK, {hA, hC}, tv);

                const Atom* nameA = as_.get_atom(hA);
                const Atom* nameC = as_.get_atom(hC);
                std::string desc = (nameA ? nameA->name : "?") + " → " +
                                   (nameC ? nameC->name : "?");

                results.push_back({h, desc, tv, "deduction"});
            }
        }

        return results;
    }

    // Run modus ponens on implications
    std::vector<Conclusion> modus_ponens_all() {
        std::vector<Conclusion> results;

        auto impl_links = as_.get_by_type(types::IMPLICATION_LINK);
        for (auto lh : impl_links) {
            const Atom* impl = as_.get_atom(lh);
            if (!impl || impl->outgoing.size() != 2) continue;
            Handle hA = impl->outgoing[0];
            Handle hB = impl->outgoing[1];

            const Atom* aA = as_.get_atom(hA);
            if (!aA || aA->tv.is_default()) continue;

            TruthValue tv = formulas::modus_ponens(aA->tv, impl->tv);
            as_.set_tv(hB, tv);

            const Atom* aB = as_.get_atom(hB);
            std::string desc = "Conclude " + (aB ? aB->to_short_string() : "?");
            results.push_back({hB, desc, tv, "modus_ponens"});
        }

        return results;
    }

    // Full reasoning cycle: deduction + modus ponens + revision
    std::vector<Conclusion> reason(int iterations = 1) {
        std::vector<Conclusion> all_results;
        for (int i = 0; i < iterations; ++i) {
            auto ded = deduce_all();
            auto mp = modus_ponens_all();
            all_results.insert(all_results.end(), ded.begin(), ded.end());
            all_results.insert(all_results.end(), mp.begin(), mp.end());
        }
        return all_results;
    }

    // Query: get truth value of an inheritance relationship
    TruthValue query_inheritance(const std::string& sub,
                                  const std::string& super) {
        Handle h_sub = as_.get_node(types::CONCEPT_NODE, sub);
        Handle h_super = as_.get_node(types::CONCEPT_NODE, super);
        if (h_sub == UNDEFINED_HANDLE || h_super == UNDEFINED_HANDLE) {
            return DEFAULT_TV;
        }
        auto matches = as_.pattern_match(types::INHERITANCE_LINK, {h_sub, h_super});
        if (matches.empty()) return DEFAULT_TV;
        const Atom* a = as_.get_atom(matches[0]);
        return a ? a->tv : DEFAULT_TV;
    }

private:
    AtomSpace& as_;
};

// ─────────────────────────────────────────────────────────────────────────────
// PLN Rule Builders — Create URE-compatible rules for PLN formulas
// ─────────────────────────────────────────────────────────────────────────────
namespace rule_builders {

// Create a deduction rule as a URE Rule
// Pattern: (InheritanceLink $X $Y) with context (InheritanceLink $Y $Z)
// Rewrite: (InheritanceLink $X $Z)
inline ure::RulePtr make_deduction_rule(AtomSpace& as) {
    Handle vX = as.add_node(types::VARIABLE_NODE, "$X");
    Handle vY = as.add_node(types::VARIABLE_NODE, "$Y");
    Handle vZ = as.add_node(types::VARIABLE_NODE, "$Z");

    Handle pattern = as.add_link(types::INHERITANCE_LINK, {vX, vY});
    Handle rewrite = as.add_link(types::INHERITANCE_LINK, {vX, vZ});

    return std::make_shared<ure::Rule>(
        "pln-deduction", pattern, rewrite, HandleSeq{vX, vY, vZ}, 1.0);
}

// Create a modus ponens rule
inline ure::RulePtr make_modus_ponens_rule(AtomSpace& as) {
    Handle vA = as.add_node(types::VARIABLE_NODE, "$A");
    Handle vB = as.add_node(types::VARIABLE_NODE, "$B");

    Handle pattern = as.add_link(types::IMPLICATION_LINK, {vA, vB});
    Handle rewrite = as.add_link(types::LIST_LINK, {vB});

    return std::make_shared<ure::Rule>(
        "pln-modus-ponens", pattern, rewrite, HandleSeq{vA, vB}, 0.8);
}

} // namespace rule_builders

} // namespace pln
} // namespace oc

#endif // OC_PLN_HPP
