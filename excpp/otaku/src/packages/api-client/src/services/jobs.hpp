#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/jobs.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Jobs API Service - One-off messaging with automatic polling support
 *
 * The Jobs API provides a simplified interface for one-off messages to agents
 * with automatic response handling and polling capabilities.
 *
 * @example
 * ```typescript
 * const client = new ElizaClient({ baseUrl: 'http://localhost:3000' });
 *
 * // Create a job and poll for completion
 *   userId: 'user-uuid',
 *   content: 'What is the weather today?'
 * });
 *
 * ```
 */
class JobsService {
public:
    std::future<CreateJobResponse> create(CreateJobRequest params);
    std::future<JobDetailsResponse> getJob(const std::string& jobId);
    std::future<JobListResponse> list(std::optional<ListJobsParams> params);
    std::future<JobHealthResponse> health();
    std::future<PollResult> poll(const std::string& jobId, PollOptions options = {});
    std::future<PollResult> createAndPoll(CreateJobRequest params, std::optional<PollOptions> pollOptions);
    std::future<std::string> ask(UUID userId, const std::string& content, std::optional<UUID> agentId, std::optional<PollOptions> pollOptions);


} // namespace elizaos
