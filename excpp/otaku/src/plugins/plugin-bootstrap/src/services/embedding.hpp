#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct EmbeddingQueueItem {
    Memory memory;
    std::string priority;
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
class EmbeddingGenerationService {
public:
    static std::future<Service> start(IAgentRuntime runtime);
    std::future<void> initialize();
    std::future<void> handleEmbeddingRequest(EmbeddingGenerationPayload payload);
    void makeRoomInQueue();
    void insertItemByPriority(EmbeddingQueueItem queueItem);
    void startProcessing();
    std::future<void> processQueue();
    std::future<void> generateEmbedding(EmbeddingQueueItem item);
    std::future<void> stop();
    double getQueueSize();
     getQueueStats();
    void clearQueue();
};


} // namespace elizaos
