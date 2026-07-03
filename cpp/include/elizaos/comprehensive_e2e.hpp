#pragma once

#include <chrono>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace elizaos {
namespace comprehensive_e2e {

/**
 * Canonical stages exercised by the comprehensive ElizaOS autonomy pipeline.
 * The enum gives E2E tests and diagnostics a stable, typed vocabulary instead
 * of ad-hoc strings while remaining header-only for public API smoke tests.
 */
enum class Stage {
    Memory,
    Browser,
    Conversation,
    Knowledge,
    Character,
    Communication,
    Hat,
    Mcp,
    Integration
};

inline const char* toString(Stage stage) {
    switch (stage) {
        case Stage::Memory: return "memory";
        case Stage::Browser: return "browser";
        case Stage::Conversation: return "conversation";
        case Stage::Knowledge: return "knowledge";
        case Stage::Character: return "character";
        case Stage::Communication: return "communication";
        case Stage::Hat: return "hat";
        case Stage::Mcp: return "mcp";
        case Stage::Integration: return "integration";
    }
    return "unknown";
}

struct StageResult {
    Stage stage{Stage::Integration};
    std::string name;
    bool passed{false};
    std::string detail;
    double latency_ms{0.0};
};

inline StageResult makeStageResult(
    Stage stage,
    std::string name,
    bool passed,
    std::string detail = {},
    double latency_ms = 0.0) {
    StageResult result;
    result.stage = stage;
    result.name = std::move(name);
    result.passed = passed;
    result.detail = std::move(detail);
    result.latency_ms = latency_ms < 0.0 ? 0.0 : latency_ms;
    return result;
}

class PipelineSnapshot {
public:
    PipelineSnapshot()
        : created_at_(std::chrono::system_clock::now()) {}

    void add(StageResult result) {
        stages_.push_back(std::move(result));
    }

    void add(Stage stage, std::string name, bool passed, std::string detail = {}, double latency_ms = 0.0) {
        add(makeStageResult(stage, std::move(name), passed, std::move(detail), latency_ms));
    }

    const std::vector<StageResult>& stages() const noexcept {
        return stages_;
    }

    std::chrono::system_clock::time_point createdAt() const noexcept {
        return created_at_;
    }

    std::size_t totalCount() const noexcept {
        return stages_.size();
    }

    std::size_t passedCount() const noexcept {
        std::size_t count = 0;
        for (const auto& stage : stages_) {
            if (stage.passed) {
                ++count;
            }
        }
        return count;
    }

    std::size_t failedCount() const noexcept {
        return totalCount() - passedCount();
    }

    bool passed() const noexcept {
        return !stages_.empty() && failedCount() == 0;
    }

    std::string summary() const {
        std::ostringstream out;
        out << "ElizaOS comprehensive E2E: " << passedCount() << "/" << totalCount()
            << " stages passed";
        if (failedCount() > 0) {
            out << " (failed:";
            bool first = true;
            for (const auto& stage : stages_) {
                if (!stage.passed) {
                    out << (first ? " " : ", ") << toString(stage.stage) << ":" << stage.name;
                    first = false;
                }
            }
            out << ")";
        }
        return out.str();
    }

private:
    std::chrono::system_clock::time_point created_at_;
    std::vector<StageResult> stages_;
};

} // namespace comprehensive_e2e
} // namespace elizaos
