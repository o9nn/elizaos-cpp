#include ".lib/base-client.hpp"
#include ".types/base.hpp"
#include ".types/jobs.hpp"
#include ".types/messaging.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Internal payload interfaces for API requests
struct ChannelCreatePayload {
    std::string name;
    ChannelType type;
    UUID server_id;
    std::optional<ChannelMetadata> metadata;
};

struct GroupChannelCreatePayload {
    std::string name;
    UUID server_id;
    std::vector<UUID> participantCentralUserIds;
    std::optional<ChannelType> type;
    std::optional<ChannelMetadata> metadata;
};

struct DmChannelQuery {
    UUID currentUserId;
    UUID targetUserId;
    UUID dmServerId;
};

class MessagingService extends BaseApiClient {
  /**
   * Submit agent replies or system messages
   */
  async submitMessage(params: MessageSubmitParams): Promise<Message> {
    return this.post<Message>('/api/messaging/submit', params);
  }

  /**
   * Notify message completion
   */

  /**
   * Ingest messages from external platforms
   */

  /**
   * Create a new channel
   */
    // Server expects: { name, type, server_id, metadata }

  /**
   * Create a group channel
   */
    // Server expects: { name, server_id, participantCentralUserIds, type?, metadata? }
    // The client currently provides participantIds and may include server_id/type in metadata.

    // Extract and clean metadata - handle legacy fields that might be in metadata

      // Create a new metadata object without the hoisted fields

      // Extract hoisted fields safely using bracket notation (ChannelMetadata allows [key: string]: unknown)

      // Only include metadata if there are remaining properties

      // If caller intended DM, allow type override

  /**
   * Find or create a DM channel
   */
    // Map participantIds -> { currentUserId, targetUserId }
    // Arbitrarily treat the first as current and second as target; callers pass [current, target]

  /**
   * Get channel details
   */

  /**
   * Get channel participants
   */

  /**
   * Add agent to channel
   */

  /**
   * Remove agent from channel
   */

  /**
   * Delete a channel
   */

  /**
   * Clear channel history
   */

  /**
   * Add agent to server (associates agent with a server so it can receive messages)
   */

  /**
   * Remove agent from server
   */

  /**
   * Post a new message to a channel
   */

  /**
   * Get channel messages
   */

  /**
   * Get a specific message
   */

  /**
   * Delete a message from a channel
   */

  /**
   * Update a message
   */

  /**
   * Search messages
   */

  /**
   * List all message servers
   */

  /**
   * Get server channels
   */

  /**
   * Create a new server
   */

  /**
   * Sync server channels
   */

  /**
   * Delete a server
   */

  /**
   * Update a channel
   */

  /**
   * Generate channel title from a user message
   */

  /**
   * Generate dynamic quick start prompts for a channel based on conversation context
   */

  /**
   * Add user to channel participants (implemented via updateChannel)
   */
    // First get current participants

    // Add new user if not already present

  /**
   * Add multiple users to channel participants (implemented via updateChannel)
   */
    // First get current participants

    // Add new users that aren't already present

  /**
   * Remove user from channel participants (implemented via updateChannel)
   */
    // First get current participants

    // Remove user from participants

  // =============================================================================
  // Jobs API - One-off messaging
  // =============================================================================

  /**
   * Create a new job (one-off message to agent)
   * 
   * This creates a temporary channel and sends a message to the agent.
   * The job tracks the request and response, with automatic cleanup.
   * 
   * @param params - Job creation parameters
   * @returns Job details including jobId and status
   * 
   * @example
   * ```typescript
   * const job = await client.messaging.createJob({
   *   agentId: 'agent-uuid', // optional - uses first agent if not provided
   *   userId: 'user-uuid',
   *   content: 'What is the weather?',
   *   timeoutMs: 30000,
   *   metadata: { source: 'api' }
   * });
   * console.log(job.jobId);
   * ```
   */

  /**
   * Get job details and status
   * 
   * Retrieves the current status of a job, including the result if completed.
   * 
   * @param jobId - The unique job identifier
   * @returns Job details including status and result
   * 
   * @example
   * ```typescript
   * const job = await client.messaging.getJob('job-uuid');
   * if (job.status === JobStatus.COMPLETED) {
   *   console.log(job.result?.message.content);
   * }
   * ```
   */

  /**
   * List all jobs with optional filtering
   * 
   * @param params - List parameters (limit and status filter)
   * @returns List of jobs with total counts
   * 
   * @example
   * ```typescript
   * const { jobs, total } = await client.messaging.listJobs({
   *   limit: 10,
   *   status: JobStatus.COMPLETED
   * });
   * ```
   */

  /**
   * Get jobs service health status
   * 
   * @returns Health information including job counts by status
   * 
   * @example
   * ```typescript
   * const health = await client.messaging.getJobsHealth();
   * console.log(`Total jobs: ${health.totalJobs}`);
   * console.log(`Completed: ${health.statusCounts.completed}`);
   * ```
   */

  /**
   * Poll a job until it completes or times out
   * 
   * Continuously polls the job status until it reaches a terminal state
   * (COMPLETED, FAILED, or TIMEOUT).
   * 
   * @param jobId - The job ID to poll
   * @param interval - Polling interval in milliseconds (default: 1000)
   * @param maxAttempts - Maximum number of poll attempts (default: 30)
   * @returns Final job details
   * @throws Error if job fails, times out, or max attempts reached
   * 
   * @example
   * ```typescript
   * try {
   *   const job = await client.messaging.createJob({
   *     userId: 'user-uuid',
   *     content: 'What is 2+2?'
   *   });
   *   
   *   const result = await client.messaging.pollJob(job.jobId, 1000, 30);
   *   console.log(result.result?.message.content);
   * } catch (error) {
   *   console.error('Job failed:', error);
   * }
   * ```
   */

      // Check if job reached a terminal state

      // Wait before next poll

  /**
   * Create a job and wait for the result
   * 
   * Convenience method that creates a job and polls until completion.
   * 
   * @param params - Job creation parameters
   * @param pollInterval - Polling interval in milliseconds (default: 1000)
   * @param maxAttempts - Maximum number of poll attempts (default: 30)
   * @returns Final job details with result
   * @throws Error if job fails, times out, or max attempts reached
   * 
   * @example
   * ```typescript
   * const result = await client.messaging.createAndWaitForJob({
   *   userId: 'user-uuid',
   *   content: 'Explain quantum computing'
   * });
   * console.log(result.result?.message.content);
   * ```
   */

} // namespace elizaos
