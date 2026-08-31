#pragma once

#include <cstdint>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

namespace village::authenticity {

inline constexpr const char* kSchemaVersion = "cogverse.generation-provenance.v1";
inline constexpr const char* kValidatorVersion = "cogverse.authenticity-validator.v1";
inline constexpr const char* kZeroHash =
    "0000000000000000000000000000000000000000000000000000000000000000";

std::string sha256(const std::string& value);
std::string utc_now_iso8601();
std::string random_generation_id();
int random_seed();
bool is_sha256(const std::string& value);
std::string canonical_json(const nlohmann::json& value);

struct ResidentBinding {
    std::string id;
    std::string identity_version;
    std::string identity_manifest;
    std::string identity_manifest_sha256;
    std::string role_prompt;
    std::string role_prompt_sha256;
};

struct ContextBinding {
    std::string system_prompt;
    std::string system_prompt_sha256;
    std::string memory_snapshot;
    std::string memory_snapshot_sha256;
    std::string shared_context;
    std::string shared_context_sha256;
    std::string cognitive_state;
    std::string cognitive_state_sha256;
    std::string combined_context_sha256;
    std::string selection_record;
    std::string selection_record_sha256;
};

struct StimulusBinding {
    std::string source;
    std::string target;
    std::string text;
    std::string utf8_sha256;
    std::int64_t parent_event_id = 0;
    std::int64_t parent_tic = 0;
};

struct InvocationEvidence {
    std::string started_at;
    std::string server;
    std::string model_id;
    std::string model_revision;
    double temperature = 0.0;
    double min_p = 0.0;
    int max_tokens = 0;
    int seed = 0;
    std::string seed_sha256;
    std::string request_body;
    std::string request_body_sha256;
    std::string adapter_id;
    std::string adapter_sha256;
};

struct ResultEvidence {
    std::string status = "attempt-created";
    std::string finished_at;
    std::int64_t latency_ms = 0;
    int http_status = 0;
    int transport_code = 0;
    std::string finish_reason;
    int prompt_tokens = 0;
    int completion_tokens = 0;
    std::string raw_response;
    std::string raw_response_sha256;
    std::string raw_completion;
    std::string raw_completion_sha256;
    std::string normalized_completion;
    std::string normalized_completion_sha256;
    std::uint64_t exact_completion_prior_count = 0;
    bool exact_completion_duplicate = false;
    std::vector<std::string> normalization;
    std::string error_code;
    std::string error_detail;
};

struct PublicationDecision {
    bool allowed = false;
    std::string validator_version = kValidatorVersion;
    std::string denial_reason;
};

struct DevelopmentDisposition {
    bool eligible = true;
    bool history_immutable = true;
    bool automatic_identity_mutation_allowed = false;
    std::string review_state = "unreviewed";
    std::string outcome;
};

struct ChainEvidence {
    std::uint64_t sequence = 0;
    std::string previous_record_sha256 = kZeroHash;
    std::string record_sha256;
};

struct GenerationEvidence {
    std::string schema_version = kSchemaVersion;
    std::string generation_id;
    std::string created_at;
    std::string correlation_id;
    ResidentBinding resident;
    ContextBinding context;
    StimulusBinding stimulus;
    InvocationEvidence invocation;
    ResultEvidence result;
    PublicationDecision publication;
    DevelopmentDisposition development;
    ChainEvidence chain;

    nlohmann::json to_private_json() const;
    nlohmann::json to_public_json() const;
};

struct ValidationFinding {
    bool ok = false;
    std::string code;
    std::string detail;
};

ValidationFinding validate_for_publication(const GenerationEvidence& evidence);
std::string normalize_completion(const std::string& raw, std::vector<std::string>& operations);

class ProvenanceJournal {
public:
    explicit ProvenanceJournal(std::filesystem::path root);

    bool commit(GenerationEvidence& evidence, std::string& error);
    bool verify(std::string& error) const;
    const std::filesystem::path& root() const { return root_; }

private:
    bool load_tail(std::string& error);

    std::filesystem::path root_;
    std::filesystem::path attempts_dir_;
    std::filesystem::path journal_path_;
    mutable std::mutex mutex_;
    std::uint64_t last_sequence_ = 0;
    std::string last_hash_ = kZeroHash;
    std::unordered_set<std::string> generation_ids_;
    std::unordered_set<std::string> seed_hashes_;
    std::unordered_map<std::string, std::uint64_t> completion_hash_counts_;
};

} // namespace village::authenticity
