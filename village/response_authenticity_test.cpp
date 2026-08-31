#include "response_authenticity.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using village::authenticity::GenerationEvidence;
using village::authenticity::ProvenanceJournal;
using village::authenticity::canonical_json;
using village::authenticity::normalize_completion;
using village::authenticity::sha256;
using village::authenticity::validate_for_publication;

namespace {

int failures = 0;
int checks = 0;

void check(bool condition, const std::string& name) {
    ++checks;
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << name << '\n';
    }
}

GenerationEvidence valid_evidence(const std::string& resident,
                                  const std::string& role,
                                  int seed) {
    GenerationEvidence evidence;
    evidence.generation_id = sha256(resident + std::to_string(seed)).substr(0, 32);
    evidence.created_at = "2026-08-31T12:00:00.000Z";
    evidence.correlation_id = "corr-" + resident;

    evidence.resident.id = resident;
    evidence.resident.identity_version = "elizaos-cpp:test-revision";
    evidence.resident.role_prompt = role;
    evidence.resident.role_prompt_sha256 = sha256(role);
    evidence.resident.identity_manifest = canonical_json({
        {"resident", resident},
        {"identity_version", evidence.resident.identity_version},
        {"role_prompt_sha256", evidence.resident.role_prompt_sha256}
    });
    evidence.resident.identity_manifest_sha256 =
        sha256(evidence.resident.identity_manifest);

    evidence.context.memory_snapshot = "private memory for " + resident;
    evidence.context.memory_snapshot_sha256 =
        sha256(evidence.context.memory_snapshot);
    evidence.context.shared_context = "shared event context";
    evidence.context.shared_context_sha256 =
        sha256(evidence.context.shared_context);
    evidence.context.cognitive_state = canonical_json({
        {"resident", resident}, {"sti", 201.0}, {"gear", "test"}
    });
    evidence.context.cognitive_state_sha256 =
        sha256(evidence.context.cognitive_state);
    evidence.context.selection_record = canonical_json({
        {"policy", "test-private-memory-v1"}, {"resident", resident}
    });
    evidence.context.selection_record_sha256 =
        sha256(evidence.context.selection_record);
    evidence.context.system_prompt = role + "\n" + evidence.context.memory_snapshot;
    evidence.context.system_prompt_sha256 = sha256(evidence.context.system_prompt);
    evidence.context.combined_context_sha256 = sha256(
        evidence.context.system_prompt + "\n--MEMORY--\n" +
        evidence.context.memory_snapshot + "\n--SHARED--\n" +
        evidence.context.shared_context + "\n--STATE--\n" +
        evidence.context.cognitive_state);

    evidence.stimulus.source = "test.operator";
    evidence.stimulus.target = resident;
    evidence.stimulus.text = "What do you notice right now?";
    evidence.stimulus.utf8_sha256 = sha256(evidence.stimulus.text);
    evidence.stimulus.parent_event_id = 100;
    evidence.stimulus.parent_tic = 200;

    evidence.invocation.started_at = "2026-08-31T12:00:00.001Z";
    evidence.invocation.server = "http://127.0.0.1:2242/v1/chat/completions";
    evidence.invocation.model_id = "qwen2.5-14b-instruct-awq";
    evidence.invocation.model_revision = "sha256:model-test";
    evidence.invocation.temperature = 0.7;
    evidence.invocation.min_p = 0.1;
    evidence.invocation.max_tokens = 512;
    evidence.invocation.seed = seed;
    evidence.invocation.seed_sha256 = sha256(std::to_string(seed));
    evidence.invocation.request_body = canonical_json({
        {"model", evidence.invocation.model_id},
        {"messages", {
            {{"role", "system"}, {"content", evidence.context.system_prompt}},
            {{"role", "user"}, {"content", evidence.stimulus.text}}
        }},
        {"temperature", evidence.invocation.temperature},
        {"min_p", evidence.invocation.min_p},
        {"max_tokens", evidence.invocation.max_tokens},
        {"seed", evidence.invocation.seed}
    });
    evidence.invocation.request_body_sha256 =
        sha256(evidence.invocation.request_body);

    evidence.result.status = "generated-unverified";
    evidence.result.finished_at = "2026-08-31T12:00:00.123Z";
    evidence.result.latency_ms = 122;
    evidence.result.http_status = 200;
    evidence.result.transport_code = 0;
    evidence.result.finish_reason = "stop";
    evidence.result.prompt_tokens = 100;
    evidence.result.completion_tokens = 12;
    evidence.result.raw_response = "{\"choices\":[{\"message\":{\"content\":\"I am uncertain.\"}}]}";
    evidence.result.raw_response_sha256 = sha256(evidence.result.raw_response);
    evidence.result.raw_completion = "I am uncertain.";
    evidence.result.raw_completion_sha256 = sha256(evidence.result.raw_completion);
    evidence.result.normalized_completion = evidence.result.raw_completion;
    evidence.result.normalized_completion_sha256 =
        sha256(evidence.result.normalized_completion);
    return evidence;
}

std::filesystem::path fresh_directory(const std::string& name) {
    const auto path = std::filesystem::temp_directory_path() /
        ("cogverse-authenticity-" + name);
    std::filesystem::remove_all(path);
    return path;
}

void test_valid_and_distinct_residents() {
    auto echo = valid_evidence("echo", "You are Echo, exploratory and recursive.", 101);
    auto aion = valid_evidence("aion", "You are Aion, precise about time.", 102);
    check(validate_for_publication(echo).ok, "valid Echo evidence accepted");
    check(validate_for_publication(aion).ok, "valid Aion evidence accepted");
    check(echo.resident.role_prompt_sha256 != aion.resident.role_prompt_sha256,
          "resident role commitments are distinct");
    check(echo.context.system_prompt_sha256 != aion.context.system_prompt_sha256,
          "resident system prompts are distinct");
    check(echo.invocation.request_body_sha256 != aion.invocation.request_body_sha256,
          "model requests are resident-specific");
    check(echo.generation_id != aion.generation_id,
          "generation IDs are distinct");

    auto changed_context = echo;
    changed_context.context.memory_snapshot = "new private memory for echo";
    changed_context.context.memory_snapshot_sha256 =
        sha256(changed_context.context.memory_snapshot);
    changed_context.context.system_prompt =
        changed_context.resident.role_prompt + "\n" + changed_context.context.memory_snapshot;
    changed_context.context.system_prompt_sha256 =
        sha256(changed_context.context.system_prompt);
    changed_context.context.combined_context_sha256 = sha256(
        changed_context.context.system_prompt + "\n--MEMORY--\n" +
        changed_context.context.memory_snapshot + "\n--SHARED--\n" +
        changed_context.context.shared_context + "\n--STATE--\n" +
        changed_context.context.cognitive_state);
    auto request = nlohmann::json::parse(changed_context.invocation.request_body);
    request["messages"][0]["content"] = changed_context.context.system_prompt;
    changed_context.invocation.request_body = canonical_json(request);
    changed_context.invocation.request_body_sha256 =
        sha256(changed_context.invocation.request_body);
    check(validate_for_publication(changed_context).ok,
          "changed private context remains a valid bound request");
    check(changed_context.invocation.request_body_sha256 !=
              echo.invocation.request_body_sha256,
          "changed private context changes the exact model request");
}

void test_fail_closed_validation() {
    auto bad_role = valid_evidence("echo", "Echo role", 201);
    bad_role.resident.role_prompt_sha256 = sha256("counterfeit role");
    check(!validate_for_publication(bad_role).ok, "role hash mismatch rejected");

    auto deterministic = valid_evidence("echo", "Echo role", 202);
    deterministic.invocation.temperature = 0.0;
    check(!validate_for_publication(deterministic).ok,
          "deterministic temperature rejected");

    auto missing_revision = valid_evidence("echo", "Echo role", 203);
    missing_revision.invocation.model_revision = "unconfigured";
    check(!validate_for_publication(missing_revision).ok,
          "unconfigured model revision rejected");

    auto wrong_context = valid_evidence("echo", "Echo role", 204);
    auto request = nlohmann::json::parse(wrong_context.invocation.request_body);
    request["messages"][0]["content"] = "shared counterfeit prompt";
    wrong_context.invocation.request_body = request.dump();
    wrong_context.invocation.request_body_sha256 =
        sha256(wrong_context.invocation.request_body);
    check(!validate_for_publication(wrong_context).ok,
          "request/context mismatch rejected");

    auto empty_stimulus = valid_evidence("echo", "Echo role", 205);
    empty_stimulus.stimulus.text.clear();
    empty_stimulus.stimulus.utf8_sha256 = sha256("");
    check(!validate_for_publication(empty_stimulus).ok,
          "empty stimulus rejected");
}

void test_normalization_preserves_imperfections() {
    std::vector<std::string> operations;
    const std::string raw =
        "<|im_start|>assistant\nI may be wrong <|im_start|> inside.<|im_end|>";
    const auto normalized = normalize_completion(raw, operations);
    check(normalized == "I may be wrong <|im_start|> inside.",
          "only terminal transport framing removed");
    check(normalized.find("may be wrong") != std::string::npos,
          "uncertainty remains intact");
    check(operations.size() == 1 &&
              operations.front() == "remove_terminal_chat_framing",
          "normalization is explicitly recorded");
}

void test_journal_and_entropy_guards() {
    const auto path = fresh_directory("journal");
    ProvenanceJournal journal(path);
    auto success = valid_evidence("echo", "Echo role", 301);
    std::string error;
    check(journal.commit(success, error), "valid generation committed");
    check(success.publication.allowed, "valid generation publication allowed");
    check(success.result.status == "generated-authentic",
          "valid generation promoted to generated-authentic");
    check(success.development.eligible, "authentic generation enters development evidence");
    check(success.development.history_immutable,
          "development evidence cannot rewrite historical output");
    check(!success.development.automatic_identity_mutation_allowed,
          "development evidence cannot auto-mutate identity");
    check(journal.verify(error), "journal chain verifies");

    auto duplicate_id = valid_evidence("aion", "Aion role", 302);
    duplicate_id.generation_id = success.generation_id;
    check(!journal.commit(duplicate_id, error), "duplicate generation ID rejected");

    auto reused_seed = valid_evidence("aion", "Aion role", 301);
    check(!journal.commit(reused_seed, error), "reused seed rejected");

    auto failed = valid_evidence("aion", "Aion role", 303);
    failed.result.status = "generation-failed";
    failed.result.error_code = "transport_error";
    failed.result.error_detail = "connection reset";
    failed.result.raw_response.clear();
    failed.result.raw_response_sha256 = sha256("");
    failed.result.raw_completion.clear();
    failed.result.raw_completion_sha256.clear();
    failed.result.normalized_completion.clear();
    failed.result.normalized_completion_sha256.clear();
    check(journal.commit(failed, error), "failed generation preserved in journal");
    check(!failed.publication.allowed, "failed generation cannot be speech");
    check(failed.development.eligible, "failed generation remains development evidence");
    check(failed.development.outcome == "generation-failed",
          "failure outcome remains explicit");

    auto repeated_completion = valid_evidence("aion", "Aion role", 304);
    check(journal.commit(repeated_completion, error),
          "genuine repeated completion remains journaled");
    check(repeated_completion.publication.allowed,
          "repeated text from a distinct real invocation remains authentic speech");
    check(repeated_completion.result.exact_completion_duplicate &&
              repeated_completion.result.exact_completion_prior_count == 1,
          "exact repeated completion is explicitly flagged for development review");

    const auto journal_path = path / "journal.jsonl";
    std::ifstream source(journal_path);
    std::string content((std::istreambuf_iterator<char>(source)),
                        std::istreambuf_iterator<char>());
    const auto position = content.find("connection reset");
    check(position != std::string::npos, "failure detail stored as evidence");
    if (position != std::string::npos) content[position] = 'X';
    std::ofstream tampered(journal_path, std::ios::trunc);
    tampered << content;
    tampered.close();
    check(!journal.verify(error), "journal tampering detected");
    std::filesystem::remove_all(path);
}

} // namespace

int main() {
    test_valid_and_distinct_residents();
    test_fail_closed_validation();
    test_normalization_preserves_imperfections();
    test_journal_and_entropy_guards();
    std::cout << "response_authenticity_test checks=" << checks
              << " failures=" << failures << '\n';
    return failures == 0 ? 0 : 1;
}
