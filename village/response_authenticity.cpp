#include "response_authenticity.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <openssl/evp.h>
#include <openssl/rand.h>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace village::authenticity {
namespace {

std::string to_hex(const unsigned char* data, std::size_t size) {
    static constexpr char digits[] = "0123456789abcdef";
    std::string output(size * 2, '0');
    for (std::size_t i = 0; i < size; ++i) {
        output[i * 2] = digits[(data[i] >> 4U) & 0x0FU];
        output[i * 2 + 1] = digits[data[i] & 0x0FU];
    }
    return output;
}

bool atomic_write_private(const std::filesystem::path& path, const std::string& content,
                          std::string& error) {
    const auto temporary = path.string() + ".tmp";
#ifdef _WIN32
    std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
    if (!stream) {
        error = "cannot_open_temporary_artifact";
        return false;
    }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.flush();
    if (!stream) {
        error = "cannot_write_temporary_artifact";
        return false;
    }
    stream.close();
#else
    const int fd = ::open(temporary.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        error = "cannot_open_temporary_artifact";
        return false;
    }
    std::size_t written = 0;
    while (written < content.size()) {
        const auto count = ::write(fd, content.data() + written, content.size() - written);
        if (count <= 0) {
            ::close(fd);
            error = "cannot_write_temporary_artifact";
            return false;
        }
        written += static_cast<std::size_t>(count);
    }
    if (::fsync(fd) != 0) {
        ::close(fd);
        error = "cannot_fsync_temporary_artifact";
        return false;
    }
    ::close(fd);
#endif
    std::error_code ec;
    std::filesystem::rename(temporary, path, ec);
    if (ec) {
        std::filesystem::remove(temporary);
        error = "cannot_commit_artifact:" + ec.message();
        return false;
    }
    return true;
}

bool append_private(const std::filesystem::path& path, const std::string& content,
                    std::string& error) {
#ifdef _WIN32
    std::ofstream stream(path, std::ios::binary | std::ios::app);
    if (!stream) {
        error = "cannot_open_journal";
        return false;
    }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.flush();
    if (!stream) {
        error = "cannot_append_journal";
        return false;
    }
    return true;
#else
    const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        error = "cannot_open_journal";
        return false;
    }
    std::size_t written = 0;
    while (written < content.size()) {
        const auto count = ::write(fd, content.data() + written, content.size() - written);
        if (count <= 0) {
            ::close(fd);
            error = "cannot_append_journal";
            return false;
        }
        written += static_cast<std::size_t>(count);
    }
    if (::fsync(fd) != 0) {
        ::close(fd);
        error = "cannot_fsync_journal";
        return false;
    }
    ::close(fd);
    return true;
#endif
}

nlohmann::json record_without_hash(const GenerationEvidence& evidence) {
    auto record = evidence.to_private_json();
    record["chain"]["record_sha256"] = "";
    return record;
}

ValidationFinding invalid(const std::string& code, const std::string& detail) {
    return {false, code, detail};
}

} // namespace

std::string sha256(const std::string& value) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) throw std::runtime_error("sha256_context_allocation_failed");
    std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
    unsigned int digest_length = 0;
    const bool ok = EVP_DigestInit_ex(context, EVP_sha256(), nullptr) == 1 &&
                    EVP_DigestUpdate(context, value.data(), value.size()) == 1 &&
                    EVP_DigestFinal_ex(context, digest.data(), &digest_length) == 1;
    EVP_MD_CTX_free(context);
    if (!ok) throw std::runtime_error("sha256_digest_failed");
    return to_hex(digest.data(), digest_length);
}

std::string utc_now_iso8601() {
    const auto now = std::chrono::system_clock::now();
    const auto seconds = std::chrono::system_clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &seconds);
#else
    gmtime_r(&seconds, &utc);
#endif
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::ostringstream stream;
    stream << std::put_time(&utc, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3)
           << std::setfill('0') << millis.count() << 'Z';
    return stream.str();
}

std::string random_generation_id() {
    std::array<unsigned char, 16> bytes{};
    if (RAND_bytes(bytes.data(), static_cast<int>(bytes.size())) != 1) {
        throw std::runtime_error("generation_id_entropy_failed");
    }
    return to_hex(bytes.data(), bytes.size());
}

int random_seed() {
    std::uint32_t value = 0;
    if (RAND_bytes(reinterpret_cast<unsigned char*>(&value), sizeof(value)) != 1) {
        throw std::runtime_error("generation_seed_entropy_failed");
    }
    return static_cast<int>((value & 0x7fffffffU) | 1U);
}

bool is_sha256(const std::string& value) {
    return value.size() == 64 && std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    });
}

std::string canonical_json(const nlohmann::json& value) {
    return value.dump();
}

nlohmann::json GenerationEvidence::to_private_json() const {
    return {
        {"schema_version", schema_version},
        {"generation_id", generation_id},
        {"created_at", created_at},
        {"correlation_id", correlation_id},
        {"resident", {
            {"id", resident.id},
            {"identity_version", resident.identity_version},
            {"identity_manifest", resident.identity_manifest},
            {"identity_manifest_sha256", resident.identity_manifest_sha256},
            {"role_prompt", resident.role_prompt},
            {"role_prompt_sha256", resident.role_prompt_sha256}
        }},
        {"context", {
            {"system_prompt", context.system_prompt},
            {"system_prompt_sha256", context.system_prompt_sha256},
            {"memory_snapshot", context.memory_snapshot},
            {"memory_snapshot_sha256", context.memory_snapshot_sha256},
            {"shared_context", context.shared_context},
            {"shared_context_sha256", context.shared_context_sha256},
            {"cognitive_state", context.cognitive_state},
            {"cognitive_state_sha256", context.cognitive_state_sha256},
            {"combined_context_sha256", context.combined_context_sha256},
            {"selection_record", context.selection_record},
            {"selection_record_sha256", context.selection_record_sha256}
        }},
        {"stimulus", {
            {"source", stimulus.source}, {"target", stimulus.target},
            {"text", stimulus.text}, {"utf8_sha256", stimulus.utf8_sha256},
            {"parent_event_id", stimulus.parent_event_id},
            {"parent_tic", stimulus.parent_tic}
        }},
        {"invocation", {
            {"started_at", invocation.started_at}, {"server", invocation.server},
            {"model_id", invocation.model_id}, {"model_revision", invocation.model_revision},
            {"temperature", invocation.temperature}, {"min_p", invocation.min_p},
            {"max_tokens", invocation.max_tokens}, {"seed", invocation.seed},
            {"seed_sha256", invocation.seed_sha256},
            {"request_body", invocation.request_body},
            {"request_body_sha256", invocation.request_body_sha256},
            {"adapter_id", invocation.adapter_id.empty() ? nlohmann::json(nullptr) : nlohmann::json(invocation.adapter_id)},
            {"adapter_sha256", invocation.adapter_sha256.empty() ? nlohmann::json(nullptr) : nlohmann::json(invocation.adapter_sha256)}
        }},
        {"result", {
            {"status", result.status}, {"finished_at", result.finished_at},
            {"latency_ms", result.latency_ms}, {"http_status", result.http_status},
            {"transport_code", result.transport_code}, {"finish_reason", result.finish_reason},
            {"prompt_tokens", result.prompt_tokens}, {"completion_tokens", result.completion_tokens},
            {"raw_response", result.raw_response}, {"raw_response_sha256", result.raw_response_sha256},
            {"raw_completion", result.raw_completion}, {"raw_completion_sha256", result.raw_completion_sha256},
            {"normalized_completion", result.normalized_completion},
            {"normalized_completion_sha256", result.normalized_completion_sha256},
            {"exact_completion_prior_count", result.exact_completion_prior_count},
            {"exact_completion_duplicate", result.exact_completion_duplicate},
            {"normalization", result.normalization},
            {"error_code", result.error_code}, {"error_detail", result.error_detail}
        }},
        {"publication", {
            {"allowed", publication.allowed},
            {"validator_version", publication.validator_version},
            {"denial_reason", publication.denial_reason}
        }},
        {"development", {
            {"eligible", development.eligible},
            {"history_immutable", development.history_immutable},
            {"automatic_identity_mutation_allowed", development.automatic_identity_mutation_allowed},
            {"review_state", development.review_state},
            {"outcome", development.outcome}
        }},
        {"chain", {
            {"sequence", chain.sequence},
            {"previous_record_sha256", chain.previous_record_sha256},
            {"record_sha256", chain.record_sha256}
        }}
    };
}

nlohmann::json GenerationEvidence::to_public_json() const {
    return {
        {"schema_version", schema_version},
        {"generation_id", generation_id},
        {"correlation_id", correlation_id},
        {"resident", resident.id},
        {"identity_version", resident.identity_version},
        {"identity_manifest_sha256", resident.identity_manifest_sha256},
        {"role_prompt_sha256", resident.role_prompt_sha256},
        {"system_prompt_sha256", context.system_prompt_sha256},
        {"memory_snapshot_sha256", context.memory_snapshot_sha256},
        {"shared_context_sha256", context.shared_context_sha256},
        {"cognitive_state_sha256", context.cognitive_state_sha256},
        {"combined_context_sha256", context.combined_context_sha256},
        {"selection_record_sha256", context.selection_record_sha256},
        {"stimulus_sha256", stimulus.utf8_sha256},
        {"model_id", invocation.model_id},
        {"model_revision", invocation.model_revision},
        {"temperature", invocation.temperature},
        {"min_p", invocation.min_p},
        {"max_tokens", invocation.max_tokens},
        {"seed_sha256", invocation.seed_sha256},
        {"request_body_sha256", invocation.request_body_sha256},
        {"adapter_id", invocation.adapter_id.empty() ? nlohmann::json(nullptr) : nlohmann::json(invocation.adapter_id)},
        {"adapter_sha256", invocation.adapter_sha256.empty() ? nlohmann::json(nullptr) : nlohmann::json(invocation.adapter_sha256)},
        {"generation_status", result.status},
        {"latency_ms", result.latency_ms},
        {"http_status", result.http_status},
        {"finish_reason", result.finish_reason},
        {"prompt_tokens", result.prompt_tokens},
        {"completion_tokens", result.completion_tokens},
        {"raw_response_sha256", result.raw_response_sha256},
        {"raw_completion_sha256", result.raw_completion_sha256},
        {"normalized_completion_sha256", result.normalized_completion_sha256},
        {"exact_completion_prior_count", result.exact_completion_prior_count},
        {"exact_completion_duplicate", result.exact_completion_duplicate},
        {"normalization", result.normalization},
        {"error_code", result.error_code},
        {"error_detail", result.error_detail},
        {"publication_allowed", publication.allowed},
        {"validator_version", publication.validator_version},
        {"denial_reason", publication.denial_reason},
        {"development_eligible", development.eligible},
        {"development_review_state", development.review_state},
        {"history_immutable", development.history_immutable},
        {"automatic_identity_mutation_allowed", development.automatic_identity_mutation_allowed},
        {"journal_sequence", chain.sequence},
        {"journal_record_sha256", chain.record_sha256}
    };
}

ValidationFinding validate_for_publication(const GenerationEvidence& evidence) {
    if (evidence.schema_version != kSchemaVersion) return invalid("schema_version", "unexpected provenance schema");
    if (evidence.generation_id.size() != 32 ||
        !std::all_of(evidence.generation_id.begin(), evidence.generation_id.end(),
                     [](unsigned char c) {
                         return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
                     }))
        return invalid("generation_id", "generation ID is not 128-bit lowercase hex");
    if (evidence.resident.id.empty()) return invalid("resident_missing", "resident identity is empty");
    if (evidence.resident.identity_version.empty() ||
        evidence.resident.identity_version == "unconfigured")
        return invalid("identity_version_missing", "identity version is not configured");
    if (evidence.resident.role_prompt.empty()) return invalid("role_prompt_missing", "role prompt is empty");
    if (sha256(evidence.resident.identity_manifest) != evidence.resident.identity_manifest_sha256)
        return invalid("identity_hash_mismatch", "identity manifest commitment does not match");
    if (sha256(evidence.resident.role_prompt) != evidence.resident.role_prompt_sha256)
        return invalid("role_hash_mismatch", "role prompt commitment does not match");
    if (sha256(evidence.context.system_prompt) != evidence.context.system_prompt_sha256)
        return invalid("system_prompt_hash_mismatch", "system prompt commitment does not match");
    if (sha256(evidence.context.memory_snapshot) != evidence.context.memory_snapshot_sha256)
        return invalid("memory_hash_mismatch", "memory snapshot commitment does not match");
    if (sha256(evidence.context.shared_context) != evidence.context.shared_context_sha256)
        return invalid("shared_context_hash_mismatch", "shared context commitment does not match");
    if (sha256(evidence.context.cognitive_state) != evidence.context.cognitive_state_sha256)
        return invalid("cognitive_state_hash_mismatch", "cognitive state commitment does not match");
    const std::string combined = evidence.context.system_prompt + "\n--MEMORY--\n" +
                                 evidence.context.memory_snapshot + "\n--SHARED--\n" +
                                 evidence.context.shared_context + "\n--STATE--\n" +
                                 evidence.context.cognitive_state;
    if (sha256(combined) != evidence.context.combined_context_sha256)
        return invalid("combined_context_hash_mismatch", "combined context commitment does not match");
    if (sha256(evidence.context.selection_record) != evidence.context.selection_record_sha256)
        return invalid("selection_hash_mismatch", "context selection commitment does not match");
    if (evidence.stimulus.target != evidence.resident.id)
        return invalid("stimulus_target_mismatch", "stimulus target does not match resident");
    if (evidence.stimulus.source.empty() || evidence.stimulus.text.empty())
        return invalid("stimulus_missing", "stimulus source or text is empty");
    if (sha256(evidence.stimulus.text) != evidence.stimulus.utf8_sha256)
        return invalid("stimulus_hash_mismatch", "stimulus commitment does not match");
    if (evidence.invocation.started_at.empty() || evidence.invocation.server.empty() ||
        evidence.invocation.model_id.empty() || evidence.invocation.model_revision.empty() ||
        evidence.invocation.model_revision == "unconfigured")
        return invalid("model_identity_missing", "server, model, or model revision is missing");
    if (evidence.invocation.temperature <= 0.0)
        return invalid("deterministic_temperature", "temperature must be greater than zero");
    if (evidence.invocation.seed <= 0)
        return invalid("seed_missing", "non-reused request seed is missing");
    if (sha256(std::to_string(evidence.invocation.seed)) != evidence.invocation.seed_sha256)
        return invalid("seed_hash_mismatch", "seed commitment does not match");
    if (sha256(evidence.invocation.request_body) != evidence.invocation.request_body_sha256)
        return invalid("request_hash_mismatch", "model request commitment does not match");
    try {
        const auto request = nlohmann::json::parse(evidence.invocation.request_body);
        if (request.value("model", "") != evidence.invocation.model_id ||
            request.value("seed", 0) != evidence.invocation.seed ||
            request.value("temperature", 0.0) != evidence.invocation.temperature ||
            request.value("min_p", -1.0) != evidence.invocation.min_p ||
            request.value("max_tokens", 0) != evidence.invocation.max_tokens)
            return invalid("request_parameter_mismatch", "recorded invocation differs from request body");
        const auto& messages = request.at("messages");
        if (!messages.is_array() || messages.size() != 2 ||
            messages.at(0).value("role", "") != "system" ||
            messages.at(0).value("content", "") != evidence.context.system_prompt ||
            messages.at(1).value("role", "") != "user" ||
            messages.at(1).value("content", "") != evidence.stimulus.text)
            return invalid("request_context_mismatch", "request messages differ from bound context or stimulus");
    } catch (const std::exception& exception) {
        return invalid("request_parse_error", exception.what());
    }
    if (evidence.result.status != "generated-unverified")
        return invalid("result_state", "only a generated-unverified result can enter publication validation");
    if (evidence.result.http_status != 200)
        return invalid("http_status", "model server did not return HTTP 200");
    if (evidence.result.raw_response.empty() || evidence.result.raw_completion.empty() ||
        evidence.result.normalized_completion.empty())
        return invalid("empty_generation", "model response or completion is empty");
    if (sha256(evidence.result.raw_response) != evidence.result.raw_response_sha256)
        return invalid("raw_response_hash_mismatch", "raw response commitment does not match");
    if (sha256(evidence.result.raw_completion) != evidence.result.raw_completion_sha256)
        return invalid("raw_completion_hash_mismatch", "raw completion commitment does not match");
    if (sha256(evidence.result.normalized_completion) != evidence.result.normalized_completion_sha256)
        return invalid("normalized_completion_hash_mismatch", "normalized completion commitment does not match");
    return {true, "ok", "all authenticity bindings validate"};
}

std::string normalize_completion(const std::string& raw, std::vector<std::string>& operations) {
    std::string output = raw;
    bool removed_framing = false;
    const std::string start_token = "<|im_start|>assistant";
    if (output.rfind(start_token, 0) == 0) {
        output.erase(0, start_token.size());
        if (!output.empty() && output.front() == '\n') output.erase(0, 1);
        removed_framing = true;
    }
    const std::string end_token = "<|im_end|>";
    if (output.size() >= end_token.size() &&
        output.compare(output.size() - end_token.size(), end_token.size(), end_token) == 0) {
        output.erase(output.size() - end_token.size());
        removed_framing = true;
    }
    if (removed_framing) operations.push_back("remove_terminal_chat_framing");
    const auto original_size = output.size();
    while (!output.empty() && (output.back() == '\n' || output.back() == '\r' ||
                               output.back() == ' ' || output.back() == '\t')) {
        output.pop_back();
    }
    if (output.size() != original_size) operations.push_back("trim_terminal_whitespace");
    return output;
}

ProvenanceJournal::ProvenanceJournal(std::filesystem::path root)
    : root_(std::move(root)), attempts_dir_(root_ / "attempts"),
      journal_path_(root_ / "journal.jsonl") {
    std::error_code ec;
    std::filesystem::create_directories(attempts_dir_, ec);
    if (ec) throw std::runtime_error("cannot_create_provenance_directory:" + ec.message());
#ifndef _WIN32
    ::chmod(root_.c_str(), S_IRWXU);
    ::chmod(attempts_dir_.c_str(), S_IRWXU);
#endif
    std::string error;
    if (!load_tail(error)) throw std::runtime_error(error);
}

bool ProvenanceJournal::load_tail(std::string& error) {
    if (!std::filesystem::exists(journal_path_)) return true;
    std::ifstream stream(journal_path_);
    if (!stream) {
        error = "cannot_read_existing_journal";
        return false;
    }
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        try {
            const auto record = nlohmann::json::parse(line);
            last_sequence_ = record.at("chain").at("sequence").get<std::uint64_t>();
            last_hash_ = record.at("chain").at("record_sha256").get<std::string>();
            const auto generation_id = record.at("generation_id").get<std::string>();
            const auto seed_sha256 = record.at("invocation").at("seed_sha256").get<std::string>();
            if (!generation_ids_.insert(generation_id).second)
                throw std::runtime_error("duplicate_generation_id");
            if (!seed_hashes_.insert(seed_sha256).second)
                throw std::runtime_error("reused_generation_seed");
            if (record.at("result").value("status", "") == "generated-authentic") {
                const auto completion_hash = record.at("result").value(
                    "normalized_completion_sha256", "");
                if (is_sha256(completion_hash)) ++completion_hash_counts_[completion_hash];
            }
        } catch (const std::exception& exception) {
            error = std::string("invalid_existing_journal:") + exception.what();
            return false;
        }
    }
    return true;
}

bool ProvenanceJournal::commit(GenerationEvidence& evidence, std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (generation_ids_.count(evidence.generation_id) != 0) {
        error = "duplicate_generation_id";
        return false;
    }
    if (!evidence.invocation.seed_sha256.empty() &&
        seed_hashes_.count(evidence.invocation.seed_sha256) != 0) {
        error = "reused_generation_seed";
        return false;
    }
    if (evidence.result.status == "generated-unverified") {
        const auto finding = validate_for_publication(evidence);
        if (finding.ok) {
            evidence.result.status = "generated-authentic";
            evidence.publication.allowed = true;
            evidence.publication.denial_reason.clear();
        } else {
            evidence.result.status = "provenance-rejected";
            evidence.result.error_code = finding.code;
            evidence.result.error_detail = finding.detail;
            evidence.publication.allowed = false;
            evidence.publication.denial_reason = finding.code;
        }
    } else if (evidence.result.status == "generation-failed") {
        evidence.publication.allowed = false;
        evidence.publication.denial_reason = evidence.result.error_code.empty()
            ? "generation_failed" : evidence.result.error_code;
    } else {
        error = "invalid_commit_state:" + evidence.result.status;
        return false;
    }

    if (evidence.result.status == "generated-authentic") {
        const auto prior = completion_hash_counts_.find(
            evidence.result.normalized_completion_sha256);
        evidence.result.exact_completion_prior_count =
            prior == completion_hash_counts_.end() ? 0 : prior->second;
        evidence.result.exact_completion_duplicate =
            evidence.result.exact_completion_prior_count > 0;
    }

    evidence.development.eligible = true;
    evidence.development.history_immutable = true;
    evidence.development.automatic_identity_mutation_allowed = false;
    evidence.development.review_state = "unreviewed";
    evidence.development.outcome = evidence.result.status;

    evidence.chain.sequence = last_sequence_ + 1;
    evidence.chain.previous_record_sha256 = last_hash_;
    evidence.chain.record_sha256.clear();
    evidence.chain.record_sha256 = sha256(canonical_json(record_without_hash(evidence)));
    const auto record = canonical_json(evidence.to_private_json());
    const auto attempt_path = attempts_dir_ / (evidence.generation_id + ".json");
    if (std::filesystem::exists(attempt_path)) {
        error = "duplicate_generation_id";
        return false;
    }
    if (!atomic_write_private(attempt_path, record + "\n", error)) return false;
    if (!append_private(journal_path_, record + "\n", error)) {
        std::filesystem::remove(attempt_path);
        evidence.publication.allowed = false;
        evidence.publication.denial_reason = "journal_append_failed";
        return false;
    }
    last_sequence_ = evidence.chain.sequence;
    last_hash_ = evidence.chain.record_sha256;
    generation_ids_.insert(evidence.generation_id);
    if (!evidence.invocation.seed_sha256.empty())
        seed_hashes_.insert(evidence.invocation.seed_sha256);
    if (evidence.result.status == "generated-authentic")
        ++completion_hash_counts_[evidence.result.normalized_completion_sha256];
    return true;
}

bool ProvenanceJournal::verify(std::string& error) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!std::filesystem::exists(journal_path_)) return true;
    std::ifstream stream(journal_path_);
    if (!stream) {
        error = "cannot_read_journal";
        return false;
    }
    std::uint64_t expected_sequence = 1;
    std::string expected_previous = kZeroHash;
    std::unordered_set<std::string> generation_ids;
    std::unordered_set<std::string> seed_hashes;
    std::unordered_map<std::string, std::uint64_t> completion_hash_counts;
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        try {
            auto record = nlohmann::json::parse(line);
            const auto sequence = record.at("chain").at("sequence").get<std::uint64_t>();
            const auto previous = record.at("chain").at("previous_record_sha256").get<std::string>();
            const auto stored = record.at("chain").at("record_sha256").get<std::string>();
            const auto generation_id = record.at("generation_id").get<std::string>();
            const auto seed_sha256 = record.at("invocation").at("seed_sha256").get<std::string>();
            record["chain"]["record_sha256"] = "";
            const auto computed = sha256(canonical_json(record));
            if (sequence != expected_sequence || previous != expected_previous || stored != computed) {
                error = "journal_chain_mismatch_at_sequence:" + std::to_string(expected_sequence);
                return false;
            }
            if (!generation_ids.insert(generation_id).second) {
                error = "duplicate_generation_id_at_sequence:" + std::to_string(sequence);
                return false;
            }
            if (!seed_hashes.insert(seed_sha256).second) {
                error = "reused_generation_seed_at_sequence:" + std::to_string(sequence);
                return false;
            }
            if (record.at("result").value("status", "") == "generated-authentic") {
                const auto completion_hash = record.at("result").value(
                    "normalized_completion_sha256", "");
                const auto prior = completion_hash_counts[completion_hash];
                if (record.at("result").value("exact_completion_prior_count", 0ULL) != prior ||
                    record.at("result").value("exact_completion_duplicate", false) != (prior > 0)) {
                    error = "completion_repeat_count_mismatch_at_sequence:" +
                            std::to_string(sequence);
                    return false;
                }
                ++completion_hash_counts[completion_hash];
            }
            expected_previous = stored;
            ++expected_sequence;
        } catch (const std::exception& exception) {
            error = std::string("invalid_journal_record:") + exception.what();
            return false;
        }
    }
    return true;
}

} // namespace village::authenticity
