#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".types.hpp"
#include ".utils/log.hpp"
#include "agents.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
#include "utils/template.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Reviewer and retry loop implementations for agent solutions
 * Converted from sweagent/agent/reviewer.py
 */

/**
 * Information passed to the reviewer
 */
struct ReviewSubmission {
    Trajectory trajectory;
    AgentInfo info;
    InstanceStats modelStats;
};

class ReviewSubmissionImpl {
public:
    ReviewSubmissionImpl(const std::any& data);

private:
    Trajectory trajectory_;
    AgentInfo info_;
    InstanceStats modelStats_;
};

/**
 * Result from reviewer
 */
struct ReviewerResult {
    std::variant<bool, double> accept;
    std::vector<std::string> outputs;
    History messages;
};

/**
 * Output from preselector
 */
struct PreselectorOutput {
    std::vector<double> chosenIdx;
    std::string response;
    History messages;
};

/**
 * Output from chooser
 */
struct ChooserOutput {
    double chosenIdx;
    std::string response;
    std::optional<std::optional<PreselectorOutput>> preselectorOutput;
    History messages;
};

/**
 * Abstract reviewer interface
 */

/**
 * Abstract retry loop interface
 */
    bool retry();
    void onSubmit(ReviewSubmission _submission);
    void onModelQuery(InstanceStats _attemptStats);
    void onAttemptStarted(double _iAttempt, unknown _agent);

/**
 * Configuration for trajectory formatter
 */
struct TrajFormatterConfig {
    std::vector<std::string> filter;
    std::vector<std::string> outputFilter;
    std::string itemTemplate;
    double onlyShowLastNOutput;
};

/**
 * Configuration for reviewer
 */
struct ReviewerConfig {
    std::string systemTemplate;
    std::string instanceTemplate;
    double failureScorePenalty;
    TrajFormatterConfig trajFormatter;
    double nSample;
    double reduceByStd;
    std::string type;
};

/**
 * Configuration for chooser
 */
struct ChooserConfig {
    ModelConfig model;
    std::string systemTemplate;
    std::string instanceTemplate;
    std::string submissionTemplate;
    double maxLenSubmission;
    std::optional<std::optional<PreselectorConfig>> preselector;
};

/**
 * Configuration for preselector
 */
struct PreselectorConfig {
    ModelConfig model;
    std::string systemTemplate;
    std::string instanceTemplate;
    std::string submissionTemplate;
    double maxLenSubmission;
};

/**
 * Configuration for chooser retry loop
 */
struct ChooserRetryLoopConfig {
    std::string type;
    ChooserConfig chooser;
    double maxAttempts;
    double minBudgetForNewAttempt;
    double costLimit;
};

/**
 * Configuration for score retry loop
 */
struct ScoreRetryLoopConfig {
    std::string type;
    ReviewerConfig reviewerConfig;
    double acceptScore;
    double maxAccepts;
    double maxAttempts;
    double minBudgetForNewAttempt;
    double costLimit;
    ModelConfig model;
};

using RetryLoopConfig = std::variant<ScoreRetryLoopConfig, ChooserRetryLoopConfig>;

/**
 * Trajectory formatter for use in prompts
 */
class TrajectoryFormatter {
public:
    TrajectoryFormatter(TrajFormatterConfig config);
    bool includeStep(TrajectoryStep item);
    bool includeStepOutput(TrajectoryStep item, double iStep, double nSteps);
    std::string formatTrajectoryStep(TrajectoryStep step, double iStep, double nSteps, double iTraj = 1);
    std::string formatTrajectory(Trajectory trajectory, double iTraj = 1);

private:
    TrajFormatterConfig config_;
};

/**
 * Reviewer implementation
 */
class Reviewer {
public:
    Reviewer(ReviewerConfig config, AbstractModel model);
    History formatMessages(ProblemStatement instance, ReviewSubmission submission);
    double interpret(const std::string& response);
    std::future<ReviewerResult> review(ProblemStatement instance, ReviewSubmission submission);

private:
    ReviewerConfig config_;
    AbstractModel model_;
    TrajectoryFormatter trajFormatter_;
    AgentLogger logger_;
};

/**
 * Chooser implementation
 */
class Chooser {
public:
    Chooser(ChooserConfig config);
    double interpret(const std::string& response);
    std::string formatSubmission(const std::string& _problemStatement, ReviewSubmission submission);
    History buildMessages(const std::string& problemStatement, const std::vector<ReviewSubmission>& input);
    std::future<ChooserOutput> choose(const std::string& problemStatement, const std::vector<ReviewSubmission>& input);

private:
    ChooserConfig config_;
    AbstractModel model_;
    AgentLogger logger_;
};

/**
 * Chooser retry loop implementation
 */
class ChooserRetryLoop {
public:
    ChooserRetryLoop(ChooserRetryLoopConfig config, ProblemStatement problemStatement);
    InstanceStats totalStats();
    InstanceStats reviewModelStats() const;
    double nAttempts();
    void onSubmit(ReviewSubmission _submission);
    bool retry();
    std::variant<Promise<number, null>> getBest();

private:
    ChooserRetryLoopConfig config_;
    ProblemStatement problemStatement_;
    Chooser chooser_;
    AgentLogger logger_;
};

/**
 * Score retry loop implementation
 */
class ScoreRetryLoop {
public:
    ScoreRetryLoop(ScoreRetryLoopConfig config, ProblemStatement problemStatement);
    InstanceStats reviewModelStats() const;
    std::vector<ReviewerResult> allReviews() const;
    double nAttempts();
    double nAccepted();
    InstanceStats totalStats();
    std::future<void> onSubmit(ReviewSubmission submission);
    std::future<double> review();
    bool retry();
    std::optional<double> getBest();

private:
    AbstractModel model_;
    ProblemStatement problemStatement_;
    AbstractReviewer reviewer_;
    ScoreRetryLoopConfig config_;
    AgentLogger logger_;
};

/**
 * Factory function to create retry loop from configuration
 */
std::variant<ScoreRetryLoop, ChooserRetryLoop> getRetryLoopFromConfig(RetryLoopConfig config, ProblemStatement problemStatement);

} // namespace elizaos
