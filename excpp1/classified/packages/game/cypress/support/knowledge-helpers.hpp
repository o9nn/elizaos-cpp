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
    KnowledgeTestHelper(string = '00000000-0000-0000-0000-000000000001' agentId);
    Cypress::Chainable<UploadResponse> uploadFile(const std::string& fileName, const std::string& content, string = 'text/plain' fileType);
    Cypress::Chainable<UploadResponse> uploadFromUrl(const std::string& url);
    Cypress::Chainable<KnowledgeDocument[]> getDocuments();
    Cypress::Chainable<KnowledgeChunk[]> getDocumentChunks(const std::string& documentId);
    Cypress::Chainable<KnowledgeSearchResult[]> search(const std::string& query, number = 10 count);
    Cypress::Chainable<void> deleteDocument(const std::string& documentId);
    Cypress::Chainable<void> waitForDocumentProcessing(const std::string& documentId, number = 30000 maxWaitTime);
    Cypress::Chainable<boolean> verifyDocumentExists(const std::string& documentId);
    Cypress::Chainable<void> cleanupTestDocuments(const std::vector<string[] =>& testFileNames);
    Cypress::Chainable< createTestDocuments();
    void validateApiResponse(const std::any& response, boolean = true expectedSuccessStatus);
    Cypress::Chainable<string[]> testFileTypes();

private:
    std::string agentId_;
};

// Cypress commands for easier usage in tests
    struct Chainable {
    std::optional<std::string> fileType;
};

// Register custom commands


} // namespace elizaos
