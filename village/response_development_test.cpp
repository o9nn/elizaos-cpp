#include "response_development.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace village;

namespace {
int failures = 0;
int checks = 0;
void check(bool condition, const std::string& name) {
    ++checks;
    if (!condition) { ++failures; std::cerr << "FAIL: " << name << '\n'; }
}

authenticity::GenerationEvidence committed_generation(
    authenticity::ProvenanceJournal& journal) {
    authenticity::GenerationEvidence evidence;
    evidence.generation_id = authenticity::sha256("generation").substr(0, 32);
    evidence.created_at = authenticity::utc_now_iso8601();
    evidence.resident.id = "echo";
    evidence.resident.identity_version = "test-identity";
    evidence.resident.role_prompt = "You are Echo.";
    evidence.resident.role_prompt_sha256 = authenticity::sha256(evidence.resident.role_prompt);
    evidence.resident.identity_manifest = "{\"resident\":\"echo\"}";
    evidence.resident.identity_manifest_sha256 = authenticity::sha256(evidence.resident.identity_manifest);
    evidence.context.system_prompt = "You are Echo.\nMemory.";
    evidence.context.system_prompt_sha256 = authenticity::sha256(evidence.context.system_prompt);
    evidence.context.memory_snapshot = "Memory.";
    evidence.context.memory_snapshot_sha256 = authenticity::sha256(evidence.context.memory_snapshot);
    evidence.context.shared_context = "";
    evidence.context.shared_context_sha256 = authenticity::sha256("");
    evidence.context.cognitive_state = "{\"sti\":200}";
    evidence.context.cognitive_state_sha256 = authenticity::sha256(evidence.context.cognitive_state);
    evidence.context.selection_record = "{\"policy\":\"test\"}";
    evidence.context.selection_record_sha256 = authenticity::sha256(evidence.context.selection_record);
    evidence.context.combined_context_sha256 = authenticity::sha256(
        evidence.context.system_prompt + "\n--MEMORY--\n" + evidence.context.memory_snapshot +
        "\n--SHARED--\n" + evidence.context.shared_context + "\n--STATE--\n" +
        evidence.context.cognitive_state);
    evidence.stimulus.source = "test";
    evidence.stimulus.target = "echo";
    evidence.stimulus.text = "What do you notice?";
    evidence.stimulus.utf8_sha256 = authenticity::sha256(evidence.stimulus.text);
    evidence.invocation.started_at = authenticity::utc_now_iso8601();
    evidence.invocation.server = "local";
    evidence.invocation.model_id = "test-model";
    evidence.invocation.model_revision = "test-revision";
    evidence.invocation.temperature = 0.7;
    evidence.invocation.min_p = 0.05;
    evidence.invocation.max_tokens = 32;
    evidence.invocation.seed = 77;
    evidence.invocation.seed_sha256 = authenticity::sha256("77");
    evidence.invocation.request_body = authenticity::canonical_json({
        {"model", "test-model"},
        {"messages", {
            {{"role", "system"}, {"content", evidence.context.system_prompt}},
            {{"role", "user"}, {"content", evidence.stimulus.text}}
        }},
        {"temperature", 0.7}, {"min_p", 0.05}, {"max_tokens", 32}, {"seed", 77}
    });
    evidence.invocation.request_body_sha256 = authenticity::sha256(evidence.invocation.request_body);
    evidence.result.status = "generated-unverified";
    evidence.result.finished_at = authenticity::utc_now_iso8601();
    evidence.result.latency_ms = 12;
    evidence.result.http_status = 200;
    evidence.result.raw_response = "{\"content\":\"I may be wrong.\"}";
    evidence.result.raw_response_sha256 = authenticity::sha256(evidence.result.raw_response);
    evidence.result.raw_completion = "I may be wrong.";
    evidence.result.raw_completion_sha256 = authenticity::sha256(evidence.result.raw_completion);
    evidence.result.normalized_completion = evidence.result.raw_completion;
    evidence.result.normalized_completion_sha256 = authenticity::sha256(evidence.result.normalized_completion);
    std::string error;
    check(journal.commit(evidence, error), "generation evidence committed");
    return evidence;
}
}

int main() {
    const auto root = std::filesystem::temp_directory_path() / "cogverse-development-test";
    std::filesystem::remove_all(root);
    authenticity::ProvenanceJournal provenance(root / "generation");
    auto generation = committed_generation(provenance);
    const auto attempt = root / "generation" / "attempts" / (generation.generation_id + ".json");
    const auto before = authenticity::sha256([&]() {
        std::ifstream input(attempt);
        return std::string((std::istreambuf_iterator<char>(input)), {});
    }());

    development::DevelopmentLedger ledger(root / "development", root / "generation");
    development::EvaluatorFinding finding;
    finding.finding_id = authenticity::sha256("finding").substr(0, 32);
    finding.created_at = authenticity::utc_now_iso8601();
    finding.generation_id = generation.generation_id;
    finding.generation_record_sha256 = generation.chain.record_sha256;
    finding.reviewer_id = "human:dan";
    finding.reviewer_kind = "human";
    finding.metrics = {{"role_coherence", 0.7}, {"uncertainty_calibration", 0.8}};
    finding.narrative = "The uncertainty is visible and should remain in the historical output.";
    finding.narrative_sha256 = authenticity::sha256(finding.narrative);
    finding.outcome = "reviewed-with-imperfection";
    std::string error;
    check(ledger.append_finding(finding, error), "evaluator finding appended");

    development::EvaluatorFinding bad_reference = finding;
    bad_reference.finding_id = authenticity::sha256("bad-finding").substr(0, 32);
    bad_reference.generation_record_sha256 = std::string(64, 'f');
    check(!ledger.append_finding(bad_reference, error), "invalid generation reference rejected");

    development::DevelopmentProposal unbounded;
    unbounded.proposal_id = authenticity::sha256("unbounded").substr(0, 32);
    unbounded.created_at = authenticity::utc_now_iso8601();
    unbounded.proposer_id = "resident:echo";
    unbounded.target = "role_prompt";
    unbounded.finding_record_sha256s = {finding.chain.record_sha256};
    unbounded.deltas = {{
        "role_prompt.artifact", "replace-artifact",
        authenticity::sha256("role-before"), authenticity::sha256("role-after"),
        0.0, 0.0, false
    }};
    unbounded.maximum_change_fraction = 0.3;
    unbounded.required_tests = {"resident-isolation", "historical-immutability"};
    unbounded.rollback_artifact_sha256 = authenticity::sha256("rollback-artifact");
    check(!ledger.append_proposal(unbounded, error), "unbounded proposal rejected");

    development::DevelopmentProposal unreviewed = unbounded;
    unreviewed.proposal_id = authenticity::sha256("unreviewed").substr(0, 32);
    unreviewed.maximum_change_fraction = 0.1;
    unreviewed.finding_record_sha256s = {std::string(64, 'e')};
    check(!ledger.append_proposal(unreviewed, error), "unreviewed evidence rejected");

    development::DevelopmentProposal replacement_speech = unbounded;
    replacement_speech.proposal_id = authenticity::sha256("replacement-speech").substr(0, 32);
    replacement_speech.maximum_change_fraction = 0.1;
    replacement_speech.deltas = {{
        "replacement_speech", "replace-artifact",
        authenticity::sha256("before"), authenticity::sha256("I am a fabricated answer."),
        0.0, 0.0, false
    }};
    check(!ledger.append_proposal(replacement_speech, error),
          "replacement resident speech is structurally rejected");

    development::DevelopmentProposal proposal = unbounded;
    proposal.proposal_id = authenticity::sha256("bounded").substr(0, 32);
    proposal.maximum_change_fraction = 0.1;
    check(ledger.append_proposal(proposal, error), "bounded reviewed proposal appended");
    const auto proposal_json = proposal.to_json().dump();
    check(proposal_json.find("change_description") == std::string::npos,
          "proposal schema contains no free-text change description");
    check(proposal_json.find("rollback_plan") == std::string::npos,
          "proposal schema contains no free-text rollback plan");

    development::PromotionDecision blocked;
    blocked.decision_id = authenticity::sha256("blocked").substr(0, 32);
    blocked.created_at = authenticity::utc_now_iso8601();
    blocked.proposal_id = proposal.proposal_id;
    blocked.proposal_record_sha256 = proposal.chain.record_sha256;
    blocked.approved_by = "human:dan";
    blocked.decision = "promoted";
    blocked.tests_verified = false;
    check(!ledger.append_promotion(blocked, error), "promotion without tests rejected");

    development::PromotionDecision promoted = blocked;
    promoted.decision_id = authenticity::sha256("promoted").substr(0, 32);
    promoted.tests_verified = true;
    promoted.rollback_artifact_sha256 = authenticity::sha256("rollback-artifact");
    check(ledger.append_promotion(promoted, error), "reviewed tested promotion recorded");
    check(ledger.verify(error), "development chains verify");

    const auto after = authenticity::sha256([&]() {
        std::ifstream input(attempt);
        return std::string((std::istreambuf_iterator<char>(input)), {});
    }());
    check(before == after, "generation history remains byte-for-byte immutable");

    std::cout << "response_development_test checks=" << checks
              << " failures=" << failures << '\n';
    std::filesystem::remove_all(root);
    return failures == 0 ? 0 : 1;
}
