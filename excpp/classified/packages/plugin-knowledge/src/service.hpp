#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "config.hpp"
#include "docs-loader.hpp"
#include "document-processor.ts.hpp"
#include "elizaos/core.hpp"
#include "types.ts.hpp"
#include "utils.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Knowledge Service - Provides retrieval augmented generation capabilities
 */
class KnowledgeService {
public:
    KnowledgeService(IAgentRuntime runtime, std::optional<std::optional<KnowledgeConfig>> config);
    std::future<void> loadInitialDocuments();
    static std::future<KnowledgeService> start(IAgentRuntime runtime);
    static std::future<void> stop(IAgentRuntime runtime);
    Promise< addKnowledge(AddKnowledgeOptions options);
    Promise< processDocument(auto clientDocumentId, auto contentType, auto originalFilename, auto worldId, auto content, auto roomId, auto entityId, auto metadata);
        // Routes always send base64, but docs-loader sends plain text

        // First, check if this looks like base64
        if (looksLikeBase64(content)) files();
    void handleProcessingError(const std::any& error, const std::string& context);
    std::future<bool> checkExistingKnowledge(UUID knowledgeId);
    std::future<std::vector<KnowledgeItem>> getKnowledge(Memory message, std::optional<std::any> scope);
    std::future<void> enrichConversationMemoryWithRAG(UUID memoryId, std::optional<std::any> ragMetadata);
    void setPendingRAGMetadata(const std::any& ragMetadata);
    std::future<void> enrichRecentMemoriesWithPendingRAG();
    std::future<void> processCharacterKnowledge(const std::vector<std::string>& items);
    std::future<void> processDocumentFragment(Memory fragment);
    std::future<std::vector<Memory>> splitAndCreateFragments(KnowledgeItem document, double overlap, const std::any& scope);
    std::future<std::vector<Memory>> getMemories(std::optional<std::any> params);
    std::future<void> deleteMemory(UUID memoryId);

private:
    Semaphore knowledgeProcessingSemaphore_;
};


} // namespace elizaos
