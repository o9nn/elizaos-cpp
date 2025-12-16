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



struct EmbeddingQueueItem {
    Memory memory;
    'high' | 'normal' | 'low' priority;
    double retryCount;
    double maxRetries;
    double addedAt;
    std::optional<std::string> runId;
};

/**
 * Service responsible for generating embeddings asynchronously
 * This service listens for EMBEDDING_GENERATION_REQUESTED events
 * and processes them in a queue to avoid blocking the main runtime
 */
class EmbeddingGenerationService extends Service {
  static serviceType = 'embedding-generation';
  capabilityDescription = 'Handles asynchronous embedding generation for memories';

  private queue: EmbeddingQueueItem[] = [];
  private isProcessing = false;
  private processingInterval: NodeJS.Timeout | null = null;
  private maxQueueSize = 1000;
  private batchSize = 10; // Process up to 10 embeddings at a time
  private processingIntervalMs = 100; // Check queue every 100ms

  static async start(runtime: IAgentRuntime): Promise<Service> {
    logger.info('[EmbeddingService] Starting embedding generation service');
    const service = new EmbeddingGenerationService(runtime);
    await service.initialize();
    return service;
  }

    // Register event handlers

    // Start the processing loop

    // Skip if memory already has embeddings

    // Check queue size and make room if needed

    // Add to queue

    // Insert based on priority

  /**
   * Make room in the queue by removing items based on priority and age
   * Removes 10% of the queue (minimum 1, maximum 10 items)
   */
    // Remove 10% of queue, but at least 1 and at most 10 items

    // Create array with items and their original indices

    // Sort by priority (low first for removal) and age (oldest first)
      // Priority order for removal: low > normal > high

      // Within same priority, remove older items first

    // Get the original indices of items to remove (first N items after sorting)

    // Keep items that are not in the removal set

  /**
   * Insert an item into the queue based on its priority
   * High priority items go to the front, normal in the middle, low at the end
   */
      // Find the position after the last high priority item
      // Add to end of queue
      // Normal priority - add after high priority items but before low priority items

      // First, skip all high priority items

      // Then find where low priority items start

      // Process a batch of items

      // Process items in parallel

          // Retry if under max retries
            // Re-add to queue with same priority using proper insertion
            // Log embedding failure

            // Emit failure event

      // Generate embedding

      // Update memory with embedding

        // Log embedding completion

        // Emit completion event

    // Process remaining high priority items before shutdown

  // Public methods for monitoring


} // namespace elizaos
