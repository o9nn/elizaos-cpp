// oc::attention — Header-only C++11 reimplementation of opencog/attention
// ECAN: AttentionBank, ImportanceDiffusion, HebbianUpdating, RentCollection, Forgetting
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_ATTENTION_HPP
#define OC_ATTENTION_HPP

#include "../atomspace/atomspace.hpp"

namespace oc {
namespace attention {

// ─────────────────────────────────────────────────────────────────────────────
// ImportanceIndex — Bin-based index for fast STI-range queries
// ─────────────────────────────────────────────────────────────────────────────
class ImportanceIndex {
public:
    static const int NUM_BINS = 256;

    ImportanceIndex() { bins_.resize(NUM_BINS); }

    void insert(Handle h, AttentionValue::sti_t sti) {
        int bin = sti_to_bin(sti);
        bins_[bin].insert(h);
        atom_sti_[h] = sti;
    }

    void remove(Handle h, AttentionValue::sti_t sti) {
        int bin = sti_to_bin(sti);
        bins_[bin].erase(h);
        atom_sti_.erase(h);
    }

    void update(Handle h, AttentionValue::sti_t old_sti,
                AttentionValue::sti_t new_sti) {
        int old_bin = sti_to_bin(old_sti);
        int new_bin = sti_to_bin(new_sti);
        if (old_bin != new_bin) {
            bins_[old_bin].erase(h);
            bins_[new_bin].insert(h);
        }
        atom_sti_[h] = new_sti;
    }

    // Get all handles with STI >= threshold
    HandleSeq get_above(AttentionValue::sti_t threshold) const {
        HandleSeq result;
        int bin = sti_to_bin(threshold);
        // Bins above are guaranteed above threshold
        for (int i = bin + 1; i < NUM_BINS; ++i) {
            result.insert(result.end(), bins_[i].begin(), bins_[i].end());
        }
        // For the boundary bin, check actual STI stored in atom_sti_
        for (auto h : bins_[bin]) {
            auto it = atom_sti_.find(h);
            if (it != atom_sti_.end() && it->second >= threshold) {
                result.push_back(h);
            }
        }
        return result;
    }

    // Get all handles with STI < threshold
    HandleSeq get_below(AttentionValue::sti_t threshold) const {
        HandleSeq result;
        int bin = sti_to_bin(threshold);
        // Bins below are guaranteed below threshold
        for (int i = 0; i < bin; ++i) {
            result.insert(result.end(), bins_[i].begin(), bins_[i].end());
        }
        // For the boundary bin, check actual STI
        if (bin < NUM_BINS) {
            for (auto h : bins_[bin]) {
                auto it = atom_sti_.find(h);
                if (it != atom_sti_.end() && it->second < threshold) {
                    result.push_back(h);
                }
            }
        }
        return result;
    }

    size_t total_count() const {
        size_t n = 0;
        for (auto& b : bins_) n += b.size();
        return n;
    }

private:
    std::vector<HandleSet> bins_;
    std::unordered_map<Handle, AttentionValue::sti_t> atom_sti_;

    static int sti_to_bin(AttentionValue::sti_t sti) {
        // Map int16_t [-32768, 32767] to [0, 255]
        int bin = (static_cast<int>(sti) + 32768) * (NUM_BINS - 1) / 65535;
        if (bin < 0) bin = 0;
        if (bin >= NUM_BINS) bin = NUM_BINS - 1;
        return bin;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// AttentionBank — Central manager for attention values
// ─────────────────────────────────────────────────────────────────────────────
class AttentionBank {
public:
    explicit AttentionBank(AtomSpace& as)
        : as_(as), af_threshold_(0), total_sti_(0),
          funds_(1000), wage_(10) {}

    // Get/set attentional focus threshold
    AttentionValue::sti_t get_af_threshold() const { return af_threshold_; }
    void set_af_threshold(AttentionValue::sti_t t) { af_threshold_ = t; }

    // Stimulus: increase STI of an atom
    void stimulate(Handle h, AttentionValue::sti_t amount) {
        Atom* a = as_.get_atom_mut(h);
        if (!a) return;
        AttentionValue::sti_t old_sti = a->av.sti;
        a->av.sti += amount;
        total_sti_ += amount;
        index_.update(h, old_sti, a->av.sti);
        av_changed_.emit(h, a->av);
    }

    // Set STI directly
    void set_sti(Handle h, AttentionValue::sti_t sti) {
        Atom* a = as_.get_atom_mut(h);
        if (!a) return;
        AttentionValue::sti_t old_sti = a->av.sti;
        total_sti_ += (sti - old_sti);
        a->av.sti = sti;
        index_.update(h, old_sti, sti);
        av_changed_.emit(h, a->av);
    }

    // Set LTI directly
    void set_lti(Handle h, AttentionValue::lti_t lti) {
        Atom* a = as_.get_atom_mut(h);
        if (!a) return;
        a->av.lti = lti;
        av_changed_.emit(h, a->av);
    }

    // Get atoms in attentional focus (STI >= threshold)
    HandleSeq get_attentional_focus() const {
        return index_.get_above(af_threshold_);
    }

    // Get atoms below attentional focus
    HandleSeq get_below_focus() const {
        return index_.get_below(af_threshold_);
    }

    int64_t get_total_sti() const { return total_sti_; }
    int64_t get_funds() const { return funds_; }
    void set_funds(int64_t f) { funds_ = f; }
    int64_t get_wage() const { return wage_; }
    void set_wage(int64_t w) { wage_ = w; }

    // Register atom with the bank
    void register_atom(Handle h) {
        const Atom* a = as_.get_atom(h);
        if (a) index_.insert(h, a->av.sti);
    }

    // Signal for AV changes
    util::SigSlot<Handle, AttentionValue>& av_changed_signal() {
        return av_changed_;
    }

private:
    AtomSpace& as_;
    AttentionValue::sti_t af_threshold_;
    int64_t total_sti_;
    int64_t funds_;
    int64_t wage_;
    ImportanceIndex index_;
    util::SigSlot<Handle, AttentionValue> av_changed_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Agent — Base class for ECAN agents (mirrors opencog::Agent)
// ─────────────────────────────────────────────────────────────────────────────
class Agent {
public:
    explicit Agent(AtomSpace& as, AttentionBank& bank)
        : as_(as), bank_(bank), frequency_(1), count_(0) {}
    virtual ~Agent() {}

    virtual void run() = 0;
    virtual std::string name() const = 0;

    int frequency() const { return frequency_; }
    void set_frequency(int f) { frequency_ = f; }
    uint64_t run_count() const { return count_; }

    void tick() {
        count_++;
        if (count_ % frequency_ == 0) run();
    }

protected:
    AtomSpace& as_;
    AttentionBank& bank_;
    int frequency_;
    uint64_t count_;
};

// ─────────────────────────────────────────────────────────────────────────────
// AFImportanceDiffusionAgent — Spread STI within attentional focus
// ─────────────────────────────────────────────────────────────────────────────
class AFImportanceDiffusionAgent : public Agent {
public:
    AFImportanceDiffusionAgent(AtomSpace& as, AttentionBank& bank,
                                double max_spread_pct = 0.5,
                                double decay = 0.05)
        : Agent(as, bank), max_spread_pct_(max_spread_pct), decay_(decay) {}

    std::string name() const override { return "AFImportanceDiffusionAgent"; }

    void run() override {
        auto focus = bank_.get_attentional_focus();
        for (auto h : focus) {
            diffuse(h, focus);
        }
    }

private:
    double max_spread_pct_;
    double decay_;

    void diffuse(Handle source, const HandleSeq& focus) {
        const Atom* a = as_.get_atom(source);
        if (!a) return;
        AttentionValue::sti_t sti = a->av.sti;
        if (sti <= 0) return;

        // Amount to spread
        int16_t spread_total = static_cast<int16_t>(sti * max_spread_pct_);
        if (spread_total <= 0) return;

        // Collect neighbors (outgoing + incoming)
        HandleSet neighbors;
        for (auto oh : a->outgoing) neighbors.insert(oh);
        for (auto ih : a->incoming) neighbors.insert(ih);
        if (neighbors.empty()) return;

        // Spread evenly among neighbors in focus
        HandleSeq focus_neighbors;
        HandleSet focus_set(focus.begin(), focus.end());
        for (auto nh : neighbors) {
            if (focus_set.count(nh)) focus_neighbors.push_back(nh);
        }
        if (focus_neighbors.empty()) return;

        int16_t per_neighbor = spread_total / static_cast<int16_t>(focus_neighbors.size());
        if (per_neighbor <= 0) per_neighbor = 1;

        int16_t actually_spread = 0;
        for (auto nh : focus_neighbors) {
            bank_.stimulate(nh, per_neighbor);
            actually_spread += per_neighbor;
        }
        // Deduct from source + decay
        int16_t deduct = actually_spread + static_cast<int16_t>(sti * decay_);
        bank_.set_sti(source, sti - deduct);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// WAImportanceDiffusionAgent — Spread STI across whole AtomSpace
// ─────────────────────────────────────────────────────────────────────────────
class WAImportanceDiffusionAgent : public Agent {
public:
    WAImportanceDiffusionAgent(AtomSpace& as, AttentionBank& bank,
                                double spread_pct = 0.1)
        : Agent(as, bank), spread_pct_(spread_pct) {}

    std::string name() const override { return "WAImportanceDiffusionAgent"; }

    void run() override {
        as_.foreach_atom([this](const Atom& a) {
            if (a.av.sti <= 0) return;
            int16_t spread = static_cast<int16_t>(a.av.sti * spread_pct_);
            if (spread <= 0) return;
            HandleSet neighbors;
            for (auto oh : a.outgoing) neighbors.insert(oh);
            for (auto ih : a.incoming) neighbors.insert(ih);
            if (neighbors.empty()) return;
            int16_t per = spread / static_cast<int16_t>(neighbors.size());
            if (per <= 0) per = 1;
            for (auto nh : neighbors) {
                bank_.stimulate(nh, per);
            }
            bank_.set_sti(a.handle, a.av.sti - spread);
        });
    }

private:
    double spread_pct_;
};

// ─────────────────────────────────────────────────────────────────────────────
// AFRentCollectionAgent — Collect rent from atoms in attentional focus
// ─────────────────────────────────────────────────────────────────────────────
class AFRentCollectionAgent : public Agent {
public:
    AFRentCollectionAgent(AtomSpace& as, AttentionBank& bank,
                           AttentionValue::sti_t rent = 2)
        : Agent(as, bank), rent_(rent) {}

    std::string name() const override { return "AFRentCollectionAgent"; }

    void run() override {
        auto focus = bank_.get_attentional_focus();
        for (auto h : focus) {
            const Atom* a = as_.get_atom(h);
            if (!a) continue;
            bank_.set_sti(h, a->av.sti - rent_);
        }
    }

private:
    AttentionValue::sti_t rent_;
};

// ─────────────────────────────────────────────────────────────────────────────
// WARentCollectionAgent — Collect rent from all atoms
// ─────────────────────────────────────────────────────────────────────────────
class WARentCollectionAgent : public Agent {
public:
    WARentCollectionAgent(AtomSpace& as, AttentionBank& bank,
                           AttentionValue::sti_t rent = 1)
        : Agent(as, bank), rent_(rent) {}

    std::string name() const override { return "WARentCollectionAgent"; }

    void run() override {
        as_.foreach_atom([this](const Atom& a) {
            if (a.av.sti > 0) {
                bank_.set_sti(a.handle, a.av.sti - rent_);
            }
        });
    }

private:
    AttentionValue::sti_t rent_;
};

// ─────────────────────────────────────────────────────────────────────────────
// ForgettingAgent — Remove atoms with very low STI
// ─────────────────────────────────────────────────────────────────────────────
class ForgettingAgent : public Agent {
public:
    ForgettingAgent(AtomSpace& as, AttentionBank& bank,
                     AttentionValue::sti_t threshold = -100,
                     size_t max_remove = 10)
        : Agent(as, bank), threshold_(threshold), max_remove_(max_remove) {}

    std::string name() const override { return "ForgettingAgent"; }

    void run() override {
        auto below = bank_.get_below_focus();
        size_t removed = 0;
        for (auto h : below) {
            if (removed >= max_remove_) break;
            const Atom* a = as_.get_atom(h);
            if (!a) continue;
            if (a->av.sti < threshold_ && a->av.vlti == 0) {
                as_.remove_atom(h, false);
                removed++;
            }
        }
        forgotten_count_ += removed;
    }

    size_t total_forgotten() const { return forgotten_count_; }

private:
    AttentionValue::sti_t threshold_;
    size_t max_remove_;
    size_t forgotten_count_ = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// HebbianUpdatingAgent — Update Hebbian link weights based on co-activation
// ─────────────────────────────────────────────────────────────────────────────
class HebbianUpdatingAgent : public Agent {
public:
    HebbianUpdatingAgent(AtomSpace& as, AttentionBank& bank,
                          double learning_rate = 0.1)
        : Agent(as, bank), lr_(learning_rate) {}

    std::string name() const override { return "HebbianUpdatingAgent"; }

    void run() override {
        auto heb_links = as_.get_by_type(types::HEBBIAN_LINK);
        for (auto lh : heb_links) {
            const Atom* link = as_.get_atom(lh);
            if (!link || link->outgoing.size() != 2) continue;
            const Atom* a = as_.get_atom(link->outgoing[0]);
            const Atom* b = as_.get_atom(link->outgoing[1]);
            if (!a || !b) continue;

            // Normalized STI in [0, 1]
            double na = normalize_sti(a->av.sti);
            double nb = normalize_sti(b->av.sti);

            // Hebbian update: Δw = lr * (co_activation - w)
            double co_act = na * nb;
            double old_w = link->tv.mean;
            double new_w = old_w + lr_ * (co_act - old_w);
            new_w = std::max(0.0, std::min(1.0, new_w));

            as_.set_tv(lh, TruthValue(new_w, link->tv.confidence));
        }
    }

private:
    double lr_;

    static double normalize_sti(AttentionValue::sti_t sti) {
        return (static_cast<double>(sti) + 32768.0) / 65535.0;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// HebbianCreationAgent — Create Hebbian links between co-active atoms
// ─────────────────────────────────────────────────────────────────────────────
class HebbianCreationAgent : public Agent {
public:
    HebbianCreationAgent(AtomSpace& as, AttentionBank& bank,
                          size_t max_links = 10)
        : Agent(as, bank), max_links_(max_links) {}

    std::string name() const override { return "HebbianCreationAgent"; }

    void run() override {
        auto focus = bank_.get_attentional_focus();
        size_t created = 0;
        for (size_t i = 0; i < focus.size() && created < max_links_; ++i) {
            for (size_t j = i + 1; j < focus.size() && created < max_links_; ++j) {
                // Check if Hebbian link already exists
                auto existing = as_.pattern_match(types::HEBBIAN_LINK,
                                                   {focus[i], focus[j]});
                if (existing.empty()) {
                    // Also check reverse order for unordered
                    existing = as_.pattern_match(types::HEBBIAN_LINK,
                                                  {focus[j], focus[i]});
                }
                if (existing.empty()) {
                    as_.add_link(types::HEBBIAN_LINK,
                                {focus[i], focus[j]},
                                TruthValue(0.5, 0.1));
                    created++;
                }
            }
        }
    }

private:
    size_t max_links_;
};

// ─────────────────────────────────────────────────────────────────────────────
// ECANRunner — Orchestrates all ECAN agents in a cognitive cycle
// ─────────────────────────────────────────────────────────────────────────────
class ECANRunner {
public:
    explicit ECANRunner(AtomSpace& as)
        : as_(as), bank_(as),
          af_diffusion_(as, bank_),
          wa_diffusion_(as, bank_),
          af_rent_(as, bank_),
          wa_rent_(as, bank_),
          forgetting_(as, bank_),
          hebbian_update_(as, bank_),
          hebbian_create_(as, bank_),
          cycle_count_(0) {}

    AttentionBank& bank() { return bank_; }

    void step() {
        cycle_count_++;
        af_diffusion_.tick();
        wa_diffusion_.tick();
        af_rent_.tick();
        wa_rent_.tick();
        hebbian_update_.tick();
        hebbian_create_.tick();
        forgetting_.tick();
    }

    void run(int cycles) {
        for (int i = 0; i < cycles; ++i) step();
    }

    uint64_t cycle_count() const { return cycle_count_; }

    // Access individual agents for configuration
    AFImportanceDiffusionAgent& af_diffusion() { return af_diffusion_; }
    WAImportanceDiffusionAgent& wa_diffusion() { return wa_diffusion_; }
    AFRentCollectionAgent& af_rent() { return af_rent_; }
    WARentCollectionAgent& wa_rent() { return wa_rent_; }
    ForgettingAgent& forgetting() { return forgetting_; }
    HebbianUpdatingAgent& hebbian_update() { return hebbian_update_; }
    HebbianCreationAgent& hebbian_create() { return hebbian_create_; }

private:
    AtomSpace& as_;
    AttentionBank bank_;
    AFImportanceDiffusionAgent af_diffusion_;
    WAImportanceDiffusionAgent wa_diffusion_;
    AFRentCollectionAgent af_rent_;
    WARentCollectionAgent wa_rent_;
    ForgettingAgent forgetting_;
    HebbianUpdatingAgent hebbian_update_;
    HebbianCreationAgent hebbian_create_;
    uint64_t cycle_count_;
};

} // namespace attention
} // namespace oc

#endif // OC_ATTENTION_HPP
