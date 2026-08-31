#include "response_development.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace village::development {
namespace {

const std::unordered_set<std::string> kMetricNames = {
    "attribution_integrity", "contextual_relevance", "contradiction",
    "repetition", "uncertainty_calibration", "action_validity", "role_coherence"
};

const std::unordered_set<std::string> kProposalTargets = {
    "role_prompt", "memory_selection", "sampling", "adapter", "evaluation_policy"
};

const std::unordered_set<std::string> kRequiredTests = {
    "provenance-completeness", "resident-isolation", "context-sensitivity",
    "historical-immutability", "failure-visibility", "rollback-verification"
};

const std::map<std::string, std::unordered_set<std::string>> kParametersByTarget = {
    {"role_prompt", {"role_prompt.artifact"}},
    {"memory_selection", {"memory.policy_artifact", "memory.window_entries"}},
    {"sampling", {"sampling.temperature", "sampling.min_p", "sampling.max_tokens"}},
    {"adapter", {"adapter.artifact"}},
    {"evaluation_policy", {"evaluation_policy.artifact"}}
};

bool append_private(const std::filesystem::path& path, const std::string& content,
                    std::string& error) {
#ifdef _WIN32
    std::ofstream stream(path, std::ios::binary | std::ios::app);
    if (!stream) { error = "cannot_open_development_journal"; return false; }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.flush();
    if (!stream) { error = "cannot_append_development_journal"; return false; }
    return true;
#else
    const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd < 0) { error = "cannot_open_development_journal"; return false; }
    std::size_t written = 0;
    while (written < content.size()) {
        const auto count = ::write(fd, content.data() + written, content.size() - written);
        if (count <= 0) {
            ::close(fd);
            error = "cannot_append_development_journal";
            return false;
        }
        written += static_cast<std::size_t>(count);
    }
    if (::fsync(fd) != 0) {
        ::close(fd);
        error = "cannot_fsync_development_journal";
        return false;
    }
    ::close(fd);
    return true;
#endif
}

bool valid_hex_id(const std::string& value, std::size_t size) {
    return value.size() == size &&
           std::all_of(value.begin(), value.end(), [](unsigned char c) {
               return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
           });
}

} // namespace

nlohmann::json EvaluatorFinding::to_json() const {
    return {
        {"schema_version", "cogverse.evaluator-finding.v1"},
        {"finding_id", finding_id}, {"created_at", created_at},
        {"generation_id", generation_id},
        {"generation_record_sha256", generation_record_sha256},
        {"reviewer_id", reviewer_id}, {"reviewer_kind", reviewer_kind},
        {"metrics", metrics}, {"narrative", narrative},
        {"narrative_sha256", narrative_sha256}, {"outcome", outcome},
        {"chain", {
            {"sequence", chain.sequence},
            {"previous_record_sha256", chain.previous_record_sha256},
            {"record_sha256", chain.record_sha256}
        }}
    };
}

nlohmann::json DevelopmentProposal::to_json() const {
    nlohmann::json serialized_deltas = nlohmann::json::array();
    for (const auto& delta : deltas) serialized_deltas.push_back(delta.to_json());
    return {
        {"schema_version", "cogverse.development-proposal.v1"},
        {"proposal_id", proposal_id}, {"created_at", created_at},
        {"proposer_id", proposer_id}, {"target", target},
        {"finding_record_sha256s", finding_record_sha256s},
        {"deltas", serialized_deltas},
        {"maximum_change_fraction", maximum_change_fraction},
        {"required_tests", required_tests},
        {"rollback_artifact_sha256", rollback_artifact_sha256},
        {"status", status},
        {"chain", {
            {"sequence", chain.sequence},
            {"previous_record_sha256", chain.previous_record_sha256},
            {"record_sha256", chain.record_sha256}
        }}
    };
}

nlohmann::json DevelopmentProposal::ParameterDelta::to_json() const {
    nlohmann::json record = {
        {"parameter", parameter}, {"operation", operation},
        {"before_artifact_sha256", before_artifact_sha256.empty()
            ? nlohmann::json(nullptr) : nlohmann::json(before_artifact_sha256)},
        {"after_artifact_sha256", after_artifact_sha256.empty()
            ? nlohmann::json(nullptr) : nlohmann::json(after_artifact_sha256)},
        {"has_numeric_values", has_numeric_values}
    };
    record["before_number"] = has_numeric_values ? nlohmann::json(before_number)
                                                  : nlohmann::json(nullptr);
    record["after_number"] = has_numeric_values ? nlohmann::json(after_number)
                                                 : nlohmann::json(nullptr);
    return record;
}

nlohmann::json PromotionDecision::to_json() const {
    return {
        {"schema_version", "cogverse.promotion-decision.v1"},
        {"decision_id", decision_id}, {"created_at", created_at},
        {"proposal_id", proposal_id},
        {"proposal_record_sha256", proposal_record_sha256},
        {"approved_by", approved_by}, {"tests_verified", tests_verified},
        {"rollback_artifact_sha256", rollback_artifact_sha256},
        {"decision", decision}, {"reason", reason},
        {"chain", {
            {"sequence", chain.sequence},
            {"previous_record_sha256", chain.previous_record_sha256},
            {"record_sha256", chain.record_sha256}
        }}
    };
}

DevelopmentLedger::DevelopmentLedger(std::filesystem::path root,
                                     std::filesystem::path generation_evidence_root)
    : root_(std::move(root)),
      generation_evidence_root_(std::move(generation_evidence_root)),
      findings_{root_ / "evaluator-findings.jsonl"},
      proposals_{root_ / "development-proposals.jsonl"},
      promotions_{root_ / "promotion-decisions.jsonl"} {
    std::error_code ec;
    std::filesystem::create_directories(root_, ec);
    if (ec) throw std::runtime_error("cannot_create_development_directory:" + ec.message());
#ifndef _WIN32
    ::chmod(root_.c_str(), S_IRWXU);
#endif
    std::string error;
    if (!load_stream(findings_, "finding_id", error) ||
        !load_stream(proposals_, "proposal_id", error) ||
        !load_stream(promotions_, "decision_id", error)) {
        throw std::runtime_error(error);
    }
}

bool DevelopmentLedger::load_stream(StreamState& stream, const std::string& id_field,
                                    std::string& error) {
    if (!std::filesystem::exists(stream.path)) return true;
    std::ifstream input(stream.path);
    if (!input) { error = "cannot_read_development_journal"; return false; }
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        try {
            const auto record = nlohmann::json::parse(line);
            const auto id = record.at(id_field).get<std::string>();
            const auto hash = record.at("chain").at("record_sha256").get<std::string>();
            if (!stream.ids.insert(id).second || !stream.record_hashes.insert(hash).second)
                throw std::runtime_error("duplicate_development_record");
            stream.last_sequence = record.at("chain").at("sequence").get<std::uint64_t>();
            stream.last_hash = hash;
        } catch (const std::exception& exception) {
            error = std::string("invalid_development_journal:") + exception.what();
            return false;
        }
    }
    return true;
}

bool DevelopmentLedger::append_stream(StreamState& stream, const std::string& id_field,
                                      nlohmann::json record,
                                      authenticity::ChainEvidence& chain,
                                      std::string& error) {
    const auto id = record.at(id_field).get<std::string>();
    if (stream.ids.count(id) != 0) { error = "duplicate_development_id"; return false; }
    chain.sequence = stream.last_sequence + 1;
    chain.previous_record_sha256 = stream.last_hash;
    chain.record_sha256.clear();
    record["chain"] = {
        {"sequence", chain.sequence},
        {"previous_record_sha256", chain.previous_record_sha256},
        {"record_sha256", ""}
    };
    chain.record_sha256 = authenticity::sha256(authenticity::canonical_json(record));
    record["chain"]["record_sha256"] = chain.record_sha256;
    if (!append_private(stream.path, authenticity::canonical_json(record) + "\n", error))
        return false;
    stream.last_sequence = chain.sequence;
    stream.last_hash = chain.record_sha256;
    stream.ids.insert(id);
    stream.record_hashes.insert(chain.record_sha256);
    return true;
}

bool DevelopmentLedger::verify_generation_reference(const EvaluatorFinding& finding,
                                                    std::string& error) const {
    const auto path = generation_evidence_root_ / "attempts" /
                      (finding.generation_id + ".json");
    std::ifstream input(path);
    if (!input) { error = "generation_evidence_not_found"; return false; }
    try {
        nlohmann::json record;
        input >> record;
        if (record.at("generation_id").get<std::string>() != finding.generation_id ||
            record.at("chain").at("record_sha256").get<std::string>() !=
                finding.generation_record_sha256) {
            error = "generation_evidence_reference_mismatch";
            return false;
        }
    } catch (const std::exception& exception) {
        error = std::string("invalid_generation_evidence:") + exception.what();
        return false;
    }
    return true;
}

bool DevelopmentLedger::append_finding(EvaluatorFinding& finding, std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!valid_hex_id(finding.finding_id, 32) || finding.created_at.empty() ||
        finding.reviewer_id.empty() || finding.reviewer_kind.empty() ||
        finding.outcome.empty()) {
        error = "invalid_evaluator_finding_identity";
        return false;
    }
    if (authenticity::sha256(finding.narrative) != finding.narrative_sha256) {
        error = "evaluator_narrative_hash_mismatch";
        return false;
    }
    for (const auto& [name, value] : finding.metrics) {
        if (kMetricNames.count(name) == 0 || value < 0.0 || value > 1.0) {
            error = "invalid_evaluator_metric";
            return false;
        }
    }
    if (!verify_generation_reference(finding, error)) return false;
    return append_stream(findings_, "finding_id", finding.to_json(), finding.chain, error);
}

bool DevelopmentLedger::append_proposal(DevelopmentProposal& proposal, std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!valid_hex_id(proposal.proposal_id, 32) || proposal.created_at.empty() ||
        proposal.proposer_id.empty() || kProposalTargets.count(proposal.target) == 0 ||
        proposal.status != "proposed" || proposal.finding_record_sha256s.empty() ||
        proposal.deltas.empty() || proposal.required_tests.empty() ||
        !authenticity::is_sha256(proposal.rollback_artifact_sha256) ||
        proposal.maximum_change_fraction <= 0.0 || proposal.maximum_change_fraction > 0.2) {
        error = "invalid_or_unbounded_development_proposal";
        return false;
    }
    for (const auto& test : proposal.required_tests) {
        if (kRequiredTests.count(test) == 0) {
            error = "unknown_required_test";
            return false;
        }
    }
    for (const auto& finding_hash : proposal.finding_record_sha256s) {
        if (findings_.record_hashes.count(finding_hash) == 0) {
            error = "proposal_references_unreviewed_evidence";
            return false;
        }
    }
    const auto& allowed_parameters = kParametersByTarget.at(proposal.target);
    for (const auto& delta : proposal.deltas) {
        if (allowed_parameters.count(delta.parameter) == 0) {
            error = "forbidden_or_unknown_parameter";
            return false;
        }
        if (delta.has_numeric_values) {
            const bool numeric_parameter =
                delta.parameter.rfind("sampling.", 0) == 0 ||
                delta.parameter == "memory.window_entries";
            if (delta.operation != "set-number" || !numeric_parameter) {
                error = "invalid_numeric_delta";
                return false;
            }
        } else if (delta.operation != "replace-artifact" ||
                   !authenticity::is_sha256(delta.before_artifact_sha256) ||
                   !authenticity::is_sha256(delta.after_artifact_sha256)) {
            error = "invalid_artifact_delta";
            return false;
        }
    }
    return append_stream(proposals_, "proposal_id", proposal.to_json(), proposal.chain, error);
}

bool DevelopmentLedger::append_promotion(PromotionDecision& decision, std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!valid_hex_id(decision.decision_id, 32) || decision.created_at.empty() ||
        decision.proposal_id.empty() || decision.approved_by.empty() ||
        (decision.decision != "promoted" && decision.decision != "rejected") ||
        proposals_.record_hashes.count(decision.proposal_record_sha256) == 0) {
        error = "invalid_promotion_decision";
        return false;
    }
    if (decision.decision == "promoted" &&
        (!decision.tests_verified ||
         !authenticity::is_sha256(decision.rollback_artifact_sha256))) {
        error = "promotion_gate_not_satisfied";
        return false;
    }
    return append_stream(promotions_, "decision_id", decision.to_json(), decision.chain, error);
}

bool DevelopmentLedger::verify_stream(const StreamState& stream,
                                      const std::string& id_field,
                                      std::string& error) const {
    if (!std::filesystem::exists(stream.path)) return true;
    std::ifstream input(stream.path);
    if (!input) { error = "cannot_verify_development_journal"; return false; }
    std::uint64_t expected_sequence = 1;
    std::string expected_previous = authenticity::kZeroHash;
    std::unordered_set<std::string> ids;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        try {
            auto record = nlohmann::json::parse(line);
            const auto id = record.at(id_field).get<std::string>();
            const auto sequence = record.at("chain").at("sequence").get<std::uint64_t>();
            const auto previous = record.at("chain").at("previous_record_sha256").get<std::string>();
            const auto stored = record.at("chain").at("record_sha256").get<std::string>();
            record["chain"]["record_sha256"] = "";
            if (!ids.insert(id).second || sequence != expected_sequence ||
                previous != expected_previous ||
                authenticity::sha256(authenticity::canonical_json(record)) != stored) {
                error = "development_chain_mismatch_at_sequence:" +
                        std::to_string(expected_sequence);
                return false;
            }
            expected_previous = stored;
            ++expected_sequence;
        } catch (const std::exception& exception) {
            error = std::string("invalid_development_record:") + exception.what();
            return false;
        }
    }
    return true;
}

bool DevelopmentLedger::verify(std::string& error) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return verify_stream(findings_, "finding_id", error) &&
           verify_stream(proposals_, "proposal_id", error) &&
           verify_stream(promotions_, "decision_id", error);
}

} // namespace village::development
