#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Knowledge Base Test Helpers
 * Utility functions for testing knowledge base CRUD operations
 */

struct KnowledgeDocument {
    std::string id;
    std::string title;
    std::optional<std::string> description;
    std::string createdAt;
};

struct KnowledgeChunk {
    std::string id;
    std::string text;
    std::optional<std::vector<double>> embedding;
};

struct KnowledgeSearchResult {
    std::string text;
    std::optional<double> similarity;
};

struct UploadResponse {
    bool success;
    std::string id;
    std::optional<std::string> message;
    std::string message;
    std::optional<std::string> code;
};

class KnowledgeTestHelper {
public:
    KnowledgeTestHelper(std::string agentId = "00000000-0000-0000-0000-000000000001");
    Cypress::Chainable<UploadResponse> uploadFromUrl(const std::string& url);
    Cypress::Chainable<KnowledgeDocument[]> getDocuments();
    Cypress::Chainable<KnowledgeChunk[]> getDocumentChunks(const std::string& documentId);
    Cypress::Chainable<KnowledgeSearchResult[]> search(const std::string& query, double count = 10);
    Cypress::Chainable<void> deleteDocument(const std::string& documentId);
    Cypress::Chainable<void> waitForDocumentProcessing(const std::string& documentId, double maxWaitTime = 30000);
    Cypress::Chainable<boolean> verifyDocumentExists(const std::string& documentId);
    Cypress::Chainable<void> cleanupTestDocuments(std::vector<std::string> testFileNames = {});
    void validateApiResponse(const std::any& response, bool expectedSuccessStatus = true);

// Cypress commands for easier usage in tests
    struct Chainable {
    std::optional<std::string> fileType;
};

// Register custom commands


} // namespace elizaos
