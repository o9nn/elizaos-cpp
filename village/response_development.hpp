#pragma once

#include "response_authenticity.hpp"

#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

namespace village::development {

struct EvaluatorFinding {
    std::string finding_id;
    std::string created_at;
    std::string generation_id;
    std::string generation_record_sha256;
    std::string reviewer_id;
    std::string reviewer_kind;
    std::map<std::string, double> metrics;
    std::string narrative;
    std::string narrative_sha256;
    std::string outcome;
    authenticity::ChainEvidence chain;

    nlohmann::json to_json() const;
};

struct DevelopmentProposal {
    struct ParameterDelta {
        std::string parameter;
        std::string operation;
        std::string before_artifact_sha256;
        std::string after_artifact_sha256;
        double before_number = 0.0;
        double after_number = 0.0;
        bool has_numeric_values = false;

        nlohmann::json to_json() const;
    };

    std::string proposal_id;
    std::string created_at;
    std::string proposer_id;
    std::string target;
    std::vector<std::string> finding_record_sha256s;
    std::vector<ParameterDelta> deltas;
    double maximum_change_fraction = 0.0;
    std::vector<std::string> required_tests;
    std::string rollback_artifact_sha256;
    std::string status = "proposed";
    authenticity::ChainEvidence chain;

    nlohmann::json to_json() const;
};

struct PromotionDecision {
    std::string decision_id;
    std::string created_at;
    std::string proposal_id;
    std::string proposal_record_sha256;
    std::string approved_by;
    bool tests_verified = false;
    std::string rollback_artifact_sha256;
    std::string decision;
    std::string reason;
    authenticity::ChainEvidence chain;

    nlohmann::json to_json() const;
};

class DevelopmentLedger {
public:
    DevelopmentLedger(std::filesystem::path root,
                      std::filesystem::path generation_evidence_root);

    bool append_finding(EvaluatorFinding& finding, std::string& error);
    bool append_proposal(DevelopmentProposal& proposal, std::string& error);
    bool append_promotion(PromotionDecision& decision, std::string& error);
    bool verify(std::string& error) const;

private:
    struct StreamState {
        explicit StreamState(std::filesystem::path value) : path(std::move(value)) {}

        std::filesystem::path path;
        std::uint64_t last_sequence = 0;
        std::string last_hash = authenticity::kZeroHash;
        std::unordered_set<std::string> ids;
        std::unordered_set<std::string> record_hashes;
    };

    bool load_stream(StreamState& stream, const std::string& id_field,
                     std::string& error);
    bool append_stream(StreamState& stream, const std::string& id_field,
                       nlohmann::json record, authenticity::ChainEvidence& chain,
                       std::string& error);
    bool verify_stream(const StreamState& stream, const std::string& id_field,
                       std::string& error) const;
    bool verify_generation_reference(const EvaluatorFinding& finding,
                                     std::string& error) const;

    std::filesystem::path root_;
    std::filesystem::path generation_evidence_root_;
    mutable std::mutex mutex_;
    StreamState findings_;
    StreamState proposals_;
    StreamState promotions_;
};

} // namespace village::development
