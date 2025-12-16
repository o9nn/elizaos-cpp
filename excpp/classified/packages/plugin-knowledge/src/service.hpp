#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    std::future<KnowledgeService> start(IAgentRuntime runtime);
    std::future<void> stop(IAgentRuntime runtime);
    std::future<void> stop();
    Promise< addKnowledge(AddKnowledgeOptions options);
    void catch(auto error);
    Promise< processDocument(passedAgentId {
    agentId, auto clientDocumentId, auto contentType, auto originalFilename, auto worldId, auto content, auto roomId, auto entityId, auto metadata, AddKnowledgeOptions });
    void if(auto isPdfFile);
    void catch(const std::any& e);
        // Routes always send base64, but docs-loader sends plain text

        // First, check if this looks like base64
    void catch(const std::any& error);
    void handleProcessingError(const std::any& error, const std::string& context);
    std::future<bool> checkExistingKnowledge(UUID knowledgeId);
    std::future<void> enrichConversationMemoryWithRAG(UUID memoryId, std::optional<std::any> ragMetadata);
    void setPendingRAGMetadata(const std::any& ragMetadata);
    std::future<void> enrichRecentMemoriesWithPendingRAG();
    std::future<void> processCharacterKnowledge(const std::vector<std::string>& items);
    void if(auto existingDocument);
    void for(auto const fragment of fragments);
    std::future<void> processDocumentFragment(Memory fragment);
    std::future<void> deleteMemory(UUID memoryId);


} // namespace elizaos
