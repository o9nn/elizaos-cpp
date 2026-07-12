// oc::matrix — Header-only C++11 reimplementation of opencog/matrix
// Sparse vector/matrix library over AtomSpace: pair API, marginals,
// cosine similarity, mutual information, entropy, frequency counting
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_MATRIX_HPP
#define OC_MATRIX_HPP

#include "../atomspace/atomspace.hpp"

namespace oc {
namespace matrix {

// ─────────────────────────────────────────────────────────────────────────────
// PairAPI — Low-level API for accessing pairs of atoms as matrix entries
// Pairs are stored as: (EvaluationLink (PredicateNode "pred") (ListLink row col))
// ─────────────────────────────────────────────────────────────────────────────
class PairAPI {
public:
    PairAPI(AtomSpace& as, const std::string& predicate_name)
        : as_(as), pred_name_(predicate_name) {
        pred_handle_ = as_.add_node(types::PREDICATE_NODE, predicate_name);
    }

    // Set a pair value (count)
    Handle set_pair(Handle row, Handle col, double count) {
        Handle list = as_.add_link(types::LIST_LINK, {row, col});
        Handle eval = as_.add_link(types::EVALUATION_LINK, {pred_handle_, list});
        as_.set_value(eval, count_key(),
                      Value(std::vector<double>{count}));
        return eval;
    }

    // Get pair count
    double get_pair_count(Handle row, Handle col) const {
        Handle list = find_list(row, col);
        if (list == UNDEFINED_HANDLE) return 0.0;
        auto evals = as_.pattern_match(types::EVALUATION_LINK,
                                        {pred_handle_, list});
        if (evals.empty()) return 0.0;
        const Atom* a = as_.get_atom(evals[0]);
        if (!a) return 0.0;
        const Value* v = a->get_value(count_key());
        if (!v || v->floats().empty()) return 0.0;
        return v->floats()[0];
    }

    // Increment pair count
    void increment_pair(Handle row, Handle col, double amount = 1.0) {
        double current = get_pair_count(row, col);
        set_pair(row, col, current + amount);
    }

    // Get all pairs for a given row (left star: row, *)
    std::vector<std::pair<Handle, double>> get_row(Handle row) const {
        std::vector<std::pair<Handle, double>> result;
        auto evals = as_.get_by_type(types::EVALUATION_LINK);
        for (auto eh : evals) {
            const Atom* e = as_.get_atom(eh);
            if (!e || e->outgoing.size() != 2) continue;
            if (e->outgoing[0] != pred_handle_) continue;
            const Atom* list = as_.get_atom(e->outgoing[1]);
            if (!list || list->outgoing.size() != 2) continue;
            if (list->outgoing[0] != row) continue;
            const Value* v = e->get_value(count_key());
            double cnt = (v && !v->floats().empty()) ? v->floats()[0] : 0.0;
            if (cnt > 0.0) result.push_back({list->outgoing[1], cnt});
        }
        return result;
    }

    // Get all pairs for a given column (right star: *, col)
    std::vector<std::pair<Handle, double>> get_col(Handle col) const {
        std::vector<std::pair<Handle, double>> result;
        auto evals = as_.get_by_type(types::EVALUATION_LINK);
        for (auto eh : evals) {
            const Atom* e = as_.get_atom(eh);
            if (!e || e->outgoing.size() != 2) continue;
            if (e->outgoing[0] != pred_handle_) continue;
            const Atom* list = as_.get_atom(e->outgoing[1]);
            if (!list || list->outgoing.size() != 2) continue;
            if (list->outgoing[1] != col) continue;
            const Value* v = e->get_value(count_key());
            double cnt = (v && !v->floats().empty()) ? v->floats()[0] : 0.0;
            if (cnt > 0.0) result.push_back({list->outgoing[0], cnt});
        }
        return result;
    }

    // Get all unique row handles
    HandleSeq get_all_rows() const {
        HandleSet rows;
        auto evals = as_.get_by_type(types::EVALUATION_LINK);
        for (auto eh : evals) {
            const Atom* e = as_.get_atom(eh);
            if (!e || e->outgoing.size() != 2) continue;
            if (e->outgoing[0] != pred_handle_) continue;
            const Atom* list = as_.get_atom(e->outgoing[1]);
            if (!list || list->outgoing.size() != 2) continue;
            rows.insert(list->outgoing[0]);
        }
        return HandleSeq(rows.begin(), rows.end());
    }

    // Get all unique column handles
    HandleSeq get_all_cols() const {
        HandleSet cols;
        auto evals = as_.get_by_type(types::EVALUATION_LINK);
        for (auto eh : evals) {
            const Atom* e = as_.get_atom(eh);
            if (!e || e->outgoing.size() != 2) continue;
            if (e->outgoing[0] != pred_handle_) continue;
            const Atom* list = as_.get_atom(e->outgoing[1]);
            if (!list || list->outgoing.size() != 2) continue;
            cols.insert(list->outgoing[1]);
        }
        return HandleSeq(cols.begin(), cols.end());
    }

    Handle predicate() const { return pred_handle_; }
    const std::string& predicate_name() const { return pred_name_; }

private:
    AtomSpace& as_;
    std::string pred_name_;
    Handle pred_handle_;

    Handle count_key() const {
        // Use a special key for counts
        static Handle k = 0;
        if (k == 0) {
            // Use a const_cast-free approach
            k = const_cast<AtomSpace&>(as_).add_node(
                types::PREDICATE_NODE, "*-CountKey-*");
        }
        return k;
    }

    Handle find_list(Handle row, Handle col) const {
        auto lists = as_.pattern_match(types::LIST_LINK, {row, col});
        return lists.empty() ? UNDEFINED_HANDLE : lists[0];
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Marginals — Compute row/column marginal sums and total
// ─────────────────────────────────────────────────────────────────────────────
class Marginals {
public:
    explicit Marginals(PairAPI& api) : api_(api) {}

    // Row marginal: sum of counts for a given row
    double row_marginal(Handle row) const {
        auto pairs = api_.get_row(row);
        double sum = 0.0;
        for (auto& p : pairs) sum += p.second;
        return sum;
    }

    // Column marginal: sum of counts for a given column
    double col_marginal(Handle col) const {
        auto pairs = api_.get_col(col);
        double sum = 0.0;
        for (auto& p : pairs) sum += p.second;
        return sum;
    }

    // Total count across all pairs
    double total() const {
        auto rows = api_.get_all_rows();
        double sum = 0.0;
        for (auto r : rows) sum += row_marginal(r);
        return sum;
    }

    // Row count (number of non-zero entries in row)
    size_t row_count(Handle row) const {
        return api_.get_row(row).size();
    }

    // Column count
    size_t col_count(Handle col) const {
        return api_.get_col(col).size();
    }

    // Number of non-zero pairs
    size_t num_pairs() const {
        auto rows = api_.get_all_rows();
        size_t n = 0;
        for (auto r : rows) n += row_count(r);
        return n;
    }

private:
    PairAPI& api_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Similarity — Cosine similarity and other vector metrics
// ─────────────────────────────────────────────────────────────────────────────
class Similarity {
public:
    explicit Similarity(PairAPI& api) : api_(api) {}

    // Cosine similarity between two rows
    double cosine(Handle row_a, Handle row_b) const {
        auto va = api_.get_row(row_a);
        auto vb = api_.get_row(row_b);

        // Build maps for fast lookup
        std::unordered_map<Handle, double> map_a;
        for (auto& p : va) map_a[p.first] = p.second;

        double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
        for (auto& p : va) norm_a += p.second * p.second;
        for (auto& p : vb) {
            norm_b += p.second * p.second;
            auto it = map_a.find(p.first);
            if (it != map_a.end()) {
                dot += it->second * p.second;
            }
        }

        double denom = std::sqrt(norm_a) * std::sqrt(norm_b);
        if (denom < 1e-12) return 0.0;
        return dot / denom;
    }

    // Jaccard similarity between two rows (treating as sets)
    double jaccard(Handle row_a, Handle row_b) const {
        auto va = api_.get_row(row_a);
        auto vb = api_.get_row(row_b);

        HandleSet set_a, set_b;
        for (auto& p : va) set_a.insert(p.first);
        for (auto& p : vb) set_b.insert(p.first);

        size_t intersection = 0;
        for (auto h : set_a) {
            if (set_b.count(h)) intersection++;
        }

        size_t union_size = set_a.size() + set_b.size() - intersection;
        if (union_size == 0) return 0.0;
        return static_cast<double>(intersection) / union_size;
    }

    // L2 distance between two rows
    double l2_distance(Handle row_a, Handle row_b) const {
        auto va = api_.get_row(row_a);
        auto vb = api_.get_row(row_b);

        std::unordered_map<Handle, double> map_a, map_b;
        HandleSet all_cols;
        for (auto& p : va) { map_a[p.first] = p.second; all_cols.insert(p.first); }
        for (auto& p : vb) { map_b[p.first] = p.second; all_cols.insert(p.first); }

        double sum_sq = 0.0;
        for (auto c : all_cols) {
            double a = map_a.count(c) ? map_a[c] : 0.0;
            double b = map_b.count(c) ? map_b[c] : 0.0;
            sum_sq += (a - b) * (a - b);
        }
        return std::sqrt(sum_sq);
    }

    // Dot product between two rows
    double dot_product(Handle row_a, Handle row_b) const {
        auto va = api_.get_row(row_a);
        auto vb = api_.get_row(row_b);

        std::unordered_map<Handle, double> map_a;
        for (auto& p : va) map_a[p.first] = p.second;

        double dot = 0.0;
        for (auto& p : vb) {
            auto it = map_a.find(p.first);
            if (it != map_a.end()) dot += it->second * p.second;
        }
        return dot;
    }

private:
    PairAPI& api_;
};

// ─────────────────────────────────────────────────────────────────────────────
// MutualInformation — Pointwise and average MI computation
// ─────────────────────────────────────────────────────────────────────────────
class MutualInformation {
public:
    MutualInformation(PairAPI& api, Marginals& marg)
        : api_(api), marg_(marg) {}

    // Pointwise mutual information: PMI(x,y) = log2(P(x,y) / (P(x)*P(y)))
    double pmi(Handle row, Handle col) const {
        double total = marg_.total();
        if (total < 1e-12) return 0.0;

        double p_xy = api_.get_pair_count(row, col) / total;
        double p_x = marg_.row_marginal(row) / total;
        double p_y = marg_.col_marginal(col) / total;

        if (p_xy < 1e-12 || p_x < 1e-12 || p_y < 1e-12) return 0.0;
        return std::log2(p_xy / (p_x * p_y));
    }

    // Average mutual information across all pairs
    double average_mi() const {
        double total = marg_.total();
        if (total < 1e-12) return 0.0;

        double mi = 0.0;
        auto rows = api_.get_all_rows();
        for (auto r : rows) {
            auto pairs = api_.get_row(r);
            for (auto& p : pairs) {
                double p_xy = p.second / total;
                double p_x = marg_.row_marginal(r) / total;
                double p_y = marg_.col_marginal(p.first) / total;
                if (p_xy > 1e-12 && p_x > 1e-12 && p_y > 1e-12) {
                    mi += p_xy * std::log2(p_xy / (p_x * p_y));
                }
            }
        }
        return mi;
    }

    // Conditional entropy H(Y|X) = H(X,Y) - H(X)
    double conditional_entropy_yx() const {
        return joint_entropy() - row_entropy();
    }

    // Row entropy H(X)
    double row_entropy() const {
        double total = marg_.total();
        if (total < 1e-12) return 0.0;
        double h = 0.0;
        auto rows = api_.get_all_rows();
        for (auto r : rows) {
            double p = marg_.row_marginal(r) / total;
            if (p > 1e-12) h -= p * std::log2(p);
        }
        return h;
    }

    // Column entropy H(Y)
    double col_entropy() const {
        double total = marg_.total();
        if (total < 1e-12) return 0.0;
        double h = 0.0;
        auto cols = api_.get_all_cols();
        for (auto c : cols) {
            double p = marg_.col_marginal(c) / total;
            if (p > 1e-12) h -= p * std::log2(p);
        }
        return h;
    }

    // Joint entropy H(X,Y)
    double joint_entropy() const {
        double total = marg_.total();
        if (total < 1e-12) return 0.0;
        double h = 0.0;
        auto rows = api_.get_all_rows();
        for (auto r : rows) {
            auto pairs = api_.get_row(r);
            for (auto& p : pairs) {
                double prob = p.second / total;
                if (prob > 1e-12) h -= prob * std::log2(prob);
            }
        }
        return h;
    }

private:
    PairAPI& api_;
    Marginals& marg_;
};

// ─────────────────────────────────────────────────────────────────────────────
// MatrixTranspose — Transpose view of a matrix
// ─────────────────────────────────────────────────────────────────────────────
class MatrixTranspose {
public:
    MatrixTranspose(PairAPI& api, AtomSpace& as)
        : api_(api), transposed_(as, api.predicate_name() + "-T") {}

    // Compute the transpose
    void compute() {
        auto rows = api_.get_all_rows();
        for (auto r : rows) {
            auto pairs = api_.get_row(r);
            for (auto& p : pairs) {
                transposed_.set_pair(p.first, r, p.second);
            }
        }
    }

    PairAPI& transposed() { return transposed_; }

private:
    PairAPI& api_;
    PairAPI transposed_;
};

// ─────────────────────────────────────────────────────────────────────────────
// BatchSimilarity — Bulk-compute similarity scores
// ─────────────────────────────────────────────────────────────────────────────
class BatchSimilarity {
public:
    BatchSimilarity(PairAPI& api, AtomSpace& as)
        : api_(api), as_(as), sim_(api) {}

    struct SimilarityResult {
        Handle a;
        Handle b;
        double score;
    };

    // Compute cosine similarity for all row pairs above threshold
    std::vector<SimilarityResult> compute_all(double threshold = 0.1) {
        std::vector<SimilarityResult> results;
        auto rows = api_.get_all_rows();

        for (size_t i = 0; i < rows.size(); ++i) {
            for (size_t j = i + 1; j < rows.size(); ++j) {
                double score = sim_.cosine(rows[i], rows[j]);
                if (score >= threshold) {
                    results.push_back({rows[i], rows[j], score});
                    // Store in AtomSpace as SimilarityLink
                    as_.add_link(types::SIMILARITY_LINK,
                                 {rows[i], rows[j]},
                                 TruthValue(score, 0.9));
                }
            }
        }

        // Sort by score descending
        std::sort(results.begin(), results.end(),
                  [](const SimilarityResult& a, const SimilarityResult& b) {
                      return a.score > b.score;
                  });

        return results;
    }

    // Get top-k most similar to a given row
    std::vector<SimilarityResult> top_k(Handle row, size_t k) {
        std::vector<SimilarityResult> results;
        auto rows = api_.get_all_rows();

        for (auto r : rows) {
            if (r == row) continue;
            double score = sim_.cosine(row, r);
            results.push_back({row, r, score});
        }

        std::sort(results.begin(), results.end(),
                  [](const SimilarityResult& a, const SimilarityResult& b) {
                      return a.score > b.score;
                  });

        if (results.size() > k) results.resize(k);
        return results;
    }

private:
    PairAPI& api_;
    AtomSpace& as_;
    Similarity sim_;
};

} // namespace matrix
} // namespace oc

#endif // OC_MATRIX_HPP
